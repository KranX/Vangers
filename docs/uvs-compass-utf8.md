# UVS display names and compass captions

Stage 6a, based on integration `feature/text-rendering` at `9110c9b`.
This is a presentation/identity boundary migration, not a rewrite of UVS game
identifiers or the network protocol. Enable `video.ttf_text` for new alphabets;
bitmap output deliberately keeps its existing lossy CP866 presentation adapter.

## Identities are not translations

UVS `name`, `uvsGetNameByID`, tabutask numeric ranges, first-character Diagen
lookup tags, passenger comparisons, world/resource paths and compass `Name`
remain opaque game identifiers. In particular, do not replace `NameFrom` or
`NameTo` with a translated town name: their first bytes select inventory dialogue.
No resource, save-game or wire-format migration is necessary for this change.

The compass previously deduplicated and selected destinations by their displayed
CP866 caption. Translating two captions identically could therefore merge targets
or select the wrong destination; an unrepresentable caption could not round-trip
through the UI's UTF-8 → CP866 selection call.

Each compass target now owns an opaque `key` separately from its owned UTF-8
`aciName`. `GetCompasTarget` returns the borrowed **key**, not the caption.
`fncMenuItem::compass_id` carries that key through selection, refresh, clone and
removal. An empty menu key means OFF; the translated word OFF is not an action ID.
Deleting the selected destination clears it by identity, even while another row
is hovered. UI labels never enter route/quest lookup.

Keys retain the original semantic deduplication groups: static and artifact
caption-role IDs, task role plus numeric task index, and a checkpoint role. Thus
this does not add a new target for every physical artefact or change target order.
Network phantom captions are decoded once from their existing CP866 input;
the legacy name-based grouping is preserved. Migrating player identity/name
storage and the protocol is a separate stage, not hidden in this change.

## UTF-8 display boundaries

- The 35 existing compass-caption roles and two task-message fragments move
  from CP866 macros in `magnum.h` to 36 catalog entries: 35 `compass.*` roles
  and one whole `uvs.task_complete` message. English/Russian phrases,
  punctuation, spacing (including double spaces), case and line breaks are copied
  verbatim in the default rendered messages; no new translation is supplied.
  Remaining legacy notification macros keep their existing encoding and are
  outside this name-focused stage.
- Static world target creation uses one language-independent switch after
  verifying that both original switches had the same world cases, targets,
  conditions and order. Sensor names and passage tags such as `ArmorUpdate1` and
  `F2G` are unchanged.
- `aciGetLocationNameUtf8` resolves an opaque UVS lookup through the existing
  location's UTF-8 `nameID2`. Unknown legacy/mod lookup names retain their explicit
  CP866 display fallback; there is no encoding guessing. New display fields use
  the existing script `utf8_text` / catalog contract directly.
- Teleport town captions and successful-tabutask text use that display boundary.
  Tabutask selection and reward amount are unchanged. One intended RU correction:
  the success message now uses the existing translated town caption (e.g. Подиш)
  instead of leaking the ASCII lookup tag Podish. Its original phrases/order and
  reward remain unchanged; this is not a new translation.
- Item escave captions already use `nameID2`; runtime regression tests cover that
  UVS-to-inventory path as well as compass and teleport names.

`uvs.task_complete` is a whole template with `{location}` and `{reward}`, each
required exactly once. A translation can reorder them; inserted values are data,
not recursively executed templates. Invalid/missing/duplicate fields fail at
catalog loading. The task caption uses the existing single-integer formatter
(`%d`), so its number can also move. This is a narrow notification formatter, not
a new general text engine; old item-format contracts are unchanged.

Catalog compass captions must be single-line (including Unicode line/paragraph
separators). Oversized captions produce a key/width diagnostic at menu creation;
the model is not truncated. The diagnostic does not silently resize the UI.
Captions remain single-line fields in the same fixed menu. No scrolling, new
controls, automatic shrinking, enlarged panels or changed gameplay are added.
Translations must fit the existing panel. The TTF/material/fonts are unchanged.

## Tests and visual QA

`compass_runtime_test` links real engine objects and exercises add/dedup/select,
refresh, removal (including hover differing from the selected destination), OFF,
cloning, UVS numeric/name lookup, unchanged compass destination resolution, item
escave and teleport captions, and the explicit legacy/mod fallback. Its tiny font
fixture tests model/selection behavior, not native font appearance. A separate
synthetic Japanese catalog deliberately gives multiple destinations and OFF the
same caption; no placeholder Japanese locale is shipped.

The clone regression also exposed a pre-existing leak of a fresh destination's
key object. `fncMenuItem::clone` now releases its destination-owned resources before
borrowing the source's resources, with a self-clone guard. The previous rendering
test's manual-delete workaround is removed, not replaced by a sanitizer exclusion.

The localization conservation test retains every prior group/hash and checks the
36 new entries independently. Font coverage automatically includes the new catalog
strings. Existing source/binary scripts remain readable; keyed SCB recompilation
is performed only in a new isolated runtime data copy for game QA.

Compare RU/EN compass menus with base `9110c9b`, in both bitmap and TTF modes.
Inspect synthetic Japanese captions in the actual game with TTF explicitly enabled.
A GDB preset may exit the initial escave using the existing `aciExitShop` routine,
then open the real target submenu via existing menu methods. Label those frames
as an engine/UI preset, not evidence of physical driving, arrival, teleport use,
network interoperability or IME input. Original private assets/saves must not be
changed. No rendering parameters are modified for captures.

### Verification on 2026-11-15

- RelWithDebInfo, binary scripts: **73/73 tests, three consecutive runs**.
- ASan + UBSan + leak detection + pattern-initialized automatic variables:
  **69/69 tests, three consecutive runs**. Only the four previously documented
  legacy ACS invalid-vptr tests are excluded
  (`acs_script_roundtrip`, `acs_runtime`, `acs_render_bitmap`,
  `acs_render_ttf`); settings and all three new compass tests are included.
- An independent provenance audit against `9110c9b` verified all 35 caption
  defaults in each language, both original notification fragments' literal text
  and order in the whole template, unchanged remaining legacy macros, and
  identical static-world target cases, tags, conditions and ordering.
- Personally inspected actual RU/EN compass frames against that base in bitmap
  and TTF modes, plus Japanese TTF captions with duplicate labels. Captions,
  centering and fixed panel geometry match; the world background is time/RNG
  dependent, so whole-frame byte identity is not asserted. These are the
  engine/UI preset captures described above, not physical gameplay/IME tests.

## Still separate

Player names, chat storage/input, network compatibility, remaining fixed UVS/
multiplayer notification text, complete third-locale translation and physical IME
acceptance remain work for subsequent functional stages. This PR does not claim
that every UVS string or the entire game has been localized.

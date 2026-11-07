# Stable dialogue identities

Stage 2 of the [localization roadmap](text-localization-roadmap.md).
Base: `feature/text-rendering` at `73cabb9` (PR #698).

## Runtime contract

`dgQuery::id` identifies a question independently of its displayed `subj`.
Phrase/answer links are resolved to `dgAtom::question_ids` once after loading
all rooms. Opening questions, propagating their visibility between rooms,
selecting an answer and saving progress use these IDs, not translated text.

The question menu still displays the same strings, fonts, positions and arrows.
Each `fncMenuItem` additionally owns a `dialogue_id`; `aciAskQuestion` submits
that payload through `getAnswerById`. `fnc_code` retains its normal menu meaning.
Two questions can have identical labels without selecting each other's answers.

Variant choices already have separate action codes/comments in their atom.
Their menu payload is a transient `variant/<index>` within that active atom,
not the displayed label. It is not a persistent, cross-resource identity;
pending variant selection was not saved before this change either. Translators
change variant labels/comments, not the association with scenario action codes.
Invalid payloads neither consume a choice nor send its events. The text-based
`getAnswer` facade is retained only for the interactive legacy debug console.

## New resources

The existing two-header question grammar is retained, with an explicit ID in
the first header and the localized label in the second:

```text
utf8_text
"What is"
"?"
[#topic.example]
[Localized question label:B]
"An answer in the selected language."
{#topic.another}
[#topic.another]
[Another localized label]
"Another answer."
```

Use `{#topic.example}` for links from phrases or answers. Explicit IDs are
nonempty printable ASCII, excluding `#[]{}:`; `variant/` is reserved for the
transient menu payload. Use short, descriptive keys such as `topic.example`.
The shipped keys use the `topic.` namespace, e.g. `topic.mechos` and `topic.fostral`. IDs must be
unique **within a room**; the same topic can have different answers in different
rooms. An explicit link opens that ID wherever it is defined, just as legacy
links could open questions in several rooms. Unknown explicit links and
duplicate/invalid IDs are diagnosed at resource loading, not silently ignored.

The localized second header is used for every locale; no alphabet-based guess
is needed for this syntax. Mood suffixes (`:G`, `:B`) belong to the display
header, never the ID. The existing UTF-8 encoding and archive layout are retained:
`utf8_text` source compiles to the existing `DU` UTF-8 archive format. This is
not a new localization-package loader or catalog system. The new question grammar
requires a stable-ID-capable executable; older builds need their original data.

## Shipped resources and historical compatibility

Both packaged archives are now `DU` UTF-8. All **619 questions per locale**
carry explicit IDs (89 distinct concepts), and all effective question links use
`{#id}`. The authoritative sources are `data/data/ru/` and `data/data/en/`;
shared scenario metadata remains outside the locale folders. The source compiler
keeps logical archive keys unchanged. Rebuild instructions and the byte-identical
source/archive check are in [`data/data/README.md`](../data/data/README.md).

For example, `[#topic.mechos]` is followed by `[мехос]` or `[mechos]`. Changing
those display labels cannot alter the ID. No current shipped question derives
its identity from an English header or from the compatibility table.

`src/uvs/legacy_topics.inc` freezes 90 historical RU/EN header pairs and their
explicit canonical IDs. It is an **input/save adapter**, not a display catalog.
The original `DG` archives still load through that adapter; unknown historical
mod topics retain their two-header `legacy/<EN header>` fallback contract.
New resources must use explicit IDs. Do not edit old aliases when translating.

The adapter resolves old text links using their active-language labels. The
resulting canonical IDs also unify the formerly split soup topic. When questions
use exclusively explicit IDs, unkeyed text links
are rejected; there is no hidden lookup by translated label. For an old mixed
archive, label lookup applies only to topics that still have legacy headers.
An empty historical question set remains valid; its unresolvable old labels are
ignored, while unknown explicit `#id` links still fail.

## Russian canonical structure

Russian is the authoritative version. English source bugs are repaired rather
than frozen as compatibility behavior. Both locales use one `topic.creature_soup`
concept; the old English article no longer partitions topic visibility.
Missing/joined EN links, wrong answer segmentation, duplicate phrases and inventory
text truncation are repaired using existing English wording. Two malformed choice
menus are repaired in both languages with the user's approval; no phrases or action
codes are replaced. The complete audit and pending translation list are in
[`diagen-english-audit.md`](diagen-english-audit.md).

Tests compare the Russian corpus with its historical model plus those two explicit
markup fixes. English structure (including ordered links, levels, substitutions and
actions) must match Russian. Its existing wording is separately checked for
conservation, with only the reviewed duplicate removals and numeric correction.
Historical fixtures stay immutable: they are input evidence, not the desired EN
scenario structure.

## Saves

The nested Diagen save version is **5**. Room and molecule keys were already
separate from display names; their layout is unchanged. Visible questions now
store their ID and answer-progress index instead of the localized label.
The surrounding game/save-name format is not changed here. Old executables
are not expected to read this new nested version.

Versions 2/3 (CP1251 topic labels) and 4 (UTF-8 labels) still load into the new
explicit-ID resources, in either locale. Import is per room, using historical headers/frozen aliases before considering the current
display label. This prevents a new label's accidental collision from stealing
another topic's old progress. Ambiguous fallback labels are diagnosed.
Version 5 does not guess from display labels: a missing saved ID is an error,
not silently discarded progress. Negative answer indices are invalid.

Historical English resources incorrectly split 18 Russian answers into multiple
progress slots. `legacy_answer_indices.inc` freezes their fragment-to-canonical
mapping. English v2/v3/v4 subjects import through that mapping; Russian subjects
already have canonical indices. Known unmarked legacy EN answer arrays are joined
once at the input boundary, with strings owned by the question. Explicit resources
are already canonical and bypass this adaptation. New v5 saves use canonical slots.

For example, the old ZeePa `Chomper` had two EN fragments but one RU answer; both
old EN indices now refer to canonical answer 0. This is not the same as clamping
an index: intermediate old slots must not unlock a later canonical reply.
The generic bounded-display fallback still protects incomplete custom resources,
without discarding a saved out-of-range progress index.

## Verification

- Production Diagen parser/dispatcher/save tests: explicit links, renamed and
  reordered questions, duplicate question/variant labels, correct variant
  action code, invalid selections, source-to-`DU` round trips and diagnostics
  for malformed IDs, duplicate IDs, missing explicit links/saved IDs and
  negative answer indices.
- Every shipped topic is exercised in RU/EN. Russian content is compared with its historical model plus the approved markup
  corrections; English structure must match Russian, not its old defects. Saves v2/v3/v4/v5 switch locale in
  both directions, retain question IDs/indices/counters and return to the
  original locale without losing canonical progress. All 147 legacy EN index/version
  cases are checked in both languages and through a subsequent v5 save.
- The old grid format stores progress by molecule name, not individual cell
  index. Shipped-cell progress is compared with that existing reader, while
  every original question/index is independently required to survive.
- Synthetic fixtures check cell states and quest status as well as answer
  indices, and import old RU/EN labels into renamed explicit-ID resources.
- Production bitmap/TTF menus retain independent payloads under equal labels;
  cloning retains the payload and selection does not change rendered pixels.
- Source compilation for both shipped locales must reproduce the packaged
  archives byte for byte. No generated output is written into the source tree
  during testing.
- Full release CTest suite and strict ASan/UBSan/leak run. The latter excludes
  four previously documented ACS invalid-vptr tests at `aci_scr.cpp:319`, not
  any Diagen or changed menu test.
- Real game RU/EN 800x600 captures are personally inspected against an immutable
  pre-stage binary. Opening the question menu and asking questions must retain
  the accepted appearance; rendering/material/click animation is not retuned.

Builds, copied runtime assets, captures and logs stay outside the repository.
SDL dummy/software event injection verifies these UI paths, not physical
keyboard/IME input or an entire campaign. Rare repaired stock choices and replies
are additionally previewed through the production panels under debugger control;
this is not a claim of completing their quest prerequisites or visiting every room.
Fonts, third-locale packages, UVS/
network named paths and AVI-based descriptions remain later roadmap stages.

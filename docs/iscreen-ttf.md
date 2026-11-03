# Iscreen: UTF-8 text and TTF outlines

This stage covers iscreen's menu, settings/controls, history, credits, text pages,
load-slot/server-list labels, static mouse prompts and editable fields. It builds
on actint's shared SDL3_ttf implementation rather than replacing the game UI.
ACS pause/save editing, chat, Diagen and service screens remain separate legacy
subsystems. No new translation or complete additional-language font coverage is
claimed.

## Enable

Use the existing setting and restart:

```toml
[video]
ttf_text = true
```

The default remains `false`. UTF-8 storage works in either renderer; disabling
TTF encodes only at bitmap presentation. Neither setting rewrites game resources,
saves or network packets. The approved Menu and Display faces are embedded next
to Text, so existing data installations need no separately copied TTF files.
See [font provenance](../data/resource/actint/fonts/README.md).

## Relief and clicks are not a flat shadow

- `hfont01` selects **Menu**; `hfont00` selects **Display**. The smaller
  `hfont03/04` roles use Display with their original dimensions and height maps.
  The icon-only `hfont02` and control cells remain bitmap.
- TTF coverage defines the glyph footprint. Original signed relief heights are
  retained inside it, extending the profile into newly covered samples instead
  of clipping the TTF to the old silhouette. New letters without a source profile
  get a neutral raised profile. Heights below 128 are preserved, not normalized
  into positive opacity.
- Existing glyph advances, spacing and hit boxes remain tied to the original
  resources; newly supplied glyphs use TTF metrics where the old cell was blank
  or CP866 cannot represent the codepoint. CP866's replacement `?` is never used
  as a new letter's source metric or relief profile.
- Output still uses `scale_buf`, `smooth_buf`, `put_buf`/`put_tbuf` and
  `iregRender`: terrain, illumination, shadows, masks, negative scale, hide mode
  and background restoration retain their existing meanings.
- No click script, event command or timing is changed. This includes the
  seven-tick press/release and blur schedule in `buttons.inc`; it is not replaced
  with a new interpolation/easing function.

**Resolution boundary:** flat iscreen text and prompts use the existing native
output companion layer. Relief is rasterized into the original logical height
map before terrain illumination. Enlarging the window therefore still enlarges
that relief map. This is not a fully native-resolution relief renderer; that
requires a separate height/lighting/compositing change, not a fake shadow overlay.
Original `.fnh` assets must not be removed.

## Encoding boundaries

Inside iscreen, display strings, prompts, page data and editable fields are
canonical UTF-8. Fixed buffers reserve four bytes per character plus cursor/NUL;
limits count complete Unicode scalars, not UTF-8 bytes. This is not grapheme-aware
editing or an IME implementation.

Old script/text resources remain CP866 and are decoded when loaded. New scripts
can put the same directive used by actint before their display text:

```
utf8_text
```

It also applies to included source text, `text`/`s_text` files and subsequent
`text_object` resources. Those bytes are validated as UTF-8, never transcoded
through CP866. Paths, identifiers and script commands are not display strings.
The flag is appended to the SCB opcode table; existing opcode numbers and old
SCB files are unchanged. The compiler stores resource bytes plus the explicit
encoding flag, and the runtime decodes once when constructing its model.

- SDL text-input events are consumed in full. Backspace/left remove one scalar;
  Enter removes only the cursor; Escape restores the UTF-8 backup.
- Blank page lines, CRLF/LF and the final line without a newline are preserved.
- `iGetOptionText` / `iSetOptionText` are canonical UTF-8 APIs, including the
  settings adapter. Unsupported glyphs must survive a settings round-trip.
- `iGetOptionValueCHR` / `iSetOptionValueCHR` remain explicit CP866 compatibility
  boundaries for network and other legacy consumers. Cached legacy name views
  are refreshed when values or linked options are committed.
- Legacy save headers and network result/server labels decode at entry. Save
  formats and the network protocol are not migrated by this change.
- Missing font glyphs use `?` for presentation only. German completeness,
  shaping, fallback font stacks and new translations remain future work.

The goal is additional languages, not a new CP866 renderer with UTF-8 wrappers.
Legacy glyph profiles are a compatibility adapter, not a Unicode repertoire
limit. A regression uses U+E000 (present in TTF, absent from CP866) to verify
independent TTF metrics and generic relief; Japanese/emoji fixtures verify lossless
storage, **not** visual coverage. The bundled faces still cover Latin/Russian,
and the current scalar layout does not implement shaping, bidirectional text or
grapheme editing. Those must be addressed before claiming support for scripts
that need them. Resource layout/translation and fallback font selection are also
separate work; enabling TTF alone does not translate the game.

## Verification

The out-of-tree CTest suite includes production source/SCB parser round-trips,
legacy and explicit UTF-8 resource/page loading, complete SDL input events for
both field types, scalar deletion/cancel/commit, canonical settings versus legacy
network views, signed relief, original scale/blur operations and the actual
14-frame button command sequence. Tests generate their own assets outside the
repository; private game data is not required.

All **24/24** tests pass in RelWithDebInfo and Debug ASan+UBSan builds (leak
detection enabled). Both original RU/EN source scripts also compile to SCB with
the rebuilt production compiler.

Manual visual QA uses isolated copies of game data. On 2026-11-03:

- RU bitmap menu, settings, credits, credits detail and history were pixel-exact
  against the pre-change binary (`682d5ef`).
- Actual RU/EN TTF menu, settings, credits and history were inspected; flat-text
  output was also captured after resize to 1200×900 and 1600×1200.
- RU graphics, sound, controls/category and basic key-binding screens, plus
  empty load slots, were also captured and inspected.
- Press and return transitions were captured frame-by-frame against the original;
  intermediate relief depth, blur, transition timing and restored background were
  checked rather than only the final menu frame. The selected/restored comparison
  has a one-pixel residual in TTF versus three in the original at the same edge;
  this is not a claim of pixel-identical TTF output or perfectly reversible legacy
  terrain lighting.

Captures/builds/logs are external, not vendored game assets:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-iscreen-ttf-NQ16Anv9`.
The SDL dummy/software event observer is not a physical mouse, GPU, IME or live
multiplayer acceptance test. The legacy non-virtual tagged UI object ownership
is not refactored here; fixtures delete their known element types explicitly so
sanitizers stay enabled without hiding that pre-existing teardown limitation.

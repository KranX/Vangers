# Iscreen: UTF-8 text and procedural TTF relief

This stage covers iscreen's menu, settings/controls, history, credits, text pages,
load-slot/server-list labels, static mouse prompts and editable fields. It builds
on actint's shared SDL3_ttf implementation rather than replacing the game UI.
[ACS pause/save editing](acs-ttf.md) now has its own UTF-8/TTF path.
Chat, Diagen and service screens remain separate legacy subsystems. No new translation or complete additional-language font coverage is
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
  `hfont03/04` roles use Display with their original dimensions.
  The icon-only `hfont02` and control cells remain bitmap.
- TTF coverage generates heights directly: a 4x mask, a two-pass chamfer
  distance field and a rounded bevel. Smooth shape variation and deterministic
  glyph-local grain produce the surface; grain is added after downsampling so
  it is not averaged away with contour antialiasing. Menu/Display use two style
  calibrations of the same algorithm, never per-language/per-letter rules.
  Grain amplitude is 2.1 height units for Menu and 0.945 for Display: 30% below
  the initial grain version, with the same pattern, bevel and broad variation.
- No original letter height profile is read, copied, interpolated or filled in
  the TTF path. The generic terrain renderer still provides lighting and shadows;
  this does not replace them with a flat colour-noise/shadow overlay.
- Existing glyph advances, spacing and hit boxes remain tied to the original
  resources; newly supplied glyphs use TTF metrics where the old cell was blank
  or CP866 cannot represent the codepoint. CP866's replacement `?` is never used
  as a new letter's source metric.
- Output still uses `scale_buf`, `smooth_buf`, `put_buf`/`put_tbuf` and
  `iregRender`: terrain, illumination, shadows, masks, negative scale, hide mode
  and background restoration retain their existing meanings. The TTF object
  redraw also copies the exact relit scanline spans outside the nominal object,
  including wrapped shadow tails, without copying stale pixels in a larger
  rectangular bounding box. Bitmap-mode redraw remains unchanged.
- No click script, event command or timing is changed. This includes the
  seven-tick press/release and blur schedule in `buttons.inc`; it is not replaced
  with a new interpolation/easing function.

**Resolution boundary:** flat iscreen text and prompts use the existing native
output companion layer. Relief is rasterized into the original logical height
map before terrain illumination. Enlarging the window therefore still enlarges
that relief map. This is not a fully native-resolution relief renderer; that
requires a separate height/lighting/compositing change, not a fake shadow overlay.
Original `.fnh` assets are still needed for compatibility metrics, the bitmap
fallback and non-text icons, but not as a source of TTF letter heights.

### Cache and cost

Generation is linear in mask area and runs once per glyph/size. Cached height
vectors are returned by const reference; cache lookup precedes any output-buffer
allocation. Inserting another size does not invalidate existing references.
Unclipped drawing consumes the cached vector directly; only actual clipping or
animation needs temporary buffers. Cache lifetime follows the font/interface.
There is no per-frame random generation, new dependency or general material engine.

A standalone regression counts allocations during 1,000 warmed height lookups:
zero allocations, no copies and no additional rasterization. This is a cache-path
check, not an FPS benchmark or a claim that a whole frame allocates nothing.

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
Legacy metrics are a compatibility adapter, not a Unicode repertoire limit. A regression uses U+E000 (present in TTF, absent from CP866) to verify
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
network views, procedural relief, signed bitmap relief, original scale/blur operations and the actual
14-frame button command sequence. Tests generate their own assets outside the
repository; private game data is not required.

All **27/27** tests pass in RelWithDebInfo and Debug ASan+UBSan builds (leak
detection enabled). Both original RU/EN source scripts also compile to SCB with
the rebuilt production compiler.

Initial-stage visual QA used isolated copies of game data. On 2026-11-03:

- RU bitmap menu, settings, credits, credits detail and history were pixel-exact
  against the pre-change binary (`682d5ef`).
- Actual RU/EN TTF menu, settings, credits and history were inspected; flat-text
  output was also captured after resize to 1200×900 and 1600×1200.
- RU graphics, sound, controls/category and basic key-binding screens, plus
  empty load slots, were also captured and inspected.
- Press and return transitions were captured frame-by-frame against the original;
  intermediate relief depth, blur, transition timing and background were checked.

The procedural-height/cache update (`c045a2b`) was verified against the chosen
external grain prototype: all 73 captures per language (RU/EN, including press
and return frames) are pixel-identical after the cache/drawing optimizations.
All 73 bitmap-mode captures also match the previous branch binary (`de4a43f`).
The updated RU menu, settings, credits, credits detail and history were visually
inspected in the actual game.
Selected/restored captures match exactly in that scenario. This is not a claim
of pixel-identical TTF versus original bitmap contours or universally reversible
terrain lighting. The original seven-tick click/blur schedule remains tested.

The subsequent 30% grain reduction was visually checked against that version
and the original bitmap renderer. RU/EN press/return captures retain the same
transition timing and return to the exact selected frame in this scenario;
all 73 bitmap-mode captures remain unchanged. Both 27-test configurations pass.

Additional regressions cover generation with legacy `HeightMap = nullptr`,
non-CP866 outlines, empty masks, solid-glyph edge bevels, deterministic cache
regeneration/reference stability, and shadow-copy spans at a wrapped map edge.
No experimental CJK font additions, environment switches or diagnostic probes
are part of the implementation.

Captures/builds/logs are external, not vendored game assets:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-iscreen-ttf-NQ16Anv9`.
Procedural/cache follow-up captures and logs:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-relief-publish-zZlz0VXD`.
Grain-reduction comparison:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-relief-grain70-H8w1WxT5`.
The SDL dummy/software event observer is not a physical mouse, GPU, IME or live
multiplayer acceptance test. The legacy non-virtual tagged UI object ownership
is not refactored here; fixtures delete their known element types explicitly so
sanitizers stay enabled without hiding that pre-existing teardown limitation.

# ACS: UTF-8, TTF and save titles

This stage migrates ACS pause/save screens, their labels, toggles and editable
fields as a complete subsystem. It reuses the existing `video.ttf_text` setting
(default false, restart required), embedded fonts and native text companion layer.
No resource rewrite, new dependency, dialogue/network migration or additional
language coverage is included.

## Text and presentation

- ACS strings are UTF-8, including source-code toggle/slot labels and loaded save
  titles. Old source/SCB display strings decode from CP866 once at construction.
- A bare `utf8_text` directive selects validated UTF-8 for subsequent display
  strings, including includes. Its opcode is appended; old opcode numbers and
  binary scripts are unchanged. Filenames, IDs and commands are not display text.
- Fixed buffers reserve four bytes per Unicode scalar plus cursor/NUL.
  `MaxStrLen` counts scalars, not bytes. Full SDL text events are handled;
  backspace removes a scalar, Escape restores the backup, Enter removes only
  the cursor. Centering reserves a constant cursor width while blinking. Invalid UTF-8 and input control characters are rejected.
- `ifont` uses the existing Display TTF at the ACS cell dimensions (20x39/16x31).
  Its fixed cell pitch is preserved instead of using proportional TTF advances.
  ACS uses palette ranges, NOT iscreen height-map relief. Its TTF material now
  reproduces a raised face, projected dark body and grain from the
  glyph mask (see below). Colour states and centered alignment share the same layout. Control/icon cells stay bitmap.
  `font` fields reuse the existing actint UTF-8/TTF entry points.
- Bitmap mode encodes at presentation only and retains the existing drawing
  algorithm. Missing TTF glyphs affect presentation, never stored text.
- Background capture/restoration uses the existing XGR/native layer transport.
  Rectangle erasure also clears native text coverage, preventing stale overlays.
  Each field retires its own previous native draw before repainting. This removes
  layout padding/overhang outside the restored background without clipping glyphs
  or erasing neighbouring text. Weak handles do not extend font/run lifetimes.
  This change does not replace decorative images or text baked into images.

This is scalar editing, not grapheme editing, shaping, bidi or physical IME
acceptance. The bundled fonts do not gain Japanese/emoji glyphs from this change.

## Save compatibility — approved direction

The new game reads both old and new saves. **Opening new saves in older game
versions is not supported.** In particular, old fixed-size title buffers were not
UTF-8-aware. The user explicitly approved this one-way compatibility requirement.

The existing four-byte packed header retains its layout:

- bits 0..7: name byte length;
- bits 16..23: description byte length;
- bit 24: existing payload-compression flag;
- bit 25 (`0x02000000`): new explicit UTF-8 flag for BOTH text fields.

New manual saves and autosaves write validated UTF-8 names and location/time
labels with bit 25 set. Files without this flag are decoded as CP866; valid-looking
UTF-8 bytes in an old file are not autodetected. The gameplay payload, its offset
calculation and compression format do not change. Compression/decompression
preserve the encoding flag. Unaligned packed-size accesses in those routines
now use byte copies rather than undefined typed accesses.

The existing 255-byte limit per metadata field remains and is checked before the
destination is opened, not enforced by truncation. Shipped editable save titles
allow 15 scalars (at most 60 UTF-8 bytes). New resources must respect these header
limits. ACS and the main-menu load screen share one bounded header reader;
truncated headers and malformed flagged UTF-8 are rejected instead of overflowing
UI buffers or guessing another encoding. No sidecar or trailing metadata block
is used.

## Tests

Production-linked tests cover source/SCB parsing of old and explicit UTF-8
resources, includes and contextual invalid-resource errors; both field editing
and full four-byte input at limits 0/1/20/511; toggle strings; aliased copies;
bitmap width/centered pixel parity; native-layer output and palette range;
Unicode metadata round trips and real compression/decompression cycles with
unchanged gameplay bytes. Header tests include legacy/explicit encoding,
255-byte boundaries, missing/truncated files, embedded NUL and malformed UTF-8.

Successful paths retain ASan/UBSan and leak checks. The deliberately fatal parser
subprocess disables leak-at-exit reporting, following the existing parser-test
convention; production error diagnostics and exit status are still required.

Builds, captures and private QA assets remain outside the repository:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-acs-utf8-ttf`.
SDL dummy/software captures are not GPU/physical keyboard/IME acceptance tests.

### Verified on 2026-11-03

- 33/33 CTest passed in RelWithDebInfo and Debug ASan+UBSan.
- Actual RU/EN pause screens and the RU save screen were captured and visually
  inspected at 800x600. Follow-up review corrected the initial EN save-screen
  claim: that automation capture was still in the escave, not the save screen. The bitmap pause-control and save-label comparison regions matched
  the pre-ACS binary exactly (0 differing pixels); animated escave backgrounds
  and clocks are not claimed pixel-identical.
- A real manual save named `Ёж日本😀` and a Russian autosave have UTF-8 headers
  (flags 3 = compressed + UTF-8). The manual save was selected in the main menu
  and loaded through `acsLoadData`/`DiagenDispatcher::load`.
- An archived CP866 save (flags 1) was also selected and successfully loaded in
  the new game. No reverse-compatibility claim is made.
- Self-review additionally caught and fixed centered-text movement during cursor
  blinking, stale native coverage after rectangle erasure, and ACS palette-zero
  transparency for retained bitmap icons. These have rendering regressions.
- Follow-up detailed review reproduced stale native draws outside a field's BMO
  restoration rectangle. Per-field draw replacement fixes it without clipping
  letters or changing bitmap output;
  a production-linked regression checks 128 redraw/clear cycles, both ACS sizes,
  short and over-wide strings, parent-clip preservation and 1x/2x rasterization.
  Layer tests also verify that replacement preserves overlapping neighbours and
  captured backgrounds.

These are scripted SDL software-renderer checks, not a manual playthrough.

## Procedural ACS surface (visual follow-up)

The original FNC letters contain varying palette levels, not a uniform silhouette:
bright edges, darker faces and a small dark offset. Filling the TTF mask with the
maximum ramp level flattened this material. `FontStyle::embossed` is opt-in and
is enabled for ACS `ifont` only; bitmap mode, icons, ordinary actint text and the
iscreen terrain/relief renderer retain their existing presentation.

### Layout and material

- `fixed_pitch` restores the ACS advance to `cell_width + spacing` per Unicode
  scalar, including spaces/cursor and fallback characters. It does not stretch
  individual outlines. Both ACS sizes (20x39 and 16x31), measuring and centering
  share this layout. Other TTF roles remain proportional; bitmap icons keep their
  existing bearing/advance rules.
- At rasterization, a 0.2-logical-pixel optical dilation strengthens thin strokes
  within their TTF ink box. It samples the unmodified mask before updating the
  returned SDL glyph surface, never the font asset or SDL_ttf's glyph cache.
- The shared two-pass chamfer transform has an opt-in fractional-coverage seed.
  Partial pixels start at `alpha/255`; opaque pixels start at their distance to
  the image boundary. The existing two sweeps propagate fractional edge distances
  into solid neighbours too. Default thresholded seeding is unchanged for iscreen.
  This is an antialias-aware edge approximation, not an exact signed-distance field.
  ACS uses a one-logical-pixel linear bevel:
  `min(1, distance/min(scale_x,scale_y))`.
- Height is multiplied by `1 + 0.1 * smooth_grain(x/scale_x,y/scale_y)`.
  With depth 4 this is absolute grain amplitude 0.4. The height, grain and face
  base lighting are restored from the user's preferred five-cycle candidate, not
  the rejected brighter ten-cycle variant. Away-facing slopes are additionally
  darkened below; this is not a general increase in face brightness.
- Central height differences at +/-0.5 logical pixels, multiplied by 4, give the
  surface slopes. Light is normalized `(-1,-0.1,0.45)`. A fixed half-vector
  highlight has exponent 16 (four squarings). Tone is
  `clamp(round(6 + 10.8*clamp(height,0,1) + 12.96*max(0,N.L)
  + 12*max(0,N.H)^16 - 12.96*shade),2,31)` before the existing palette shift,
  where `shade=clamp(flat_incidence-N.L,0,1)` and
  `flat_incidence=0.45/length(light)`. Flat faces and slopes with at least that
  incidence keep their old brightness. Less-lit slopes darken inside the glyph
  instead of extending the silhouette. This is a stylized contrast treatment,
  not a physically traced self-shadow or a general material engine.
- Sixteen bounded height samples project a dark body down-left, with offsets
  `(3.25*z,-1.25*z)` in logical pixels and a 0.125-height-unit transition.
  Tone 2 and the 15% opacity boost are retained. Coverage comes from the height
  intersection, not a second multiplication by source alpha. This is a stylized
  oblique extrusion, **not** a physically traced cast shadow towards the light.
  An exact early rejection skips rays entirely outside bilinear field support;
  full occlusion also exits early. No extra cache or language-specific rule is used.
- For small native glyphs, only the **front coverage** gets the existing S-curve:
  `a' = a + strength*a*(1-a)*(2*a-1)`, where `a=alpha/255` and
  `strength=clamp((64-native_cell_height)/24,0,1)`. A second curve uses strength
  `strength*clamp((local_peak-128)/127,0,1)`, where the peak is read from the
  original 3x3 alpha neighbourhood. Thus extra edge contrast fades out where
  fine strokes have no opaque core; blindly repeating the curve weakened thin
  CJK glyphs. No alphabet, font-name or codepoint rule is used.
  Transparent/opaque values stay unchanged; partial values remain 1..254 and
  the half-coverage support is preserved. Height, lighting and body generation
  use the mask before these coverage adjustments. Large text retains ordinary
  antialiasing; other TTF roles are unaffected.
- Five logical pixels of layout padding retain the short body without changing
  advances or hit boxes. Per-field image replacement removes it on redraw without
  clipping overhanging letters or neighbouring fields.

The small noise and distance helpers are shared with iscreen in `text/grain.h`
and `text/inside_distance.h`; its material constants and arithmetic are unchanged.
A compiled comparison against the published iscreen implementation produced
byte-identical height maps for 48 synthetic masks/size/material combinations,
including solid, empty, edge-touching and perforated masks. No original letter
pixels/heights are sampled for ACS material: a newly supplied Unicode outline
gets the same treatment. This does not provide missing glyphs, shaping or translations.

### Earlier iterations and verification (historical)

After restoring fixed pitch, five material passes followed the earlier
alpha-gradient version: height-profile lighting (first too dark, then too uniform);
stronger height grain; optical stroke compensation; and interpolated grain for
enlarged output. Actual
RU pause/save screens were inspected at 800x600 and 1600x1200; EN pause at 800x600.
The EN save capture still shows the escave, so it is **not** claimed as verified.
The final cached renderer was captured again: the pause-label, save-heading and
empty-slot comparison crops were pixel-identical to the uncached version.

The result restores the original word rhythm, stroke weight and a rough bevel.
It is not a pixel-identical recreation: some Display contours are rounder/more
regular than the original FNC letters. The three font assets themselves are
unchanged; further contour tuning is a separate visual decision, not something
hidden by more noise.

33/33 CTest pass in RelWithDebInfo and Debug ASan+UBSan. Tests cover fixed pitch
at both ACS sizes, Unicode scalar counting, unchanged default proportional
outlines, optical non-erosion, palette ramps/fades, stable grain, native/fractional
sizes, cursor centering, weak ownership and repeated BMO redraw/clear cycles.
They also guard the shared distance boundary/alpha threshold, noise interpolation
and surface-cache reuse across size changes. SDL software captures do not replace
GPU, physical keyboard or IME acceptance.

### Cost and lifetime

Each immutable ACS run retains only its **last native-size raster**, containing
palette indices/coverage, not RGB. Palette fades are resolved at presentation.
A size change replaces the cached raster; a replaced field releases its old run
when no captured background still owns it. No global/unbounded glyph cache,
new dependency or rendering engine is introduced. A cache hit still copies the
returned pixel vector under the existing `Image` API; it does not regenerate
masks, distance fields or lighting. This is allocation-tested.

Before the edge-contrast follow-up below, a local release CPU microbenchmark used
eight separate eight-letter runs,
200 steady frames and three trials (median). Flat/surface steady rendering was
0.165/0.165 ms at 800x600 and 0.653/0.702 ms at 1600x1200. First surface rendering
was about 2.02/6.70 ms respectively. This is not gameplay FPS or a cross-platform
guarantee; rebuilding runs or resizing still pays the first-render cost.

Build logs, all rejected/intermediate captures, final comparisons, the parity
probe and benchmark sources remain outside the repository:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-acs-refine-hrQbmTBv`.

### Edge-contrast follow-up

The user approved the improved material but found the edges still too soft.
Two edge-contrast strengths were rendered and visually compared at 800x600;
full smoothstep blending (the formula above) was selected over a 0.75 blend.
This is a small edge refinement, not another change to grain, light, contours,
spacing, optical dilation, or font assets. It adds no setting, cache or dependency.
The calculation is part of generating the existing cached raster, not an extra
screen filter or a repeated pass on cache hits.

RU pause/save and EN pause were visually checked, as were native 1600x1200 output
and a diagnostic sheet of thin Latin/Cyrillic strokes and punctuation. The EN save
capture is still not an acceptance result. A compiled before/after probe compared
both ACS cell sizes at 1x/1.25x/1.5x/2x/3x: shade indices and shadow pixels were
identical, as were nonzero and half-coverage masks. Thus the checked stroke cores,
holes and gaps were not erased/filled by the new curve. At native cell heights
of at least 64, the complete rasters were identical. The large RU save heading
also remained pixel-identical in actual game captures.

33/33 release and 33/33 ASan+UBSan tests passed again. Updated regressions require
small-size edge contrast while preserving fine strokes and solid cores; large
native glyphs retain ordinary antialiasing. Existing cache/resize, fading and
BMO redraw tests remain in place. This is software-renderer validation, not
GPU/physical-input acceptance. Artifacts and the before/after probe are outside
the repository at:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-acs-sharpness-hwSchpyb`.

### Shadow/contrast refinement and comparison reference

The user requested slightly more shadow and contrast after accepting the edge
improvement. The ambient/diffuse tone terms changed from 6/25 to 4/27, retaining
the maximum highlight level. Shadow coverage is `min(255, round(alpha * 1.15))`;
its offset, palette tone and nonzero support are unchanged. The height field,
noise, contours, spacing and sharpened front alpha are not altered.

**The primary visual reference is the original bitmap renderer, not the previous
TTF iteration.** This is the user's explicit correction. The assistant inspected
original/current pause, save-heading and empty-slot crops at their native 800x600
size and with nearest-neighbour detail enlargement, plus current native 1600x1200
output. Original strokes remain more angular and irregular, especially lowercase
round forms and the green action labels. Current TTF strokes have more regular
contours and more even illumination; this is not claimed as pixel-identical.
A stronger uniform black outline would not reproduce those original contours.

33/33 release and 33/33 ASan+UBSan tests passed again. A compiled comparison with
the prior TTF is used only as a change-isolation regression: both cell sizes at
five scales retained identical front alpha and shadow support, non-decreasing
shadow coverage, unchanged shadow colour and unchanged peak face colour. The
face ramp changes only by zero or minus one palette entry for the tested ACS
palette shift. No new rendering mechanism, setting, cache or dependency was added.
Artifacts: `/home/stalkerg/projects/vangers-font-baseline/2026-11-03-acs-contrast-5QfZwxfb`.

### First universal-material matrix — 2026-11-04 (visually rejected)

The user rejected manual Latin/Cyrillic contour tuning as the next direction and
authorized a comparative test of a language-independent material. Original
bitmap captures remain the visual reference, not the previous TTF iteration.
Font assets, optical dilation, noise, front coverage and layout were frozen.

Eighteen combinations were tested: fixed / 0.35 / 0.7 adaptive bevel; offset mask /
short (0.6) / longer (0.9) height projection; original / reversed horizontal light.
The initially selected combination was **0.35 adaptation, 0.6 projection, original light**
(experiment 4). It keeps a restrained edge on thin strokes; stronger alternatives
look harsher, while reversed lighting is less consistent with the reference.
The user rejected this visual conclusion on the pause heading's `З`: it remained
too shallow. The matrix held base depth fixed and favoured restraint, missing the
requested stronger relief. Passing tests did not establish visual acceptance.
The following results describe that historical iteration, not the current material.

`text/glyph_surface.h` implements only this fixed material with alpha-mask input.
No experimental selectors, glyph-specific rules, legacy letter maps, font edits,
new settings or dependencies enter the game. Existing iscreen material is unchanged.

Verification:

- 34/34 release and 34/34 Debug ASan+UBSan CTest pass. New material tests cover
  empty/solid/thin masks, gap isolation, immutable input, deterministic bounded
  heights/shadows, shadow direction and dependence on actual height. The previous
  offset-shadow assertion was replaced by visible contact-shadow coverage, not
  removed. Existing palette, cursor, cache, resize and 128-cycle BMO tests pass.
- The full matrix passed 540 run/size/palette cases (both ACS cells, five scales,
  three palette bases); front alpha and metrics stayed invariant, cached/resize
  output stayed deterministic. Final optimized helpers match the selected
  prototype exactly over 30 cases / 3,300,300 pixels.
- Real Noto Sans CJK Thin/Regular and Noto Serif CJK glyphs were tested at native
  sizes 16/31/39/62, including `AéßЖь日語龍鬱国漢あ가`. Each glyph's presence was
  checked, not replaced by `?`. These are material-only tests on raw SDL_ttf masks,
  not in-game CJK layout or newly bundled language coverage. The final 12 sheets
  match the selected prototype and pass ASan+UBSan with leak checking.
- Fresh actual RU pause/save captures at 800x600 and 1600x1200, and EN pause at
  800x600, were inspected against the original. No EN-save, GPU, physical-input
  or IME acceptance is claimed. The remaining shape differences are explicit.

CPU cost on the local Ryzen AI 9 HX 370: eight separate eight-letter runs; fresh
run rasters followed by 200 cached frames; warm-up and nine interleaved trials.
Times below are medians in milliseconds; font creation/layout are outside the timer.

| Native output | Published cf9f63f first / cached | Selected first / cached |
| --- | ---: | ---: |
| 800x600 | 1.962 / 0.166 | 3.711 / 0.143 |
| 1600x1200 | 6.897 / 0.696 | 13.136 / 0.659 |

The first raster costs about 1.9x more; the selected 2x first-render samples ranged
12.66–19.36 ms. Cached cost is similar and no new persistent cache was added.
Rebuilt runs and size changes still pay the higher cost. These are CPU-layer
microbenchmarks, not FPS or a guarantee for slower machines. Exact early exits
avoid unnecessary shadow alpha reads after zero/full occlusion without changing
the selected pixels. Synthetic materials and real game captures are kept separate.

Sources, all variants, captures, measurements and reports are outside the repository:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-03-acs-universal-7lCdRMH5`.

### Stronger relief after the pause `З` correction — 2026-11-04 (preceding candidate)

The user authorized another implementation pass, checking the pause heading's
`З` and the earlier examples. The original has a coherent bright face/edge and
dark side; the rejected material looked soft and mottled. More grain is not a
substitute for depth. This follow-up changes the material, not the font outlines
or the reference criterion. The user subsequently confirmed an improvement, but
not sufficient relief, and requested the five further cycles documented below.

Three diagnostic rounds explored 12 depth/profile/light combinations, 12
coverage/face-light combinations and four body/grain/projection combinations.
Simply increasing the old normal-light depth made mottling worse. Real-game
captures rejected that approach before the raised-face/body material was refined.
That preceding candidate used AA-aware height, a linear bevel, depth 4,
height/light blend 0.4/0.6 and a
visible projected body. Absolute grain amplitude, optical dilation and front
coverage remain unchanged. The eight-ray local-radius search of the rejected
adaptive version is removed; no experimental switches enter production.

Actual RU pause/save at 800x600 and 1600x1200, EN pause at 800x600, and original
bitmap comparison crops were inspected. Checks include `З`, the complete pause
panel, green action labels, `Сохранить игру`, `Свободно` and thin Latin/Cyrillic
strokes/punctuation. The bright face and dark border are more coherent, including
on `З`, without per-glyph adjustments. Original FNC letters remain more angular
and their pixel-level facets differ: this is not an exact visual reproduction.
Synthetic material sheets are not presented as game captures.

Final-candidate verification:

- 34/34 RelWithDebInfo and 34/34 Debug ASan+UBSan CTest pass. Material tests cover
  faint alpha below 128, bounded/deterministic heights, gap isolation, immutable
  masks, height-dependent face/body, projection direction and padding. The TTF
  integration test requires a body visibly outside the face, not merely inside
  its antialiasing. Existing cursor, palette/fade, cache/resize and BMO tests pass.
- 60 run/scale/palette cases check 8,143,866 pixels and deterministic cache/resize
  output: two families, both ACS cells, five scales and three palette bases.
  The 24 aligned 1x/2x face masks are byte-identical to the rejected iteration;
  30 non-material rasters are also unchanged. This does not assert byte-identical
  face masks at fractional scales after the extra padding/rounding.
- Real Noto Sans CJK Thin/Regular and Noto Serif CJK glyphs at 16/31/39/62 pixels
  pass ASan+UBSan/leak checks and produce byte-identical release/debug sheets.
  Glyph presence is asserted for `AéßЖь日語龍鬱国漢あ가`; this checks the material
  on real outlines, not in-game CJK shaping, layout, fallback or bundled coverage.
- EN save, GPU rendering, physical input and IME remain outside this acceptance.
  The main-menu transition capture is not a settled-menu visual acceptance result.

Fresh paired CPU measurements use the same Ryzen AI 9 HX 370, eight eight-letter
runs, 200 cached frames and nine interleaved trials. Font creation/layout are
outside the timer; values below are medians in milliseconds.

| Native output | Published cf9f63f first / cached | Current candidate first / cached |
| --- | ---: | ---: |
| 800x600 | 1.927 / 0.163 | 3.083 / 0.159 |
| 1600x1200 | 6.785 / 0.667 | 11.186 / 0.679 |

First rasterization is about 1.6x the published version; cached cost is similar.
The candidate's 2x first-render samples span 11.125–11.358 ms. Rebuilt runs and
resizes still pay that cost. These are CPU-layer measurements, not gameplay FPS
or a cross-platform guarantee. No new persistent cache or dependency is added.

Sources, rejected variants, final captures, probes and measurement logs:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-04-acs-depth-Tj9XyTWF`.
The selected production source is captured in `final-source`; `capture-final2`
and `final2-matrix` are the current game/material results, not `final1`.


### Five further cycles on `А` and `З` — 2026-11-04 (preceding candidate)

The user confirmed that the preceding result was better, but not sufficient,
and explicitly requested five more improvement cycles with additional focus on
`А`. Original bitmap captures remained the primary reference. Both occurrences
of `А` in `ПАУЗА`, `З`, the whole pause panel, green action labels,
`Сохранить игру` and `Свободно` were checked, rather than optimizing one crop alone.

| Cycle | Experiment and visual finding | Carried forward |
| --- | --- | --- |
| 1 | Six lighting variants. Reversed light and simply darker light did not reproduce the reference; a lateral direction separated horizontal and diagonal strokes but needed a highlight. | Light `(-1,-0.1,0.45)`. |
| 2 | Three antialiased-distance/bevel variants. Fractional seeds propagated into opaque neighbours give a more consistent bevel than correcting only each boundary pixel. | Shared optional fractional seeding; no copied distance-transform loops. |
| 3 | Three narrow-highlight strengths. A fixed half-vector highlight restores a bright facet without raising the whole face equally. | Highlight amplitude 12, exponent 16, lower diffuse contribution. |
| 4 | Four edge/grain variants. A third S-curve over-sharpens; less height noise keeps the facet coherent. | Two curves, absolute grain 0.4. Final cross-font validation limits the second curve near faint strokes. |
| 5 | Six bevel/body/face balances. Reweighting the whole face flattened contrast. The original-reference check also caught an over-dark lower arc of `З`; two extra ambient levels restore it without changing the dark body. | Bevel 1, body `(2.25*z,-0.75*z)`, padding 4, ambient 6. |

Twenty-two parameter candidates were explored in these five cycles. Each cycle's
selected candidate was built and captured in the real game, with full pause/save
comparison regions, not merely synthetic letters. The final production-only
implementation removes all experimental switches, redundant transforms and general
power functions. It was built and captured again after the thin-stroke guard.
A final original-reference pass restored two ambient levels: the first fifth-cycle
balance made the lower `З` arc too dark. This refinement stays within cycle five;
the narrow highlight and dark-body tone remain unchanged.
Font assets, optical dilation, advances, fixed pitch and iscreen material constants
are unchanged. This is still a candidate for user review, not final visual approval.

The assistant inspected the final `А` crossbar/counter/diagonals, `З` arcs, complete
RU pause/save at 800x600 and 1600x1200, EN pause at 800x600, thin strokes and all
three CJK specimen families. There is clearer facet separation and less mottling;
the original remains more angular and has different highlight widths. No exact
visual match or newly integrated CJK coverage/shaping is claimed.

Verification of the final production code:

- 34/34 RelWithDebInfo and 34/34 Debug ASan+UBSan CTest pass. Added regressions
  cover propagated fractional seeds, the unchanged default threshold, directional
  facet contrast, monotonic coverage, preserved counters/cores and the faint-stroke
  guard. Existing palette, cursor, cache/resize and BMO cleanup tests pass.
- 60 run/scale/palette cases check 8,553,384 pixels and deterministic cache/resize.
  The 24 aligned 1x/2x masks retain identical nonzero and half-coverage supports;
  117,897 alpha samples intentionally change. All 30 non-material rasters remain
  byte-identical. No fractional-scale alpha-byte parity is claimed.
- 1,152 synthetic masks produce byte-identical default distance fields before
  and after the optional seed addition, protecting the shared iscreen path.
- 30 final production probe sheets, including 12 real Noto Sans CJK Thin/Regular
  and Noto Serif CJK sheets at 16/31/39/62 pixels, pass ASan+UBSan/leak checks and
  are byte-identical to release output. All test glyphs are present, not `?`.
- EN save, physical keyboard, IME and GPU acceptance remain outside these tests.
  An initial `capture-final-scales` run used an observer without resize support
  and is excluded from high-resolution acceptance. The corrected run is
  `capture-balanced-2x`; both 2x capture dimensions are checked as 1600x1200.

Fresh paired CPU measurements use eight eight-letter runs, 200 cached frames,
warm-up and nine interleaved trials on the Ryzen AI 9 HX 370. Creation/layout are
outside the timer; values are median milliseconds, not gameplay FPS.

| Native output | Published cf9f63f first / cached | Before five cycles first / cached | Final candidate first / cached |
| --- | ---: | ---: | ---: |
| 800x600 | 1.948 / 0.163 | 3.054 / 0.159 | 3.447 / 0.163 |
| 1600x1200 | 6.801 / 0.662 | 11.164 / 0.678 | 12.667 / 0.693 |

First-raster cost is about 13–14% above the preceding candidate; cached cost remains
similar. The final 2x first-render range was 12.620–13.919 ms. Rebuilt runs and
resizes still pay that cost; no new cache, dependency or render subsystem is added.

Sources, per-cycle captures, all alternatives, probes, exact measurement logs
and the final comparison are retained outside the repository:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-04-acs-five-EevzsKYQ`.
`before-source` preserves the prior local version; `final-source` and
`final-manifest.json` identify the new local result. That five-cycle task did not commit or push; the following ten-cycle task
supersedes its material and conditionally authorizes publication.


### Ten further cycles — 2026-11-04 (visually rejected)

The user requested ten more cycles and authorized a normal commit/push only if
visually better. This replaces the five-cycle candidate above, not the font assets.
The assistant initially judged it an improvement, but the user explicitly
rejected that assessment: the preceding version was better. Brightening flat
faces did not solve the main problem, insufficient shadow. The following are
historical measurements, not visual acceptance of the ten-cycle candidate. Original bitmap is the primary
reference; the preceding local TTF is a secondary before/after comparison.

| Cycle | Tested direction | Decision |
| --- | --- | --- |
| 1 | Broad face instead of a narrow specular glint | Remove specular lighting; reject an overly uniform bright cap. |
| 2 | Shallower depth and three bevel profiles | Keep depth 4; initially narrow the bevel to 0.75. |
| 3 | Light-facing slope versus cancellation at a thin ridge | Keep the one-sided horizontal slope. |
| 4 | Vertical slope and top-light balance | Retain central vertical differences; curved strokes stay better lit. |
| 5 | Absolute grain amplitudes 0.2, 0.3 and 0.6 | Keep 0.3; 0.6 fragments the face, 0.2 loses too much texture. |
| 6 | Body widths 1.75/2.75 and a lower projection | A wider side separates the face from the background; tune down in cycle 10. |
| 7 | Reduced extra edge contrast | Reject: it restores softness. Keep the full local-core-guarded curve. |
| 8 | Bevel 0.6/0.9 and a nonlinear profile | Keep a linear profile, compare both native sizes and CJK specimens. |
| 9 | Face/directional-light balance | Raise the broad face instead of adding more edge brightness. |
| 10 | Final face/bevel/body balance | Face 17, diffuse 12, ambient 4, bevel 0.85, body width 2.5. |

Each cycle has a real-game pause/save capture. Both `А` occurrences and `З` were
inspected enlarged, together with the whole pause panel, green action labels,
save heading and `Свободно`. Only the selected fixed algorithm enters production:
no tuning switches, general power functions, per-glyph rules or copied bitmap
height profiles. Optical dilation, front coverage, advances and font files stay
unchanged from the five-cycle candidate. The ordinary iscreen material is unchanged.

Final validation:

- 34/34 release and 34/34 Debug ASan+UBSan CTest pass. A new synthetic-ridge
  assertion guards against cancellation of its opposing slopes. Existing
  palette/fade, cursor, cache/resize and repeated BMO redraw tests pass.
- 60 run/scale/palette cases check 8,553,384 pixels. The 24 aligned 1x/2x front
  masks are byte-identical to the preceding local candidate, as are all 30
  non-material rasters. Cache/resize is deterministic.
- 1,152 synthetic masks retain byte-identical default distance fields, protecting
  iscreen from the optional fractional edge seeds.
- All 30 production probe sheets match the selected prototype exactly and also
  match ASan+UBSan/leak-checked output. These include 12 real CJK specimens
  (three families, four sizes); glyph presence is asserted. This is material-only
  validation, not integrated CJK layout, shaping, fallback or bundled coverage.
- Fresh RU pause/save captures at 800x600 and 1600x1200, plus EN pause at 800x600,
  were inspected. Both 2x captures are genuinely 1600x1200. The initial EN/2x
  harness invocation had incorrect environment-variable names and was stopped;
  only the corrected run is used. EN save, GPU, physical-input and IME acceptance
  remain outside this validation. Animation timing can change pause backgrounds;
  full frames are not claimed pixel-identical.

Local CPU measurements: Ryzen AI 9 HX 370, eight separate eight-letter runs,
200 cached frames, warm-up and nine interleaved trials. Font creation/layout are
outside the timer. Median milliseconds:

| Native output | Published cf9f63f first / cached | Before ten cycles first / cached | Current first / cached |
| --- | ---: | ---: | ---: |
| 800x600 | 1.962 / 0.162 | 3.445 / 0.163 | 3.436 / 0.164 |
| 1600x1200 | 6.798 / 0.669 | 12.668 / 0.689 | 12.546 / 0.699 |

Cost is essentially unchanged from the preceding local material, but first
rasterization is still about 1.8x the published shallow material. The current
2x first-render range was 12.492–17.899 ms; resizing/rebuilding runs pays that
cost again. Cached rendering remains similar. These are CPU-layer timings, not
FPS or a slower-machine guarantee; no new persistent cache was introduced.

Sources, all ten captures, rejected alternatives, probes, measurements and the
comparison report are retained outside the repository:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-04-acs-ten-GJfhUbIU`.
`cycles.tsv` records each decision, `before-source` preserves the starting local
version, and `final-source`/`final-manifest.json` identify the published candidate.


### Strong shadow after the brightness correction — 2026-11-04 (superseded)

The user explicitly preferred the preceding five-cycle lighting over commit
1b12128 and requested a **serious increase in shadow**, not brighter flat faces.
That correction supersedes the positive assessment of the ten-cycle result.
This local follow-up restores the preferred height/grain/lighting and keeps
front coverage and outlines unchanged. Three real-game shadow lengths were
compared: `(3.5,-1.5)`, `(5,-2)` and `(6.5,-2.5)`; the last is selected for review.
Relative to the preferred `(2.25,-0.75)`, it projects about 2.9x farther left and
3.3x farther down. The original bitmap remains the primary visual reference.

The algorithm remains a single height-driven extrusion with no alphabet rules,
legacy profiles, contour changes, global black outline or new settings. Sixteen
height steps keep the longer body continuous. Nine logical pixels of padding
retain its full support; advances, hit boxes and centering are unchanged.
Ray-support rejection reduces work outside the glyph. The dark palette tone and
opacity are unchanged: the effect is from a larger visible side, not a brighter
face or another screen-wide filter.

Verification:

- 34/34 release and 34/34 Debug ASan+UBSan CTest pass. The rejected one-sided
  ridge-lighting assertion is removed with that lighting model. Directional
  face-lighting and coverage tests remain. A new extent/continuity regression
  requires >90% body coverage across four logical pixels outside a raised block;
  an expanded bounds scan checks the larger padding at all tested scales.
- 60 run/scale/palette cases check 10,588,068 pixels, deterministic cache/resize
  and palette limits. Across 24 aligned 1x/2x cases, **both front shades and alpha
  are byte-identical to the preferred five-cycle material**. All 30 non-material
  rasters also remain identical. Fractional-scale byte identity is not asserted
  because different ink padding changes output-size rounding.
- On the diagnostic `ПАУЗА` at 1x, alpha-weighted visible body area increases
  from 428.817 to 1154.72 pixels (2.69x). This measures border coverage outside
  the face, not a brightness score or a claim for every string/font.
- The optimized ray bound matches the unbounded sixteen-step reference at
  1,114,992 samples, including anisotropic scales from 0.5 to 3.
- All 30 production diagnostic sheets match leak-checked ASan+UBSan output.
  CJK specimens now reserve the full enlarged shadow bounds instead of the
  former fixed four-pixel diagnostic margin. Three real CJK families at four
  sizes check the material, not integrated CJK coverage, layout or shaping.
- Actual RU pause/save at 800x600 and 1600x1200 and EN pause at 800x600 were
  inspected, including both `А`, `З`, green labels, heading and `Свободно`.
  EN-save, GPU, physical-input and IME acceptance are not claimed.

The same local eight-run/eight-letter, 200-cached-frame CPU benchmark uses nine
interleaved trials; creation/layout are outside the timer. Median milliseconds:

| Native output | Preferred before first / cached | Strong shadow first / cached |
| --- | ---: | ---: |
| 800x600 | 3.460 / 0.162 | 4.129 / 0.201 |
| 1600x1200 | 12.639 / 0.690 | 15.639 / 0.848 |

The larger raster and denser height sampling cost about 19–24% more on first
render; cached cost rises by about 0.039/0.158 ms. The 2x first-render range is
15.422–21.205 ms. This is not FPS or a cross-platform guarantee. There is still
only one retained raster per run, replaced on resize; no unbounded cache.

Sources, all three alternatives, game captures, parity and cost measurements:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-04-acs-shadow-Dwslvsh4`.
That task left a local candidate without commit/push. The user then rejected
the excessive green-letter shadow and clarified that relief must come primarily
from shading on the letters themselves; the next section supersedes it.


### Short outer shadow, stronger shading on the letter — 2026-11-04 (accepted material)

The user clarified that the strong external shadow was excessive, especially on
GREEN labels. The important relief is **shade on the letter itself**, not the
long silhouette behind it. The external projection is now exactly half in both
axes: `(6.5,-2.5)` -> `(3.25,-1.25)`. Padding drops from 9 to 5; the sixteen
height samples, dark tone and opacity remain. This does not halve the visible
shadow area exactly, because it overlaps the glyph differently.

The preferred lighting remains the base. A small fixed term subtracts
`12.96*clamp(flat_incidence-N.L,0,1)` from the face tone. Flat or better-lit
surfaces keep their former brightness; less-lit slopes darken inside the glyph.
This is directional material shading from the existing height normals, not a
physical cast self-shadow, per-letter mask, blanket dimming or black outline.
It adds no font/palette-specific branch, setting, dependency or cache.

Four candidates were built and captured: three strengths applied only when
`N.L<0`, then the selected flat-relative shading. The former affected too narrow
an edge; the selected term separates a wider dark facet without raising the
bright side. The assistant inspected original/previous/current А and З, green
labels, complete pause, save heading and Свободно, including actual RU800/1600
and ENpause800. The user subsequently accepted this visual compromise and authorized commit,
push, full PR review and merge if the review/checks are clean.

Validation:

- 34/34 release and 34/34 ASan+UBSan CTest pass. Added flat-face brightness and
  away-slope darkness checks. The contact-side test now requires a continuous
  short side and rejects the old four-pixel extension, matching the user's
  explicit shortening request. Bounds, cursor, fade/cache and BMO tests remain.
- 60 scale/family/palette cases check 8,951,760 pixels and deterministic resize.
  Front shades can intentionally reach the lowest nonzero palette step, as
  allowed by the existing tone clamp; no alpha threshold or colour rule changed.
- In 24 aligned 1x/2x cases, front alpha is identical to both the preceding long
  shadow and a short-shadow/no-added-shading control. Of 435,927 ink samples,
  226,140 darken and NONE brighten against the control. Flat-face reference
  tones stay 19/23. All 30 ordinary non-material rasters are byte-identical.
- All 30 production probe sheets match ASan+UBSan/leak-checked output, including
  12 real CJK specimens. The ray bound still matches the unbounded reference
  over 1,114,992 samples. These are material tests, not integrated CJK shaping,
  fallback or new bundled glyph coverage. EN save/GPU/physical input/IME are not
  acceptance claims.

Local CPU benchmark, same eight eight-letter runs, 200 cached frames, nine
interleaved trials; font creation/layout outside the timer. Median ms:

| Native output | Previous long shadow first / cached | Current first / cached |
| --- | ---: | ---: |
| 800x600 | 4.133 / 0.204 | 3.551 / 0.175 |
| 1600x1200 | 15.575 / 0.851 | 13.089 / 0.727 |

The shorter shadow/padding offsets the extra shading arithmetic. These are CPU
layer measurements, not gameplay FPS. Current 2x first-render range is
12.988–19.083 ms; rebuilding/resizing still pays first-render cost.

All snapshots, four captures, final RU/EN captures, probes and comparison:
`/home/stalkerg/projects/vangers-font-baseline/2026-11-04-acs-facets-DQvzQkRR`.
The user accepted this compromise for publication; it does not claim an exact
recreation of the original raster contours.

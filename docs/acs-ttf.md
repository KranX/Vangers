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
  reproduces a small bevel, offset dark edge and grain procedurally from the
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
- The shared two-pass chamfer transform measures distance inside the alpha mask.
  In logical pixels, `d = max(0, (distance - 0.5) / min(scale_x, scale_y))`.
  A shallow convex profile is `sin(min(1, d / 1.2) * pi/2)`.
- Height, not final brightness, is multiplied by
  `1 + 0.32 * smooth_grain(x / scale_x, y / scale_y)`. The deterministic noise
  grid is smoothly interpolated between logical pixels at enlarged/fractional
  resolutions, avoiding nearest-neighbour noise blocks.
- Central height differences at +/-0.5 logical pixels, multiplied by 2, give
  the surface slopes. The normalized light direction is `(-0.6, -0.4, 0.9)`.
  Tone is `clamp(round(4 + 27 * max(0, normal_dot_light)), 2, 31)` before the
  existing palette shift. A dark copy of the mask at (-1, +0.5) logical pixels,
  tone 2, is composed behind the face. Its coverage is boosted by 15% (capped
  at full opacity), keeping the same support and offset.
- For small native glyphs, only the **front coverage** gets a mild S-curve:
  `a' = a + strength * a * (1-a) * (2*a-1)`, where `a = alpha/255` and
  `strength = clamp((64-native_cell_height)/24, 0, 1)`. It is fully applied up to
  40 native pixels and fades out at 64. Fully transparent/opaque samples stay
  unchanged; partial samples stay in 1..254. The curve is monotonic and preserves
  the half-coverage boundary, without a binary threshold, halo or alphabet rules.
  Height generation, shade levels and the offset shadow use the original mask,
  **before** this coverage adjustment. Large text and other TTF roles are unaffected.
- Two pixels of layout padding retain the shadow without changing advances or
  hit boxes. Existing per-field image replacement removes it on redraw without
  clipping overhanging letters or neighbouring fields.

The small noise and distance helpers are shared with iscreen in `text/grain.h`
and `text/inside_distance.h`; its material constants and arithmetic are unchanged.
A compiled comparison against the published iscreen implementation produced
byte-identical height maps for 48 synthetic masks/size/material combinations,
including solid, empty, edge-touching and perforated masks. No original letter
pixels/heights are sampled for ACS material: a newly supplied Unicode outline
gets the same treatment. This does not provide missing glyphs, shaping or translations.

### Iterations and verification

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

# Native-resolution actint TTF text

This stage builds on actint's UTF-8 storage. It replaces the **actint output
paths**, not just one screen: HUD/info panels, inventory/menu labels and counts,
shop descriptions/prices, escave dialogue/topics, world-map letters, mouse
prompts, animated messages, racing labels and full-screen text pages.

## Enable / disable

In the game's `settings.toml`, set the existing `[video]` table's field:

```toml
[video]
ttf_text = true
```

Restart the game. The default is **false**, retaining the original bitmap
renderer. This setting does not convert resources, saves, or network data.

Builds require **SDL3_ttf 3.2+** in addition to SDL3/SDL3_net. The Linux, macOS,
MSYS2 and vcpkg build configurations include the dependency. `INSTALL` describes
the platform prerequisites. The TTF is embedded at build time: users do not need
to copy a new font into an existing private game-data installation.

## Font and text contract

Only `data/resource/actint/fonts/VangersText-Regular.ttf` is imported from the
approved v0.10 font experiment. That directory records its source/provenance.
The Menu and Display faces are for a subsequent iscreen stage, not this backend.

- Canonical UTF-8 reaches SDL3_ttf without a CP866 encode/decode round-trip.
- The current face covers ASCII Latin, Russian including Ёё, digits,
  punctuation, NBSP and the game currency sign (`$`, also U+E000).
- Missing characters use `?` for presentation only; stored text is not changed.
  This is **not** complete German coverage, a new translation, shaping or IME.
- Layout and drawing share font-unit advances. The face's included one-pixel
  tracking is removed before applying each caller's spacing. Historical cell
  sizes, line spacing and fixed per-letter vertical placement remain explicit.
- FreeType receives both native-output scales through SDL3_ttf, including narrow
  and wide legacy cell variants. Window resizing does not change wrapping or
  hit boxes. Neither axis of a rendered letter is stretched from an 800×600 image.
- Drawing at 18 pixels per em or below uses light vertical hinting for small-text
  clarity. Larger output and all layout metrics remain unhinted. This does not
  change the font asset, advances, wrapping, or the legacy bitmap control icons.
- Standard-font borders are an output effect, not baked into the outlines.
  Packed numeric/price colours and the currency sign remain supported.
  Range fonts select the existing palette bank/intensity; antialiasing coverage
  is separate from palette colour, so palette fades still work.
- Original control/icon cells remain bitmap images with their old bearings.
  They are not interpreted as alphabetic Unicode glyphs.

## Why companion text layers

Actint does not draw only to the screen. It paints temporary indexed buffers,
retained panel bitmaps and the iscreen colour map; those are then copied,
scrolled, clipped and overlaid by inventory art, gates, cursor and prompts.
Drawing one late SDL text overlay would lose that ordering. Drawing TTF into
those low-resolution indexed buffers would lose native-resolution contours.

`xgraph/text_layer` therefore retains a small companion layer only for registered
buffers. A draw owns its text/image; buffer copies own visibility snapshots.
Bitmap writes invalidate the affected logical cells; transparent writes leave
unwritten cells intact. Copies and terrain-height masks carry the text along.
These logical masks determine **occlusion**, not the contours of the glyphs.

At presentation the two XGR layers are composited at the actual SDL output size,
using the current palette and existing scene aspect/letterbox rectangle. The
cursor/prompt background is restored after presentation, including its text
snapshot. Resizing and buffer destruction discard stale companion state.
This follows the existing rendering path rather than adding another UI tree.

Hooks in iscreen/ACS only transport or invalidate actint text underneath their
existing drawing. They do not reinterpret those subsystems' strings. Their own
fonts, main menu/history, ACS pause/save UI, chat and service screens remain
bitmap/legacy. This includes static iscreen object tooltips (for example the
shop-category hint); actint-owned HUD/item prompts select native callbacks. Original `aPutStr`, `aOutStr`, `aOutText32`, etc. keep CP866
contracts; only actint's `*Utf8` calls and its mouse prompts select this backend.

## Validation

Run the normal out-of-tree CMake build and CTest suite. `actint_ttf` checks:

- embedded/source font metrics and complete pixel equality;
- real RU/EN strings, Ёё, spacing, CR/LF, missing glyphs and invalid UTF-8;
- native detail at 2× resolution, fractional scales and narrow/wide cells;
- palette changes, premultiplied alpha, borders and numeric/currency highlighting;
- original control-icon bearings/pixels and safe missing-icon fallback;
- clipping, top-layer/terrain masks, partial transparent erasure, self-copies,
  ownership after source release, resize and buffer deregistration.

`actint_mouse_prompt` additionally checks owned canonical UTF-8 and per-prompt
callbacks without changing the shared legacy XGR callbacks. The settings test
checks bitmap-by-default and TTF-setting persistence. The existing source/binary
script and runtime regressions remain in the suite.

The `actint_menu_{bitmap,ttf}` and `actint_counter_{bitmap,ttf}` integration
tests use real production widgets and generated BML fixtures (no private assets).
They check question-menu centering, selection changes, scrolling and clearing,
plus range/standard counter origins at 1×/2× output. Both TTF cases failed on
the original PR revision and pass after the compositor/bearing fixes. The same
tests also exercise menu and bitmap-font destruction under ASan.

The 3 November follow-up also checks the composed question against independently
rendered children: a short topic must not erase the end of its prefix (the `ь`
in `А что иль кто есть`). Only the menu set clears that shared canvas; a child's
allocation may overlap its neighbour because it accommodates longer topics.
The glyph test checks complete soft-sign coverage, light hinting at 1×, unhinted
outlines at 2×, and a return to 1× without changing layout metrics.

Local Linux validation on 1 November 2026 used SDL3 3.4.18 / SDL3_ttf 3.2.2:

- Full binary-script build, including the standalone iscreen compiler; **15/15
  CTests passed** initially; **19/19 passed after the self-review fixes**. The
  source-script (`BINARY_SCRIPT=OFF`) executables also build.
- **19/19 CTests passed with ASan + UBSan**, including leak detection, after
  fixing the old menu base-pointer deletion and bitmap-font array deletion
  exposed by the new integration fixtures.
- Rechecked populated RU/EN escave question selectors in isolated game runs at
  800×600 and 1600×1200 after advancing the opening speech and clicking the
  question button. The text reaches the final screen in both languages.
- Isolated RU/EN game-data copies and SDL dummy/software captures exercised
  dialogue, shop descriptions/prices, hover prompts, world/inventory overlays
  and the existing pause/save/load UI. The embedded font was also used from a
  data directory with no TTF file. Original game data and saves were not edited.
  The inventory covering part of the ACS pause panel reproduces on the same
  route in the pre-TTF integration binary; this PR does not change that old UI issue.
- Bitmap mode exactly matched the pre-TTF binary for the initial EN menu,
  history, escave and first dialogue-skip captures (full 800×600 pixel buffers).
- Window-output captures at 800×600, 1200×900 and 1600×1200, plus a 1600×900
  letterboxed window, checked native contours and alignment against scene art.
- The strict whole-game sanitizer run encounters an existing unaligned scalar
  read in `aciCHTree::load` (`chtree.cpp`, unchanged here) while loading assets.
  A recovery-mode run reached the RU escave/resize captures without an ASan
  memory-access report, but logged other legacy UBSan findings and had runtime
  leak-at-exit checking disabled. Passing isolated tests is not a claim that the entire legacy game is UBSan-clean.

These are software-renderer/synthetic-input checks, not physical-input,
GPU/HiDPI, multiplayer, every late-game screen, or full gameplay acceptance.
Automated screenshot names are not proof that a purchase/save command succeeded;
manual acceptance remains important. Platform CI and a real-window playthrough
are additional checks, not implied by the local results.

# European and Japanese font coverage

Stage 5, from integration `feature/text-rendering` (`337dfbc`). This is renderer
support, **not a Japanese translation of the game**. Existing locale catalogs,
SCB commands, quest IDs, geometry, gameplay and save format are unchanged.
Enable `video.ttf_text = true`; the diagnostic bitmap renderer has no new alphabet.

## Fonts and fallback

All three Vangers faces gain 183 European characters, including German umlauts,
eszett, French accents/ligatures, Polish letters, Czech/Slovak/Hungarian accents,
and Romanian comma-below forms. Their old cmap entries, contours and advances
are preserved. The reproducible construction tools and provenance are documented
in [fonts/README.md](../data/resource/actint/fonts/README.md).

Each role uses the same chain: its Vangers face, then embedded **Noto Sans JP
Medium**, then the existing presentation-only `?`. Selection is by glyph
coverage, never by locale ID. A lazy, shared fallback face survives with its
retained runs; it is not reopened per glyph/frame. Its metrics face is separate
from its resized drawing face. Missing scalars are warned once at rendering;
the original UTF-8 text is never replaced in storage.

Fallback outlines use the **same** iscreen height-map, grain, lighting, shadow
and click-blur pipeline, and the same ACS palette material. No old glyph-height
profiles, Japanese letter textures, special lighting or language branches were
added. Proportional roles use square fallback ems (84% of the line cell), with
normal advances from that font. Fixed-pitch ACS retains its exact original cell
pitch; fallback ink fits a square inside that cell, not overlapping the next one.
Medium weight was chosen after comparison with Regular in actual game frames;
it improves small kana/kanji without changing the material or RU/EN letters.

## Text boundaries

- Layout composes adjacent canonical pairs, notably `e + acute` and
  `kana + dakuten/handakuten`, in a temporary presentation view. Iscreen relief
  measurement and drawing use the same view. Stored/input text is not rewritten.
- This is **not full Unicode normalization**, grapheme editing, OpenType shaping
  or bidi. The existing scalar-count/backspace editing contract remains.
- Horizontal Japanese/CJK wrapping uses measured glyph widths. It keeps opening
  brackets off line ends and closing punctuation, small kana, iteration marks,
  prolonged sound marks and half-width dakuten off line starts. Combining marks
  are not detached. Folded Japanese paragraph lines do not gain ASCII spaces.
- There is no locale switch in the line breaker and no CJK change to the original
  Latin/Cyrillic wrapping path. No hanging punctuation outside the fixed box,
  scrolling, new pages, automatic shrinking or hidden tail. An impossibly narrow
  CJK box fails instead of overflowing. This is a horizontal UI kinsoku adapter,
  **not a complete implementation of UAX #14 / JIS X 4051**. Reference:
  [W3C Japanese layout requirements §3.1.7–3.1.8](https://www.w3.org/TR/jlreq/).

A future translation must still fit its existing panels. The bundled Noto face
covers common Japanese kanji, hiragana, katakana and punctuation, not every
Unicode ideograph/emoji. Vertical writing, ruby, variation-selector shaping,
Arabic/Hebrew, and physical IME/preedit acceptance are not claimed by this PR.
Committed SDL UTF-8 text input and Japanese title rendering are exercised.

## Check a translation before delivery

```sh
/path/to/build/src/vangers_font_check /path/to/data/locales ja
/path/to/build/src/vangers_font_check /path/to/data/locales ja --text /path/to/dialogue.txt
```

The tool checks the resolved catalog in all three role chains, optionally an
additional UTF-8 file (for dialogue/history/credits authoring). Missing glyphs
report locale, catalog key/file, role and Unicode scalar. Exit status 0 means
covered, 1 means missing glyphs, 2 means invalid input/package. Catalog fallback
diagnostics are distinct from glyph coverage: English fallback is not evidence
that a translation is complete. No encoding autodetection is added.

CTest discovers every committed `locales/*/manifest.toml` automatically. Separate
regressions exercise Japanese, canonical pairs, unsupported U+10FFFF, malformed
UTF-8, all three roles, ACS material/cache and procedural relief without legacy
heights. No actual `ja` translation/package is shipped as a placeholder.

## Visual QA and invariants

Use a fresh isolated copy of full game data; recompile the eight keyed SCB there.
Never overwrite original private data or saves. Synthetic QA captions/Diagen
phrases are **rendering samples, not proposed translations**. Actual game frames
(SDL dummy/software backend) cover menu, dialogue, shop and ACS save-title
editing. If GDB selects an existing
save screen, label that frame as a renderer preview rather than a physical
save-game/gameplay test. SDL event injection is not a physical IME test.

Compare original/new RU/EN at the same logical/window sizes. In this iteration:
all original glyph maps/advances/contours are unchanged; 72 face/material/pitch/
scale raster snapshots and all 184 stock description frames match the base.
Corpus audit found one intentionally restored character in shipped RU dialogue:
`№` in `Inventory.text` (previously missing in the v0.10 face, displayed as `?`).
Existing glyphs are unchanged; previously unsupported characters now use fallback.
No relief/material parameters or click-animation commands were changed.

Local verification: 69 tests passed three times excluding the unchanged
`settings` fixture failure (also reproduced with the base binary). ASan/UBSan
and leak checks passed 65 tests three times, excluding that fixture and four
pre-existing ACS invalid-vptr cases. Those ACS cases remain covered by the
normal suite; the new fallback/cache tests pass under sanitizers. This does not
substitute for native Windows/macOS or physical IME testing.

The fallback adds about 5.7 MB of embedded font data, no new runtime library.
Noto's OFL notice is embedded in its name metadata and copied beside the locale
catalogs by the existing delivery target. Unicode data terms accompany it there;
all existing Linux/Windows/macOS/Flatpak locale-copy paths carry these notices.

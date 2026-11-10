# Vangers fonts

`VangersText-Regular.ttf` is the approved experimental v0.10 outline font from
`experiment/text-rendering-lab`, commit `b9aabeeb4a590f725dcd3716d4e166653a167e78`,
`docs/text-rendering/font-lab/roles-v10/`. Its editable construction sources,
per-glyph provenance and visual comparisons remain on that branch.

Derived from the game's `cfont00.bml` (10×18), with contour detail from
`cfont03.bml`. The baseline is 15; the advance already includes one source pixel
of spacing. Uneven letter placement is fixed in the outlines, not random jitter.
Latin ASCII and Russian (including Ёё), digits, punctuation, NBSP, and the game
currency symbol (`$` / U+E000) are present. Other scripts require more glyphs.

This is derived game artwork: the source code's GPL does not grant a new licence
to the original glyph designs. Original artwork rights remain with their owners.

## Iscreen faces

The same approved v0.10 directory supplies two additional, base faces:

| Font | Original role | Cell | Baseline | Included tracking | Bytes |
| --- | --- | --- | --- | --- | --- |
| `VangersMenu-Regular.ttf` | `hfont01.fnh`, decorative main-menu capitals | 45×50 | 44 | 5 | 141364 |
| `VangersDisplay-Regular.ttf` | `hfont00.fnh`, settings/headings | 26×50 | 44 | 5 | 72128 |

Menu and Display are **not interchangeable**. Iscreen's smaller `hfont03/04`
roles use Display at their original cell sizes. Letter heights are generated
procedurally from TTF coverage; no old letter height profiles are used.
`hfont02` is an icon font and stays bitmap. Original `.fnh` resources remain
required for compatibility metrics, icons and bitmap mode. Generated text heights
use the existing terrain lighting, shadows, inversion and click blur; see
[the relief implementation](../../../../docs/iscreen-ttf.md).
Newly supplied characters (including Menu lowercase/digits/Ё) are not evidence
that the original bitmap contained those letters.

Original v0.10 input SHA-256:

- Menu: `45e44b66abd41d65bc4529c9361c08c3bbd55fa9784b487dd4373eff9f966b37`
- Display: `dd64ebdcb8108b58806f34a9ebeb27f083a633a7a31175ab7945b0c4e0227f49`

All three files are embedded in the executable. The hashes above identify the
original v0.10 inputs, before the extensions described below. Base construction
experiments remain on the original lab branch. New extension tools are maintained
here, as described below.


## European extension and Japanese fallback (stage 5)

The existing three faces now add 183 European characters each. Base letters use
our existing game outlines; accents, strokes and eszett are new font-space
geometry, and ligatures combine our own outlines. No Noto contours were merged
into these game-artwork faces. Old glyph outlines/cmap/advances remain unchanged.
The original artwork rights described above still apply.

`tools/fonts/extend.py SOURCE_DIRECTORY OUTPUT_DIRECTORY` reproduces them from
the three unmodified v0.10 fonts at integration `337dfbc`. Export those inputs to
a disk working directory, not over a user's data; run using Python and
`fonttools==4.66.1`. The tool does not run during normal C++ builds.

`NotoSansJP-Medium.ttf` is a separate fallback, not a fourth recreation of game
artwork. Source: [Google Fonts Noto Sans JP](https://github.com/google/fonts/tree/5bb28733cec313b63fbd415b574b5ae35c8aa8b8/ofl/notosansjp),
revision `5bb28733cec313b63fbd415b574b5ae35c8aa8b8`, upstream `NotoSansJP[wght].ttf`.
Its SHA-256 is `c2f3b4d463500a2ddcd3849cded1fceeb9fd6d1c32e6cbecd568453ba50fc68f`.
Freeze that input at weight500 with `tools/fonts/fallback.py INPUT OUTPUT OFL.txt`
using the same fontTools version. The script checks the input hash, updates the
static face names and embeds the full OFL notice in font name13. No network
access, OS-installed font or fontTools is needed to build/run the game.
Copyright 2014–2021 Adobe; SIL Open Font License1.1, reserved font name “Source”.
Full terms: `NotoSansJP-OFL.txt`. Noto is distributed under those terms separately
from the original game artwork. The build copies the notice with locale files
for all existing distribution paths.

`tools/fonts/unicode_tables.py` uses Python's Unicode16.0.0 database to regenerate
`src/text/unicode_tables.h`; format the result with repository clang-format22.1.5.
It supplies canonical adjacent-pair and combining-mark data, not a replacement
Unicode shaping library. Terms: `tools/fonts/UNICODE-LICENSE.txt`, also delivered
with the locale files. Detailed renderer contract/limits and coverage checker:
[font-language-coverage.md](../../../../docs/font-language-coverage.md).

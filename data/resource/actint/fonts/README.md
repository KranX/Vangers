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

The same approved v0.10 directory supplies two additional, unmodified faces:

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

SHA-256:

- Menu: `45e44b66abd41d65bc4529c9361c08c3bbd55fa9784b487dd4373eff9f966b37`
- Display: `dd64ebdcb8108b58806f34a9ebeb27f083a633a7a31175ab7945b0c4e0227f49`

All three files are embedded in the executable. Editable font-construction
sources and experiments remain on the original lab branch, not in this PR.

# Vangers Text

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
Menu and Display fonts are not imported here: they belong to the later iscreen
stage, not the actint text backend.

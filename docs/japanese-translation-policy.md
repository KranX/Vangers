# Japanese translation policy

Current Japanese assets in this branch should be treated as **draft localization**.

## Source priority

For Vangers text, Japanese translation must use this order of authority:

1. **Russian original** — primary source of meaning, lore, tone, and word choice.
2. **English localization** — secondary reference used only to clarify structure, intent, or already-solved adaptation problems.
3. **Current Japanese files** — working draft, to be revised over time.

## Why

The original game text was authored in Russian and carries:
- lore-specific terminology,
- idiosyncratic narration,
- wordplay,
- bios/counselor voice,
- culture-specific stylistic contrast between factions.

English is useful, but it is not the canonical source.

## Practical workflow

When translating or revising `*_jpn.text`, `*_jpn.query`, or UI catalogs:

1. open the original dual-language source file,
2. inspect the **Russian and English source blocks together**,
3. translate into Japanese from the **Russian meaning first**,
4. use English only as a support reference,
5. preserve faction voice and repeated terminology consistently.

## Helper

Use:

```bash
python scripts/translation/dual_text_view.py data/data/Podish.query --section '[мехос]'
```

Examples:

```bash
python scripts/translation/dual_text_view.py data/data/Lampasso.query --section '[Куклы]'
python scripts/translation/dual_text_view.py data/data/Inventory.text --section '[Poponka 0]'
```

The helper prints the original file with tagged `<<RU N>>` / `<<EN N>>` quote blocks in source order for safer Japanese revision work.

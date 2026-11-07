# UTF-8 in Diagen

Diagen stores dialogue text in UTF-8: phrases, question subjects and links,
answers, variants/comments, room/world/cycle display names and inventory/final
messages. Actint consumes these strings directly; selecting a subject does not
round-trip through CP866. This stage does not change fonts, relief, quest commands,
cell/molecule IDs, network messages or language selection. The subsequent
[stable-ID stage](diagen-stable-ids.md) separates question identity from display
labels and migrates the shipped resources; the current contracts below include it.

## Resources

- Packaged `diagen.text` / `diagen_eng.text` now use `DU` UTF-8 and explicit
  question/link IDs. Historical `DG` CP1251 archives remain readable through
  the compatibility adapter, decoded **once** at loading. Even the historical
  English archive contains CP1251 names in its shared room table.
- `DU` retains the entry layout: count (`int32`), then name byte length
  (`uint8`), NUL-terminated name, payload byte length (`int32`) and NUL-separated
  tokens. Payloads are validated and kept as UTF-8 without alphabet detection.
- `DIAGEN_TEST` accepts `utf8_text` on the first line of single-language source.
  Unmarked historical source remains CP1251 with bilingual filtering. Current
  localized sources are in `data/data/ru/` and `data/data/en/`; common scenario
  metadata stays shared. The compiler writes `DU` to `diagen_new.text`.
  Questions have `[#stable.id]` followed by `[localized label]`; use `{#stable.id}`
  for links. The older two-display-header syntax is a compatibility input only,
  not the authoring convention for new translations. Room-table lookup IDs stay
  separate from display names. See [`data/data/README.md`](../data/data/README.md).
- Existing compressed/encrypted archive loading still goes through `Parser`.
  FileBox checks the archive signature, entry bounds, terminated tokens and UTF-8
  payloads before exposing them to Diagen. Zero-length entries remain valid for
  empty question files. This does not harden the separate
  legacy compression parser against arbitrary malicious input.

The shared settings codec now includes a CP1251 decoder, including punctuation,
`Ё/ё` and `№`. Undefined CP1251 byte `0x98` becomes U+FFFD. Embedded NUL separators
are preserved. There is no encoding autodetection or additional codec framework.

## Text and ownership

ASCII syntax (`[]`, `{}`, `$()` and commands) is parsed as before. Substitution
uses a per-molecule `std::string`, not the old fixed 1311-byte buffer; translated
replacements may be longer than their placeholders. No UTF-8 sequence is split.
Diagen getters return borrowed UTF-8 pointers, not newly allocated converted
copies. A handled phrase remains valid until that molecule's next handled phrase
or dispatcher destruction; resource-backed strings live with the dispatcher.
Source token buffers are owned for as long as their phrases/commands borrow them.

Inventory/Threall messages enter `aciPrepareTextUtf8`; final screens use
`aciScreenText::alloc_utf8`. The existing `aciPrepareText` / `alloc` APIs retain
CP866 contracts for other producers (for example racing messages). Bitmap
fallback conversion happens in actint's output adapter, not in Diagen.

## Saves

The current nested Diagen save version is **5**, storing stable question IDs.
The preceding UTF-8 migration introduced version 4, with UTF-8 display labels;
versions 2/3 used CP1251 labels. All three old versions remain readable through
explicit historical-alias import. Room/molecule layout is unchanged. See
[stable-ID saves](diagen-stable-ids.md#saves) for language switching, progress
indices and missing-ID behavior. Old executables need not read new v5 saves.

Bounded string reads replace fixed 256-byte buffers. Length overflow, missing
terminators and invalid UTF-8 are rejected; the signed-short byte-length limit
remains 32767.

## Verification

`ctest` includes legacy RU/EN and Unicode RU/EN production-runtime fixtures,
source-to-archive round trips, shipped RU/EN archive loading, old v2/v3/v4 and new v5
save progress, links, variants, commands and full-screen text. Unicode fixtures
exercise mixed Latin/Cyrillic/CJK/emoji, subjects over 256 bytes, phrases over
1311 bytes and expanding substitutions. Malformed signatures/counts/lengths,
unterminated payloads and invalid UTF-8 source/archive data fail explicitly.
Empty legacy `DG` / native UTF-8 `DU` question entries and empty UTF-8 source
round trips are covered too; negative payload lengths remain invalid. All fixtures
and output are written in the build tree, not the source data.

Historical verification of the preceding UTF-8 stage (PR #697): 43/43 release tests; 10/10 codec/Diagen tests
with ASan, UBSan halt-on-error and normal leak checking. A strict whole-suite
sanitizer run reports a pre-existing invalid-vptr issue in
`aciScreenEventLine::aciScreenEventLine` (`aci_scr.cpp:319`), affecting four ACS
tests. That unchanged ACS allocation path is not part of this migration.
At that stage, recompiling the legacy sources reproduced all 29 entry payloads
after decoding. Current explicit-ID sources must reproduce the new archives
byte for byte, and their full parsed models are compared with the immutable
old corpus in `tests/fixtures/diagen-legacy.zip`.

This is an encoding migration, not a new translation, font fallback, shaping,
IME implementation or complete campaign/playthrough test. Unicode that is not
covered by the selected font can be stored/selected/saved correctly but still
needs a suitable font to be displayed.

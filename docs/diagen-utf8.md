# UTF-8 in Diagen

Diagen stores dialogue text in UTF-8: phrases, question subjects and links,
answers, variants/comments, room/world/cycle display names and inventory/final
messages. Actint consumes these strings directly; selecting a subject does not
round-trip through CP866. This stage does not change fonts, relief, quest commands,
cell/molecule IDs, network messages or language selection.

## Resources

- Existing `diagen.text` / `diagen_eng.text` archives have signature `DG` and
  CP1251 payloads. Both are decoded **once**, when loaded. Even the English
  archive contains CP1251 names in its shared room table. ASCII IDs and commands
  are unchanged. No shipped resource needs to be rewritten.
- New archives use signature `DU`, with the same entry layout: entry count
  (`int32`), then name byte length (`uint8`), NUL-terminated name, payload byte
  length (`int32`) and NUL-separated tokens. Payloads are validated and kept as
  UTF-8, without legacy conversion or alphabet-based language detection.
- The existing `DIAGEN_TEST` source compiler accepts `utf8_text` on the first
  line of each UTF-8 source file. Unmarked legacy source remains CP1251 with its
  historical bilingual filtering. Marked source is single-language and is not
  filtered by the first letter's alphabet. The compiler writes `DU` to
  `diagen_new.text`. The existing grammar is retained, including **two subject
  headers per query**: repeat the localized subject in both slots for a new
  single-language resource. Room-table lookup IDs stay separate from display names.
- Existing compressed/encrypted archive loading still goes through `Parser`.
  FileBox checks the archive signature, entry bounds, terminated tokens and UTF-8
  payloads before exposing them to Diagen. This does not harden the separate
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

The nested Diagen save version is now **4**. Query subjects are stored as UTF-8
with their byte length; room and molecule lookup IDs keep their existing layout.
Versions **2 and 3 remain readable**, with CP1251 subjects decoded on load.
Counters, visible questions, answer indices and cell states retain their layout
and meaning. Dynamic bounded string reads replace fixed 256-byte buffers;
length overflow, missing terminators and invalid v4 UTF-8 are rejected. The
existing signed-short byte-length limit for subjects remains 32767.
Old executables are not expected to read v4 Diagen saves.

## Verification

`ctest` includes legacy RU/EN and Unicode RU/EN production-runtime fixtures,
source-to-archive round trips, shipped RU/EN archive loading, old v2/v3 and new v4
save progress, links, variants, commands and full-screen text. Unicode fixtures
exercise mixed Latin/Cyrillic/CJK/emoji, subjects over 256 bytes, phrases over
1311 bytes and expanding substitutions. Malformed signatures/counts/lengths,
unterminated payloads and invalid UTF-8 source/archive data fail explicitly.
All fixtures and output are written in the build tree, not the source data.

Local verification on 2026-11-07: 41/41 release tests; 8/8 codec/Diagen tests
with ASan, UBSan halt-on-error and normal leak checking. A strict whole-suite
sanitizer run reports a pre-existing invalid-vptr issue in
`aciScreenEventLine::aciScreenEventLine` (`aci_scr.cpp:319`), affecting four ACS
tests. That unchanged ACS allocation path is not part of this migration.
Recompiling the shipped legacy sources in RU and EN produces the same 29 entry
payloads as each shipped archive after CP1251 decoding.

This is an encoding migration, not a new translation, font fallback, shaping,
IME implementation or complete campaign/playthrough test. Unicode that is not
covered by the selected font can be stored/selected/saved correctly but still
needs a suitable font to be displayed.

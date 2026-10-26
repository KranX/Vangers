# UTF-8 text in actint

Actint stores display text in UTF-8: HUD and shop panels, inventory labels and
comments, menu entries, dialogue lines and topics, world/location labels,
mouse-prompt sources, animated messages and full-screen text pages. Existing
bitmap output and shipped resources remain compatible. This does not migrate
ACS, iscreen, Diagen internals, the network protocol or save-file formats.

## Input and output boundaries

- Existing actint source/binary scripts contain CP866 display strings. The
  parser decodes them once, using the existing settings codec. Resource paths,
  location lookup keys, screen/object IDs and animation names remain opaque.
- New scripts can put `utf8_text` before their display strings. The flag is
  serialized as a new trailing option without renumbering existing options;
  source and binary readers then validate and retain UTF-8 directly. The flag
  applies to subsequent display strings in that parse, including included files.
  There is no encoding autodetection or rewrite of existing resource files.
  Mixing a CP866 include after this flag is an error, not an implicit reset.
  Invalid UTF-8 is reported through `handle_error` / `ErrH.Abort`, including the
  input resource, option and source-line/binary-offset diagnostic. The new
  `alloc_utf8` / `add_utf8_str` input boundaries also report invalid text through
  `ErrH.Abort`, with the API name, instead of leaking an exception into the game.
- Diagen phrases/topics and compass/player-name inputs are imported from their
  existing CP866 interfaces. Selected topics/compass names are encoded back at
  the call into the unchanged legacy subsystem, preserving lookup behavior.
- `aciPromptData::add_str` and `aciScreenText::alloc` retain their legacy CP866
  input contracts. New producers use `add_utf8_str` / `alloc_utf8` directly;
  copying and storage preserve characters absent from the old fonts.
- Actint uses explicitly named `*Utf8` width/draw entry points. They create a
  temporary bitmap view through the existing UTF-8 to CP866 codec. The original
  `aPutStr`, `aStrLen`, `aPutStr32`, etc. still accept CP866 for other callers.
  Width and drawing share the same glyph mapping, including the smaller
  numeric-only font: unrepresentable characters/out-of-range glyph indices use
  `?`, or a space when `?` is unavailable. Fonts with no space glyph (32 or fewer
  glyphs) produce an empty bitmap view, never an embedded NUL or an invalid index.
- XGR mouse-prompt copies, iscreen mechos captions, ACS toggle/slot labels and
  save metadata are encoded at those legacy boundaries. Save header lengths
  continue to describe CP866 bytes, not UTF-8 bytes.
  XGR's `set_text` borrows storage, so the actint adapter uses `init_text` to
  own the converted CP866 bytes. Repeated updates release the previous copy;
  unchanged hover text reuses it. XGR also owns the active displayed snapshot,
  since its source can be replaced while the mouse remains stationary.
  Active prompts are refreshed before capturing their background, so text and
  geometry change together. No hit, empty text, or a list switch clears stale
  state. A list switch never dereferences the old list: it may already be freed.

The `$` currency glyph, arrow/control glyphs, palettes and numeric highlighting
are unchanged. Source-code labels in `aci_str.h` are now UTF-8; they are not
resource files. Existing allocation/clone structures are retained rather than
replaced with a new UI/text framework. No dependencies or TTF backend are added.

## Text operations

Dialogue wrapping, prompt clipping, animated reveal and full-screen column
limits operate on complete Unicode scalars instead of UTF-8 bytes. Existing
dialogue leading spaces are retained. Long words can split at scalar boundaries
instead of disappearing, and animated prefixes no longer need a 256-byte buffer.
Prompt clipping modifies only a temporary presentation string, not `StrBuf`;
a wider redraw can display the full original text again.
This is not grapheme-aware editing, shaping, IME support or new glyph coverage.
ACS input and chat are unchanged, including their pre-existing input limitations.

Scalar-boundary helpers live in the neutral `text/utf8` module. Bitmap adapter
declarations live in `actint/raster_text.h`, separate from text utilities.
The animated help-text overlay lives in actint rather than the particle-lighting
module; lighting no longer includes or calls the actint text APIs.

## Validation

The `actint_text` CTest covers explicit encodings, applicable CP866 round-trips,
Unicode preservation, limited font coverage, icons, empty/long strings, clipping,
reveal boundaries and wrapping. Run the normal out-of-tree build and CTest suite;
the standalone module test also supports AddressSanitizer and UBSan:

```sh
c++ -std=c++17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Isrc tests/actint_text_test.cpp src/actint/text.cpp \
  src/text/utf8.cpp src/settings/text_encoding.cpp -o /tmp/vangers-actint-text-test
ASAN_OPTIONS=detect_leaks=1 /tmp/vangers-actint-text-test
```

The `actint_mouse_prompt` CTest runs the production UTF-8 adapter and XGR prompt
lifecycle, with only font measurement stubbed. It checks post-return reads,
short/long Russian labels, repeated updates, borrowed/owned transitions, both
constructors and the displayed snapshot's lifetime. It can also run standalone:

```sh
c++ -std=c++17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Isrc -Ilib/xtool -Ilib/xgraph tests/actint_mouse_prompt_test.cpp \
  lib/xgraph/xprompt.cpp src/settings/text_encoding.cpp \
  $(pkg-config --cflags --libs sdl3) -o /tmp/vangers-actint-mouse-prompt-test
ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1 \
  /tmp/vangers-actint-mouse-prompt-test
```

The original adapter's borrowed-pointer call was reproduced as an ASan
heap-use-after-free by changing only `init_text` back to `set_text` in an external
negative-control copy of the adapter. The fixed code passes ASan/UBSan and leak
detection, including repeated updates. The initial nine-test suite did not
exercise this lifetime boundary; that follow-up added the mouse-prompt test.
A separate negative control retaining a borrowed active snapshot also reproduces
an ASan heap-use-after-free after the prompt source is replaced.

The follow-up regression tests additionally cover the production boundaries:

The current integration-branch suite has 14 CTest entries, including these two:

- `actint_script_roundtrip` invokes the real actint source writer and binary
  reader in separate processes. It checks `utf8_text` (option 157), included
  files, CP866 compatibility, and non-CP866 characters in world/item/menu labels,
  templates and prompts. Invalid source input in each decoder path, a corrupted
  SCB payload, a mixed CP866 include, and invalid UTF-8 API inputs must produce
  an error dialog/log diagnostic, not an uncaught exception.
  Direct source fixtures without a trailing newline also round-trip; the final
  unlinked source line and binary-output filename are released on normal cleanup.
  Source buffers remain NUL-terminated after include expansion.
- `actint_runtime_regressions` calls real `InitPrompt` / `PutPrompt`, checks
  snapshot ownership, stationary updates, removed sources and list switches,
  and calls real prompt redraw with a zero-height font fixture to check narrow
  then wide layout without requiring video. It also constructs/destroys the
  ML resource buffers and a loaded shutter. ASan can check their `new[]` /
  `delete[]` pairing, including the adjacent event/flush/sequence arrays.
  A deliberately unaligned `XBuffer` round-trip additionally checks the packed
  scalar I/O used by SCB headers: byte copies replace typed references to
  unaligned storage, without changing the serialized representation.

Leak checking remains enabled for successful parsing and runtime-model cleanup.
The negative-input subprocesses deliberately call the terminating error handler
with live models; only those expected-abort cases disable leak-at-exit reporting.

These tests do not use the standalone **iscreen** compiler as an actint parser.
The common game objects are reused; only the script-mode-dependent translation
units are rebuilt for the two parser modes. SDL's platform entry point is a thin
separate file, allowing tests to link the runtime without starting the game.

The ownership follow-up was also checked in the game under GDB with isolated
RU/EN data and SDL dummy/software: all 13 initialized mouse prompts in each
language owned their CP866 storage. Actual `InitPrompt`/`PutPrompt` calls with
the real fonts retained an independent displayed snapshot and identical raster
buffers after replacing its source. These are targeted runtime checks, not a
new manual playthrough.

Local Linux validation on 24 October 2026 additionally checked:

- Actual SCB-loaded models: 489 strings for RU high/low resolution and 493 for
  EN high resolution, all valid UTF-8; RU has 468 non-ASCII strings.
- A new UTF-8 source script compiled and reloaded through the binary parser,
  retaining Cyrillic, extended Latin, Japanese and emoji. A real prompt clone
  retained the same unsupported characters, not their bitmap replacements.
- Widths and complete raster buffers matched the original CP866 API for all
  eight standard and four range fonts, using real font resources.
- RU/EN before/after captures covered initial dialogue, trade panels, inventory,
  prompts, pause and save screens. Initial menu/history/escave frames matched
  exactly; moving world/AVI frames are not deterministic pixel baselines.
- Manual-save and autosave names retained identical CP866 bytes, including
  Russian `Автозапись`; location metadata remained CP866. Cross-version loads
  succeeded in both directions for RU and old-save into new binary for EN.

These runtime checks used isolated data, SDL dummy/software, and synthetic input,
not GPU/HiDPI/physical keyboard/IME or a live multiplayer session. In the available
legacy assets, EN `aci_low.scb` fails decompression and the full RU source script
fails on an unknown token in both baseline and changed binaries; these two paths
are not counted as passing. Existing SCB files and original data were not changed.

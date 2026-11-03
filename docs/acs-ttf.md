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
  ACS uses palette ranges, NOT iscreen height-map relief. Colour states and
  centered alignment share the same layout. Control/icon cells stay bitmap.
  `font` fields reuse the existing actint UTF-8/TTF entry points.
- Bitmap mode encodes at presentation only and retains the existing drawing
  algorithm. Missing TTF glyphs affect presentation, never stored text.
- Background capture/restoration uses the existing XGR/native layer transport.
  Rectangle erasure also clears native text coverage, preventing stale overlays.
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
- Actual RU/EN pause and save screens were captured and visually inspected at
  800x600. The bitmap pause-control and save-label comparison regions matched
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

These are scripted SDL software-renderer checks, not a manual playthrough.

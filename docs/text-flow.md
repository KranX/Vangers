# Measured text layout

Stage 1 of [the localization roadmap](text-localization-roadmap.md).
Base: `feature/text-rendering` at `6b6b4a6` (PR #697). This stage changes layout,
not the accepted TTF outlines, relief material, palette or button animation.

## Shared contract

`src/text/layout.{h,cpp}` accepts canonical UTF-8, a logical width and a measurement
callback supplied by the consumer's active font. It returns owned display lines;
`paginate` groups them by the box's row capacity. There is no font/renderer/locale
registry in this module and no new dependency.

- `LineMode::Preserve` keeps authored LF/CRLF boundaries. It still wraps a line
  that is too wide: a translator can supply a whole paragraph on one source line.
- `LineMode::Paragraphs` treats single physical line endings as soft breaks.
  Empty lines remain paragraph gaps. This is explicit policy, not an encoding
  detector and not a heuristic applied to every historical resource.
- U+2028 forces a line break; form feed (`\f`) forces a page break. A line ending
  immediately following form feed does not create an extra empty display row.
- Words break at ASCII spaces/tabs. An overlong word is split at UTF-8 scalar
  boundaries, with guaranteed progress. NBSP is not discarded as a break space.
  A single glyph wider than the box is indivisible and may exceed that width.
- Metric queries use exponential/binary search bounded near a display line's
  length, rather than repeatedly measuring the entire remaining novel.
- Display layout is cached by its owner; it is not regenerated every animation
  frame. Window scaling does not change an unchanged logical box.

This is not a Unicode line-break/shaping engine: grapheme clusters, CJK punctuation
rules, hyphenation and bidi need script-specific work in later roadmap stages.

## Dialogue / InfoPanel

`aciInitPanel` measures using the panel font and spacing. Page capacity is bounded
by both `MaxStr` and its actual logical height. All lines are retained instead of
printing "Dialog phrase too long" and discarding the tail. The original strict
width and leading-wrap-space convention are retained for compatible appearance.

`InfoPanel` owns the visual pages. `aciNextPhrase` displays a pending page **before**
clearing the question menu or invoking Diagen. Thus extra visual pages neither
become new atoms nor rerun quest commands. Speech animation uses the existing
sequence. The existing end-of-speech lock is delayed until the last visual page;
so the original closing/locking sequence does not run while terminal/service
text is still unread. Replacing/clearing a phrase also clears its pending pages.

## Full-screen messages

`aciScreenText` retains `source_text` separately from its NUL-separated display
buffer and page/string tables. Layout is deferred until fonts exist, then measured
inside the logical screen with 60-unit horizontal and 40-unit vertical margins. It is rebuilt
only for a changed logical box/font/line spacing or replacement source. A genuine
logical reflow restarts the message, rather than keeping an invalid page index
and skipping text. Merely resizing a scaled window does not trigger reflow.

The old 55/60-character wrappers and `MaxStrLen` are removed. Authored legacy
resources retain hard breaks, the existing page-row cap and historical half-row
vertical offset. `alloc_utf8(text, LineMode::Paragraphs)` uses height-based pages
without that legacy cap. Existing `\\` page markers remain supported. Page tables
have an explicit null sentinel; reveal never consumes a following page's string.
Next/reset update the active page pointer immediately.

Full-screen text temporarily selects the full logical clipping rectangle and
restores the caller's rectangle afterwards. It must not inherit a narrower world
viewport and silently hide the right-hand part of a measured line.

## History / iscreen text objects

`iTextData` keeps decoded source separately from measured display lines. Both old
and new resources wrap by the row template's actual font and logical width.
Only template rows that fit its height consume text. Display rows own enough
bytes for the whole measured line; editable-field limits (49/511 scalars) do not
truncate these non-editable bodies. Input-field limits are unchanged.

By default authored hard breaks remain intact. For flowing paragraph resources:

```text
utf8_text
text_flow 700
text_object 1 "History" "resource/iscreen/text/locale/history.txt"
text_flow 0
```

`text_flow` is a width in logical UI units (1..800; 0 restores hard-break policy),
clamped to the existing template's width. It applies to subsequent `text_object`
declarations independently of `utf8_text`, including source/SCB round trips.
Its opcode is appended, so existing SCB option numbers are unchanged.

The existing text-screen transition/button scripts still perform next/previous
navigation. Explicit short pages are padded to the row capacity so these scripts
do not mix two pages. Returning to the preceding document selects its last page,
including exact-full, partial and empty cases. Static `text`/`s_text` lists (for
example credits) retain their authored layout; long paged prose belongs in a
`text_object`, not a static list of menu elements.

## Verification

Automated coverage exercises UTF-8 boundaries, measured width, paragraphs, hard
and page breaks, empty/final pages, long words, metric-query bounds, production
bitmap/TTF consumers, full-screen page state/clipping restoration, whole dialogue
continuations through `aciNextPhrase`, terminal-page speech locking, narrow rows exceeding the old input cap,
next/back navigation and source/SCB policy round trips. Existing input, codecs,
Diagen saves/events and renderer tests remain in the full suite.

Visual QA uses isolated copies of game assets and immutable pre-stage binary
`6b6b4a6`, with SDL dummy/software captures at 800x600. No private game assets,
saves or generated capture images are added to the repository. Standard RU/EN
history and dialogue states are compared with the accepted pre-stage TTF version;
flowing history and injected long dialogue are checked separately. Full-screen
QA opens a shipped inventory story through GDB in an isolated running game;
it does not claim that the corresponding quest/item was obtained in gameplay.

The unrelated pre-existing ACS invalid-vptr sanitizer failure remains separate.
On 2026-11-07 the release suite passed 48/48 tests in three consecutive runs.
Strict ASan/UBSan passed 44/44 tests after excluding the four tests blocked by
that unchanged ACS issue (`acs_script_roundtrip`, `acs_runtime`,
`acs_render_bitmap`, `acs_render_ttf`). A full strict run reproduced the same
`aciScreenEventLine` invalid-vptr at `aci_scr.cpp:319`; it is not a clean full
sanitizer run. Visual QA also checked paragraph mode at 1200x900: logical line
breaks stayed identical to 800x600. The original history/dialogue text regions
were pixel-identical in the sampled RU/EN states. This is not a claim that every
game screen or final quest outcome was manually played through.

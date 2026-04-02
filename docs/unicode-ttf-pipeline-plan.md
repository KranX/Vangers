# План полной переделки текстового пайплайна под SDL2_ttf и Unicode

## Зачем

Цель этой ветки — не просто заменить растровые шрифты на TTF, а перевести игру на **полноценный Unicode-пайплайн**, достаточный для:

- японского языка;
- нормального UTF-8 ввода и буфера обмена;
- отказа от текущих legacy-кодировок в рантайме;
- удаления хаков вида `English / Russian / German hack`.

Это отдельная экспериментальная ветка, поэтому план допускает крупную перестройку, но должен держаться за:

- **DRY** — один общий text core, а не четыре разных реализации;
- **KISS** — без лишней «универсальной платформы текста» на старте;
- **YAGNI** — не внедрять сложную типографику и shaping для всех письменностей сразу, если текущая цель — японский, английский и русский.

---

## 1. Текущее состояние

Сейчас в проекте нет одного общего текстового пайплайна. Есть несколько исторически независимых подсистем:

### 1.1 `iscreen` bitmap fonts

- `src/iscreen/hfont.h/.cpp`
- `src/iscreen/imap_fnc.cpp`
- `src/iscreen/iscreen.cpp`
- `src/iscreen/iscr_fnc.cpp`

Ресурсы:

- `data/resource/iscreen/fonts/hfont00..04.fnh`

Это legacy `HFont` + `HFntTable` + `iPutStr()` / `iStrLen()`.

### 1.2 `actint` bitmap fonts

- `src/actint/actint.h/.cpp`
- `src/actint/ascr_fnc.cpp`

Ресурсы:

- `data/resource/actint/fonts/sfont00..07.bml`

Это `aScrFonts`.

### 1.3 `actint` anti-aliased-like 32 fonts

- `src/actint/ascr_fnc.cpp`
- `src/iscreen/i_chat.cpp`

Ресурсы:

- `data/resource/actint/fonts/cfont00..03.bml`

Это `aScrFonts32`, `aOutText32()`, `aTextWidth32()`, `aTextHeight32()`.

### 1.4 ACS input/fonts

- `src/actint/aci_scr.cpp`

Ресурсы:

- `data/resource/actint/640x480/hfonts/font00..01.fnc`

Это ещё один отдельный `HFont`-path через `acsFntTable`.

### 1.5 HUD / road / surmap fixed fonts

- `road.cpp`
- `surmap/*`
- `lib/xgraph/xfont.cpp`

Ресурсы:

- `data/road.fnt`
- `data/zfont.fnt`
- built-in `XGR_FONT0_8x16`, `XGR_FONT0_8x8`

---

## 2. Главные архитектурные проблемы

### 2.1 Внутренняя строковая модель не Unicode

Сейчас в проекте смешаны:

- ASCII;
- runtime CP866 для русского;
- местами source cp1251-подобные данные;
- ручные конвертеры:
  - `Convert()`
  - `ConvertUTF8()`
  - `UTF8toCP866()`
  - `CP866toUTF8()`

Файлы с этим:

- `src/uvs/diagen.cpp`
- `src/iscreen/i_chat.cpp`
- `src/actint/aci_scr.cpp`
- `src/road.cpp`

### 2.2 Много логики завязано на `1 byte == 1 char`

Примеры:

- `strlen(...)`
- побайтовый курсор;
- `ptr[sz - 1] = '_'`;
- выделение/удаление по byte index;
- проверки ширины по legacy-ширине символов;
- фиксированные char buffers.

Это несовместимо с японским UTF-8 вводом и рендерингом.

### 2.3 Текущая локализация жёстко двухколоночная

Сейчас по сути модель:

- English
- Russian
- и местами отдельные German-ветки в коде

Примеры:

- `data/iscreen/scripts/strings.inc`
- `data/actint/a_str.inc`
- `src/iscreen/i_str.h`
- `lang() == RUSSIAN`
- `lang() == GERMAN`

Для японского так жить уже нельзя.

### 2.4 Нельзя просто заменить рендер

Если просто подключить `TTF_RenderUTF8_*`, не меняя пайплайн:

- ввод сломается;
- layout поплывёт;
- Japanese line breaking не заработает как надо;
- старые palette/buffer paths останутся отдельными островами.

---

## 3. Целевая архитектура

Нужен **единый text stack**.

### 3.1 Каноническая внутренняя кодировка

**UTF-8 для всех строковых данных в рантайме.**

Это касается:

- локализованных UI строк;
- динамического текста;
- input fields;
- chat;
- названий объектов;
- текстов из скриптов/диалогов.

### 3.2 Единый text core

Новый модуль, условно:

- `src/text/unicode.h/.cpp`
- `src/text/text_layout.h/.cpp`
- `src/text/text_renderer.h/.cpp`
- `src/text/font_manager.h/.cpp`
- `src/text/legacy_codec.h/.cpp`

### 3.3 Один font backend

`SDL2_ttf` должен стать **единственным источником глифов**.

Но вывод всё ещё должен уметь рисовать:

- в `XGR_VIDEOBUF`;
- в iscreen buffers;
- в actint buffers;
- в палитровые слои.

То есть нужен не прямой `SDL_RenderCopy`-path, а **TTF glyph cache + наши blitters**.

### 3.4 Один layout path

Единый код для:

- измерения текста;
- line height;
- baseline;
- kerning;
- wrapping;
- horizontal alignment;
- cursor x-position;
- selection range.

Не должно быть отдельных несовместимых реализаций для:

- `iPutStr`
- `aOutText32`
- chat
- ACS input

### 3.5 Один input model

Нужен единый Unicode input path:

- `SDL_TEXTINPUT`
- `SDL_TEXTEDITING`
- cursor / selection на codepoint indices
- clipboard как UTF-8
- IME preedit support

Без этого японский ввод не взлетит.

---

## 4. Принципы реализации

### 4.1 Не мигрировать всё за один коммит

Нужны поэтапные совместимые слои:

1. новый backend;
2. адаптеры;
3. постепенное переключение подсистем;
4. удаление legacy.

### 4.2 Не плодить по backend на каждую подсистему

Плохо:

- `TTFForIsc`,
- `TTFForActint`,
- `TTFForChat`,
- `TTFForHUD`.

Правильно:

- один `FontManager`;
- один `TextLayout`;
- один `TextRenderer`;
- несколько thin adapters.

### 4.3 Не использовать `TTF_RenderUTF8_*_Wrapped` как главный layout engine

Для японского это слабая база:

- wrap там ориентирован на пробелы и стандартные word-boundaries;
- японский требует собственного line breaking.

Поэтому:

- SDL2_ttf используем для **glyph rasterization** и базовых metrics;
- wrapping делаем сами.

### 4.4 Не внедрять слишком рано «идеальный shaping engine»

Для японского, английского и русского достаточно:

- UTF-8;
- codepoint iteration;
- fallback fonts;
- line breaking для CJK;
- IME.

Полноценный HarfBuzz-like pipeline нужен позже, если появятся:

- арабский;
- индийские письменности;
- сложная typographic shaping.

---

## 5. Новый целевой пайплайн данных

### 5.1 Хранение строк

В рантайме все строки должны быть:

- `std::string` в UTF-8

Не `char*` с implicit CP866 semantics.

### 5.2 Индексация текста

Для input/layout нужен минимум:

- UTF-8 byte offset
- codepoint index

В идеале:

- отдельный helper итерации по Unicode codepoints

Для текущих целевых языков этого достаточно.

Полные grapheme clusters можно отложить.

### 5.3 Локализация

Вместо схемы `STR_XXX1 / STR_XXX2` нужен runtime locale catalog.

Простой вариант:

- `data/locale/en/*.txt`
- `data/locale/ru/*.txt`
- `data/locale/ja/*.txt`

Формат:

- UTF-8
- `key=value`

Без JSON/XML/лишнего DSL.

Это соответствует KISS.

### 5.4 Старые legacy-файлы

На переходный период нужен import layer:

- старые cp866/cp1251-ish строки загружаются;
- конвертируются в UTF-8;
- дальше живут уже как Unicode.

---

## 6. Новый font subsystem

### 6.1 Логические font roles

Нельзя привязываться к текущему `font=0..7` как к формату ресурсов.

Нужно разделить:

- `UiSmall`
- `UiNormal`
- `UiLarge`
- `ActIntMenu`
- `ActIntPrompt`
- `Chat`
- `HudMono`
- `RoadDebug`

А уже потом маппить старые ID на эти роли.

### 6.2 Font family + fallback chain

Для каждого logical font role нужна цепочка:

- primary font
- fallback 1
- fallback 2

Например:

- Latin/Cyrillic UI face
- CJK fallback face
- symbol fallback

Это обязательно для японского.

### 6.3 Glyph cache

Нужен cache по ключу:

- font role
- size
- style flags
- palette mode / render mode
- codepoint

Храним:

- alpha bitmap
- width / height
- bearing X / Y
- advance

### 6.4 Font metrics API

Единый слой должен выдавать:

- ascent
- descent
- line skip
- glyph advance
- text width
- measured wrap extent

---

## 7. Новый layout subsystem

### 7.1 Что он должен уметь

- single-line width
- multi-line height
- alignment
- clipping
- line breaking
- cursor hit-testing
- selection rectangles

### 7.2 Japanese line breaking

Нужен собственный line breaker:

- break on explicit `\n`
- break after CJK characters
- не разрывать UTF-8 sequence
- не опираться только на пробелы

Минимально достаточный вариант:

- UAX14-lite без полной реализации стандарта
- специальные правила для ASCII words + CJK characters

Этого достаточно для первой японской версии.

### 7.3 Cursor / selection

Нужны операции:

- move left/right by codepoint
- backspace/delete by codepoint
- selection by codepoint range
- convert codepoint range → pixel x

---

## 8. Новый rendering subsystem

### 8.1 Не прямой SDL texture text path

Основной renderer должен уметь:

- отрисовать текст в текущий software/palette buffer;
- использовать SDL2_ttf только для получения glyph alpha;
- затем класть пиксели в старые буферы через общие blitters.

### 8.2 Render modes

Нужно поддержать как минимум:

1. **Solid palette text**
   - замена `iPutStr`
   - замена `sysfont.draw`

2. **Ramp / grayscale text**
   - замена `aOutText32`
   - замена `aPutStr32`

3. **Text-to-buffer**
   - для offscreen UI буферов

### 8.3 Цвет

Новый renderer должен принимать не только “один SDL_Color”, а абстракцию вида:

- fixed palette color;
- palette ramp base + intensity shift;
- optional shadow/outline later.

---

## 9. Input и IME

Это отдельный обязательный блок.

### 9.1 Что есть сейчас

Сейчас:

- UTF-8 из SDL режется вручную;
- Cyrillic маппится в CP866;
- Japanese IME не поддерживается.

Файлы:

- `src/iscreen/i_chat.cpp`
- `src/actint/aci_scr.cpp`
- `src/iscreen/iscreen.cpp`

### 9.2 Что нужно

Новый input model:

- text buffer в UTF-8
- cursor position в codepoints
- selection range
- preedit string
- preedit cursor
- committed string

### 9.3 SDL events

Нужно использовать:

- `SDL_TEXTINPUT`
- `SDL_TEXTEDITING`

А не пытаться самим собирать символы из `SDL_KEYDOWN`.

### 9.4 Что откладываем

Не надо сразу делать полноценный rich text editor.

Достаточно:

- обычный single-line input;
- chat input;
- basic selection;
- clipboard;
- IME composition display.

---

## 10. Миграция локализации

### 10.1 Краткосрочно

Сделать loader, который понимает старые ресурсы и отдаёт UTF-8.

### 10.2 Среднесрочно

Перевести все string resources в UTF-8:

- `data/iscreen/scripts/strings*.inc`
- `data/actint/a_str*.inc`
- `data/actint/acs_str*.inc`
- остальные текстовые include-файлы

### 10.3 Долгосрочно

Убрать старую модель:

- `lang() == RUSSIAN`
- `lang() == GERMAN`
- `STR_XXX1 / STR_XXX2`

и заменить её на:

- `LocaleId`
- runtime catalog lookup

---

## 11. Поэтапный roadmap

## Phase 0 — подготовка

### Цель

Подготовить почву, ничего не ломая.

### Сделать

- добавить `SDL2_ttf` в CMake;
- добавить `FindSDL2_ttf.cmake`;
- добавить feature flag:
  - `VANGERS_TTF_TEXT_EXPERIMENT`
- завести `docs/` и этот план;
- выбрать initial TTF набор:
  - Latin/Cyrillic UI
  - Japanese fallback

### Результат

Сборка знает про SDL2_ttf, но старый рендер ещё жив.

---

## Phase 1 — Unicode utilities

### Цель

Создать единый low-level слой Unicode.

### Новые файлы

- `src/text/unicode.h/.cpp`
- `src/text/legacy_codec.h/.cpp`

### Сделать

- UTF-8 iteration;
- codepoint count;
- byte offset ↔ codepoint index;
- cp866 → utf8;
- utf8 → cp866 only as temporary compatibility helper;
- cp1251/source legacy → utf8 import helper.

### Результат

Дублированные `UTF8toCP866` / `CP866toUTF8` / `Convert` начинают уходить в один модуль.

---

## Phase 2 — FontManager + GlyphCache

### Цель

Сделать новый backend без переключения UI.

### Новые файлы

- `src/text/font_manager.h/.cpp`
- `src/text/glyph_cache.h/.cpp`

### Сделать

- `TTF_Init` / `TTF_Quit`;
- открытие font faces;
- fallback chain;
- `TTF_GlyphIsProvided32`;
- `TTF_GlyphMetrics32`;
- glyph alpha rasterization;
- cache.

### Результат

Есть рабочий TTF backend, пока без интеграции во все экраны.

---

## Phase 3 — TextLayout

### Цель

Отделить измерение и line breaking от рисования.

### Новые файлы

- `src/text/text_layout.h/.cpp`
- `src/text/line_break.h/.cpp`

### Сделать

- measure line width;
- measure multiline block;
- CJK-friendly wrapping;
- cursor x mapping;
- selection rectangles.

### Результат

Layout больше не зависит от legacy bitmap tables.

---

## Phase 4 — TextRenderer

### Цель

Научиться рисовать TTF-глифы в текущие игровые буферы.

### Новые файлы

- `src/text/text_renderer.h/.cpp`

### Сделать

- palette blit mode;
- grayscale ramp mode;
- text-to-buffer mode;
- draw glyph run;
- draw line;
- draw multiline block.

### Результат

Новый текст может жить в старом software renderer.

---

## Phase 5 — Pilot migration: `aScrFonts32`

### Почему первым

Это самый чистый pilot:

- anti-aliased path;
- чат уже частично живёт рядом с UTF-8/clipboard;
- меньше зависимости от terrain text logic.

### Файлы

- `src/actint/ascr_fnc.cpp`
- `src/iscreen/i_chat.cpp`

### Заменить

- `aOutText32`
- `aTextWidth32`
- `aTextHeight32`
- `aPutStr32`

### Результат

Chat и часть actint получают TTF backend первыми.

---

## Phase 6 — Unicode input rewrite

### Файлы

- `src/iscreen/i_chat.cpp`
- `src/iscreen/iscreen.cpp`
- `src/actint/aci_scr.cpp`
- новый `src/text/text_input_model.h/.cpp`

### Сделать

- буферы input в UTF-8;
- cursor/selection по codepoint index;
- `SDL_TEXTINPUT`;
- `SDL_TEXTEDITING`;
- preedit drawing;
- clipboard in/out UTF-8.

### Результат

Японский ввод становится технически возможным.

---

## Phase 7 — iscreen string pipeline

### Файлы

- `src/iscreen/imap_fnc.cpp`
- `src/iscreen/iscreen.cpp`
- `src/iscreen/iscr_fnc.cpp`

### Заменить

- `HFntTable`
- `iPutStr`
- `iPutStr2buf`
- `iStrLen`
- `i_terrPutStr`

### Результат

Основное menu/iscreen rendering уходит с `.fnh`.

---

## Phase 8 — actint / ACS migration

### Файлы

- `src/actint/aci_scr.cpp`
- `src/actint/ascr_fnc.cpp`
- `src/actint/actint.cpp`

### Заменить

- `acsFntTable`
- `aScrFonts`
- legacy string measuring in actint

### Результат

ActInt и ACS уходят на новый text stack.

---

## Phase 9 — localization migration

### Сделать

- runtime locale catalogs;
- UTF-8 resources;
- locale lookup API;
- подготовить `ja` каталог;
- перенести текущие `en/ru` строки.

### Результат

Добавление японского перестаёт требовать новых макро-хаков.

---

## Phase 10 — road / HUD / surmap

### Что делать

Отдельно решить, какие пути действительно надо мигрировать:

- HUD;
- debug overlays;
- chat over road;
- surmap editor.

### Важно

Это уже поздняя фаза.
Не надо тормозить японский UI из-за полного переписывания `sqFont` в первый месяц.

---

## Phase 11 — cleanup

Удалить:

- `HFont` runtime use;
- `aciFont` bitmap runtime use;
- `Convert()`-based text rendering;
- CP866 assumptions в UI/input;
- German special-cases, где их заменил locale catalog.

---

## 12. Что можно оставить временно

Чтобы не утонуть:

- старые font resource loaders — временно;
- legacy codecs — временно;
- часть HUD/system/debug fonts — временно;
- `lang()` enum — временно, пока не введён нормальный locale manager.

---

## 13. Что нельзя откладывать, если цель — японский

Это обязательный минимум:

1. UTF-8 рантайм строки  
2. Unicode input model  
3. IME support  
4. TTF backend with fallback fonts  
5. CJK-friendly line breaking  
6. runtime localization catalogs  

Без этих шести пунктов японский будет только «частично показываться».

---

## 14. Риски

### 14.1 Layout drift

TTF метрики не совпадут с bitmap fonts.

Нужно будет хранить per-font tuning:

- size;
- line skip;
- extra spacing;
- baseline offset.

### 14.2 Partial migration hell

Если долго держать 3-4 параллельных текстовых пути, будет больно.

Поэтому после каждого этапа нужно:

- выбирать один следующий subsystem;
- доводить его до конца;
- и закрывать legacy use-sites.

### 14.3 Input regression

IME и selection — самая хрупкая часть.

Нужно обязательно тестировать:

- Latin
- Russian
- Japanese IME
- clipboard
- backspace/delete
- left/right/home/end

### 14.4 Font asset licensing

Для японского нужен шрифт с нормальным CJK coverage и совместимой лицензией.

Это надо решить в самом начале ветки.

---

## 15. Рекомендованный порядок реальной работы

Если делать practically:

1. `SDL2_ttf` + CMake  
2. `unicode + legacy_codec`  
3. `FontManager + GlyphCache`  
4. `TextLayout`  
5. `TextRenderer`  
6. pilot на `aScrFonts32`  
7. input/IME rewrite  
8. `iscreen` migration  
9. `actint/ACS` migration  
10. locale catalogs  
11. Japanese content integration  
12. cleanup legacy fonts/codecs  

---

## 16. Definition of done

Можно считать миграцию состоявшейся, когда:

- все UI строки в рантайме — UTF-8;
- английский / русский / японский выбираются из locale catalogs;
- ввод в chat и UI fields работает через Unicode + IME;
- `iscreen`, `actint`, `ACS` и chat используют один text core;
- bitmap font resources больше не нужны для основных игровых UI-путей;
- German больше не живёт как ad-hoc кодовый хак.

---

## 17. References

- SDL2_ttf `TTF_Init`
- SDL2_ttf `TTF_WasInit`
- SDL2_ttf `TTF_OpenFont`
- SDL2_ttf `TTF_SetFontHinting`
- SDL2_ttf `TTF_GlyphIsProvided32`
- SDL2_ttf `TTF_GlyphMetrics32`
- SDL2_ttf `TTF_SizeUTF8`
- SDL2_ttf `TTF_MeasureUTF8`
- SDL2_ttf `TTF_RenderGlyph32_Blended`
- SDL2_ttf `TTF_RenderGlyph32_Solid`

Официальная документация:

- https://wiki.libsdl.org/SDL2_ttf


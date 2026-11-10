# Текстовые описания предметов и мехосов

Этап 4 локально реализован в `feature/text-item-descriptions` от интеграционного
`ac19203` (PR #700). Описания магазина больше не требуют AVI с изображением
букв: текст находится в `data/locales/{en,ru}/strings.toml`, выводится существующим
`TtfFont` и переносится общим `text::layout_lines`.

## Контракт отображения

- Меняется только текстовое представление в прежнем AVI-окне 340×255.
  Настоящие видео, камеры, рамка IBS, видимость и анимация переключения остаются
  прежними. Правая кнопка мыши по-прежнему переключает изображение и описание.
- Ключ выбирается по `invItem::ID` либо `invMatrix::type`.
  `internalID` мехоса может повторяться и для этого не подходит.
  Имена на экране и пути AVI не используются для поиска перевода.
- В штатном текстовом режиме не читаются даже старые слоты текстовых AVI.
  Достаточно реального видео/изображения и строк каталога. Неизвестные mod-ID
  сохраняют прежний AVI-путь; проверяются границы массива представлений.
- Заголовок отделяется U+2028, остальное хранится абзацами без ручных переносов.
  Переносы рассчитываются по метрикам TTF. Все строки центрированы по горизонтали.
- `description_style` в manifest выбирает фиксированное оформление:
  `regular-top` (EN, значение по умолчанию), `regular-centered`
  и `compact-centered` (RU).
  Любая локаль может выбрать любой профиль; проверок языка в рендерере нет.
  Это сохраняет различие исходных кадров: EN привязан к верху, RU центрирован.
  Таблица `[description_styles]` позволяет задать оформление конкретного блока:
  исходный Arial-кадр eLeech центрирован и в EN. Эта настройка тоже находится
  в данных пакета, а не в условии по ID внутри рендерера.
- **Нет прокрутки, новых страниц, автоматического уменьшения шрифта или
  скрытого хвоста.** Непоместившийся перевод отклоняется с ключом и диагностикой;
  переводчик должен исправить текст. Оформление не подбирается по длине.
- Кэшируется готовая RGBA-поверхность выбранного описания, а не кадры AVI.
  В обычном кадре выполняется копирование, при анимации — прежнее затемнение.
  Формирование глифов и переносы не выполняются каждый кадр.
- Эта область остаётся в логическом разрешении интерфейса, как прежние AVI.
  Используется общая TTF-гарнитура Text, в том числе вместо редких Arial-кадров;
  попиксельная копия старых видео-гарнитур не заявляется. Параметры прочего
  плоского/рельефного текста не меняются. Описания используют TTF и при
  диагностическом отключении TTF для остальных UI-ролей.
- SCB-формат и существующие скрипты не меняются. Новая локаль использует общий
  английский fallback и диагностику каталогов. Новые алфавиты и shaping/RTL
  остаются этапом 5, наличие UTF-8 само по себе их не реализует.

## Полнота переноса

Аудит `aci_iscr.inc` и макросов `actint.inc` охватывает 78 предметов и
39 вариантов мехосов. В исходных данных имеются 168 однокадровых AVI
340×255 с PNG-кодеком: 120 в `items/text`, 48 в `text`.
Из них 166 используются штатными моделями. `items/text/itm23{,r}.avi`
не используются: оба состояния контейнера ссылаются на `itm24{,r}.avi`.
Неиспользуемые кадры не подменяют активный русский Нюхонтейнер.

Добавлено по 92 ключа (83 непустых и 9 пустых); вместе с прежними строками
в каждом пакете 726 ключей. Общие/разряженные состояния сохраняют исходное
соответствие. RU/EN вычитаны по исходным кадрам; физические переносы заменены
автоматической вёрсткой. Пунктуация ASCII исключает OCR-кавычки, которых нет
в текущем TTF. Исторические опечатки вроде «знют» и «оставилили» не выдаются
за новые смысловые формулировки.

### Отсутствующие исходники

Скрипты ссылаются на 18 отсутствующих файлов (9 описаний × 2 языка).
Соответствующие строки оставлены пустыми: игра показывает пустую область,
не сочиняет описание и не пытается открыть отсутствующий AVI.
Для наполнения этих записей нужен отдельный авторский текст, а затем перевод.

| Игровой ID | Авторский токен | Название RU | Ключ |
| --- | --- | --- | --- |
| 20 | ITM20 | Lhark | `descriptions.item.20` |
| 21 | ITM21 | Lhark | `descriptions.item.21` |
| 34 | ITM34 | Ядробус | `descriptions.item.34` |
| 35 | ITM35 | Пипетка | `descriptions.item.35` |
| 36 | ITM36 | Маленький Цык | `descriptions.item.36` |
| 44 | ITM42 | Нечто вонючее | `descriptions.item.42` |
| 78 | ITM71 | Глиняные черепки | `descriptions.item.71` |
| 76 | ITM72 | Вонючая кашица | `descriptions.item.72` |
| 77 | ITM77 | Куча гнилья | `descriptions.item.77` |

### Исправленные дефекты английских кадров и остаток перевода

- `mechos.06` (Iron Wraith): удалён чужой хвост из Oxidized Monk после
  «gulp you down.»; исправлено очевидное «you've already dead» на
  «you're already dead». Новых сведений/перевода не добавлено.
- `mechos.08` (Sandrider): после законченного предложения удалено висящее
  «Since». В исходном EN отсутствует последнее предложение RU:
  **«С тех пор никто не хочет пробовать даже сесть на него.»**
  Оно требует нового перевода и здесь не дописывается.
- RU Пупки/Окорока указывают по 5 ракет, EN Pupok/Okorok — 10 и 8.
  Значения исходных описаний сохранены; перед смысловым редактированием нужно
  сверить актуальные игровые параметры. Перенос не меняет количество боеприпасов.
- Остальные различия формулировок не переписываются новым переводом в этом PR.

## Проверки

`item_description_map` читает настоящую секцию моделей с обоими wrapper-профилями
и RU/EN, прогоняет source → SCB → runtime и сверяет все 117 игровых ID.
Отдельные тесты проверяют переключение с единственным слотом настоящего видео,
неизвестный mod-ID и отсутствие слота камеры, загрузку без AVI, смену кэша,
копирование/освобождение/повторное открытие, пустые записи, оба оформления,
границы и fade. Проверяется покрытие шрифтом всех символов новых RU/EN строк.

`item_descriptions` рассчитывает и рисует все 92 записи обоих языков.
Переполнение, новый page break и невалидный UTF-8 должны отвергаться.
Исходные checksum-проверки прежних 634 UI-строк сохранены независимо.

Для экспорта кадров сравнения:
```sh
description_test /path/to/data/locales /path/to/comparisons
```
Требуются только открытые ресурсы репозитория, не купленные AVI.

Визуальная QA дополнительно использует настоящую игру, штатный вход в магазин,
правый клик, смену категории и повторное переключение в RU/EN. В новой тестовой
копии удалены обе директории текстовых AVI, настоящие видео сохранены.
SDL-ввод здесь синтетический; это не проверка физической торговли/покупки.

## Карта моделей

В колонке AVI указан исходный ресурс английского кадра; русский имеет суффикс
`r`. Это **аудиторская ссылка**, не runtime-таблица поиска по имени файла.

### Предметы

| ID | Токен | Ключ | Исходный AVI |
| --- | --- | --- | --- |
| 1 | ITM01 | `descriptions.item.01` | `items/text/itm01.avi` |
| 2 | ITM02 | `descriptions.item.02` | `items/text/itm02.avi` |
| 3 | ITM03 | `descriptions.item.03` | `items/text/itm03.avi` |
| 4 | ITM04 | `descriptions.item.04` | `items/text/itm04.avi` |
| 5 | ITM05 | `descriptions.item.05` | `items/text/itm05.avi` |
| 6 | ITM06 | `descriptions.item.06` | `items/text/itm06.avi` |
| 7 | ITM07 | `descriptions.item.07` | `items/text/itm07.avi` |
| 8 | ITM08 | `descriptions.item.08` | `items/text/itm08.avi` |
| 9 | ITM09 | `descriptions.item.09` | `items/text/itm09.avi` |
| 10 | ITM10 | `descriptions.item.10` | `items/text/itm10.avi` |
| 11 | ITM11 | `descriptions.item.11` | `items/text/itm11.avi` |
| 12 | ITM12 | `descriptions.item.12` | `items/text/itm12.avi` |
| 13 | ITM13 | `descriptions.item.13` | `items/text/itm13.avi` |
| 14 | ITM14 | `descriptions.item.14` | `items/text/itm14.avi` |
| 15 | ITM15 | `descriptions.item.14` | `items/text/itm14.avi` |
| 16 | ITM16 | `descriptions.item.16` | `items/text/itm16.avi` |
| 17 | ITM17 | `descriptions.item.16` | `items/text/itm16.avi` |
| 18 | ITM18 | `descriptions.item.18` | `items/text/itm18.avi` |
| 19 | ITM19 | `descriptions.item.18` | `items/text/itm18.avi` |
| 20 | ITM20 | `descriptions.item.20` | `items/text/itm20.avi` — отсутствует |
| 21 | ITM21 | `descriptions.item.21` | `items/text/itm21.avi` — отсутствует |
| 22 | ITM22 | `descriptions.item.22` | `items/text/itm22.avi` |
| 23 | ITM23 | `descriptions.item.24` | `items/text/itm24.avi` |
| 24 | ITM24 | `descriptions.item.24` | `items/text/itm24.avi` |
| 25 | ITM25 | `descriptions.item.26` | `items/text/itm26.avi` |
| 26 | ITM26 | `descriptions.item.26` | `items/text/itm26.avi` |
| 27 | ITM27 | `descriptions.item.27` | `items/text/itm27.avi` |
| 28 | ITM28 | `descriptions.item.28` | `items/text/itm28.avi` |
| 29 | ITM29 | `descriptions.item.29` | `items/text/itm29.avi` |
| 30 | ITM30 | `descriptions.item.30` | `items/text/itm30.avi` |
| 31 | ITM31 | `descriptions.item.31` | `items/text/itm31.avi` |
| 32 | ITM32 | `descriptions.item.32` | `items/text/itm32.avi` |
| 33 | ITM33 | `descriptions.item.33` | `items/text/itm33.avi` |
| 34 | ITM34 | `descriptions.item.34` | `items/text/itm34.avi` — отсутствует |
| 35 | ITM35 | `descriptions.item.35` | `items/text/itm35.avi` — отсутствует |
| 36 | ITM36 | `descriptions.item.36` | `items/text/itm36.avi` — отсутствует |
| 37 | ITM37 | `descriptions.item.37` | `items/text/itm37.avi` |
| 38 | ITM38 | `descriptions.item.38` | `items/text/itm38.avi` |
| 39 | ITM39 | `descriptions.item.39` | `items/text/itm39.avi` |
| 40 | ITM40 | `descriptions.item.40` | `items/text/itm40.avi` |
| 41 | ITM56 | `descriptions.item.56` | `items/text/itm56.avi` |
| 42 | ITM55 | `descriptions.item.55` | `items/text/itm55.avi` |
| 43 | ITM41 | `descriptions.item.41` | `items/text/itm41.avi` |
| 44 | ITM42 | `descriptions.item.42` | `items/text/itm42.avi` — отсутствует |
| 45 | ITM43 | `descriptions.item.43` | `items/text/itm43.avi` |
| 46 | ITM44 | `descriptions.item.44` | `items/text/itm44.avi` |
| 47 | ITM45 | `descriptions.item.45` | `items/text/itm45.avi` |
| 48 | ITM46 | `descriptions.item.46` | `items/text/itm46.avi` |
| 49 | ITM47 | `descriptions.item.47` | `items/text/itm47.avi` |
| 50 | ITM48 | `descriptions.item.48` | `items/text/itm48.avi` |
| 51 | ITM49 | `descriptions.item.49` | `items/text/itm49.avi` |
| 52 | ITM50 | `descriptions.item.50` | `items/text/itm50.avi` |
| 53 | ITM51 | `descriptions.item.51` | `items/text/itm51.avi` |
| 54 | ITM52 | `descriptions.item.52` | `items/text/itm52.avi` |
| 55 | ITM53 | `descriptions.item.53` | `items/text/itm53.avi` |
| 56 | ITM54 | `descriptions.item.54` | `items/text/itm54.avi` |
| 57 | ITM61 | `descriptions.item.61` | `items/text/itm61.avi` |
| 58 | ITM59 | `descriptions.item.59` | `items/text/itm59.avi` |
| 59 | ITM57 | `descriptions.item.57` | `items/text/itm57.avi` |
| 60 | ITM58 | `descriptions.item.58` | `items/text/itm58.avi` |
| 61 | ITM60 | `descriptions.item.60` | `items/text/itm60.avi` |
| 62 | ITM62 | `descriptions.item.62` | `items/text/itm62.avi` |
| 63 | ITM63 | `descriptions.item.63` | `items/text/itm63.avi` |
| 64 | ITM64 | `descriptions.item.64` | `items/text/itm64.avi` |
| 65 | ITM65 | `descriptions.item.65` | `items/text/itm65.avi` |
| 66 | ITM66 | `descriptions.item.66` | `items/text/itm66.avi` |
| 67 | ITM67 | `descriptions.item.67` | `items/text/itm67.avi` |
| 68 | ITM68 | `descriptions.item.68` | `items/text/itm68.avi` |
| 69 | ITM69 | `descriptions.item.69` | `items/text/itm69.avi` |
| 70 | ITM70 | `descriptions.item.70` | `items/text/itm70.avi` |
| 71 | ITM73 | `descriptions.item.60` | `items/text/itm60.avi` |
| 72 | ITM74 | `descriptions.item.60` | `items/text/itm60.avi` |
| 73 | ITM75 | `descriptions.item.61` | `items/text/itm61.avi` |
| 74 | ITM76 | `descriptions.item.59` | `items/text/itm59.avi` |
| 75 | ITM78 | `descriptions.item.57` | `items/text/itm57.avi` |
| 76 | ITM72 | `descriptions.item.72` | `items/text/itm72.avi` — отсутствует |
| 77 | ITM77 | `descriptions.item.77` | `items/text/itm77.avi` — отсутствует |
| 78 | ITM71 | `descriptions.item.71` | `items/text/itm71.avi` — отсутствует |

### Мехосы

| type | Токен | Ключ | Исходный AVI |
| --- | --- | --- | --- |
| 0 | MECH00 | `descriptions.mechos.00` | `text/mech00.avi` |
| 1 | MECH01 | `descriptions.mechos.01` | `text/mech01.avi` |
| 2 | MECH02 | `descriptions.mechos.02` | `text/mech02.avi` |
| 3 | MECH03 | `descriptions.mechos.03` | `text/mech03.avi` |
| 4 | MECH04 | `descriptions.mechos.04` | `text/mech04.avi` |
| 5 | MECH05 | `descriptions.mechos.05` | `text/mech05.avi` |
| 6 | MECH06 | `descriptions.mechos.06` | `text/mech06.avi` |
| 7 | MECH07 | `descriptions.mechos.07` | `text/mech07.avi` |
| 8 | MECH09 | `descriptions.mechos.09` | `text/mech09.avi` |
| 9 | MECH10 | `descriptions.mechos.10` | `text/mech10.avi` |
| 10 | MECH11 | `descriptions.mechos.11` | `text/mech11.avi` |
| 11 | MECH12 | `descriptions.mechos.12` | `text/mech12.avi` |
| 12 | MECH13 | `descriptions.mechos.13` | `text/mech13.avi` |
| 13 | MECH14 | `descriptions.mechos.14` | `text/mech14.avi` |
| 14 | MECH18 | `descriptions.mechos.18` | `text/mech18.avi` |
| 15 | MECH19 | `descriptions.mechos.19` | `text/mech19.avi` |
| 16 | MECH20 | `descriptions.mechos.20` | `text/mech20.avi` |
| 17 | MECH21 | `descriptions.mechos.21` | `text/mech21.avi` |
| 18 | MECH22 | `descriptions.mechos.22` | `text/mech22.avi` |
| 19 | MECH23 | `descriptions.mechos.08` | `text/mech08.avi` |
| 20 | MECH24 | `descriptions.mechos.08` | `text/mech08.avi` |
| 21 | MECH25 | `descriptions.mechos.08` | `text/mech08.avi` |
| 22 | MECH08 | `descriptions.mechos.08` | `text/mech08.avi` |
| 23 | MECH26 | `descriptions.mechos.15` | `text/mech15.avi` |
| 24 | MECH27 | `descriptions.mechos.15` | `text/mech15.avi` |
| 25 | MECH28 | `descriptions.mechos.15` | `text/mech15.avi` |
| 26 | MECH15 | `descriptions.mechos.15` | `text/mech15.avi` |
| 27 | MECH29 | `descriptions.mechos.16` | `text/mech16.avi` |
| 28 | MECH30 | `descriptions.mechos.16` | `text/mech16.avi` |
| 29 | MECH31 | `descriptions.mechos.16` | `text/mech16.avi` |
| 30 | MECH16 | `descriptions.mechos.16` | `text/mech16.avi` |
| 31 | MECH32 | `descriptions.mechos.17` | `text/mech17.avi` |
| 32 | MECH33 | `descriptions.mechos.17` | `text/mech17.avi` |
| 33 | MECH34 | `descriptions.mechos.17` | `text/mech17.avi` |
| 34 | MECH17 | `descriptions.mechos.17` | `text/mech17.avi` |
| 35 | MECH36 | `descriptions.mechos.35` | `text/mech35.avi` |
| 36 | MECH37 | `descriptions.mechos.35` | `text/mech35.avi` |
| 37 | MECH38 | `descriptions.mechos.35` | `text/mech35.avi` |
| 38 | MECH35 | `descriptions.mechos.35` | `text/mech35.avi` |

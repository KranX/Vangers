#!/usr/bin/env python3
"""Render real TTF via unhinted FreeType; compare with normalized source heights.

The height field is a flat proxy, NOT a screenshot of iscreen's relief lighting.
"""
import argparse
import json
from pathlib import Path

import freetype
import numpy as np
from PIL import Image, ImageDraw, ImageFont

from build_font import CORE_TEXT, RUSSIAN, bearings, read_source

LABEL_FONT = '/usr/share/fonts/noto/NotoSans-Regular.ttf'
BG = (19, 22, 25)
FG = (237, 233, 217)
MUTED = (161, 171, 176)


def paste_max(canvas, tile, x, y):
    x, y = round(x), round(y)
    x0, y0 = max(0, x), max(0, y)
    x1, y1 = min(canvas.shape[1], x + tile.shape[1]), min(canvas.shape[0], y + tile.shape[0])
    if x1 > x0 and y1 > y0:
        canvas[y0:y1, x0:x1] = np.maximum(canvas[y0:y1, x0:x1], tile[y0-y:y1-y, x0-x:x1-x])


def source_line(source, text, spacing, pad=6):
    glyphs, width = [], 0
    for char in text:
        code = char.encode('cp866')[0]
        field = source['glyphs'][code]
        left, right = bearings(field, source['kind'], code)
        glyphs.append((field, left, width))
        width += source['width'] - left - right + spacing
    canvas = np.zeros((source['height'] + pad * 2, int(width) + pad * 2))
    for field, left, x in glyphs:
        paste_max(canvas, field, x + pad - left, pad)
    return canvas


def font_line(face, text, ppem, baseline, sy, pad=6):
    face.set_pixel_sizes(0, ppem)
    flags = freetype.FT_LOAD_RENDER | freetype.FT_LOAD_NO_HINTING | freetype.FT_LOAD_NO_AUTOHINT
    glyphs, width = [], 0
    for char in text:
        face.load_char(char, flags)
        g = face.glyph
        b = g.bitmap
        if b.rows and b.width:
            tile = np.array(b.buffer, dtype=np.uint8).reshape(b.rows, abs(b.pitch))[:, :b.width] / 255
            glyphs.append((tile, width + g.bitmap_left, baseline / sy * ppem - g.bitmap_top))
        width += g.advance.x / 64
    canvas = np.zeros((ppem + pad * 2, int(np.ceil(width)) + pad * 2))
    for tile, x, y in glyphs:
        paste_max(canvas, tile, pad + x, pad + y)
    return canvas


def colored(array, foreground=FG):
    a = np.clip(array, 0, 1)[..., None]
    return Image.fromarray(np.uint8(np.array(BG) * (1-a) + np.array(foreground) * a))


def aligned(a, b):
    shape = tuple(max(x, y) for x, y in zip(a.shape, b.shape))
    aa, bb = np.zeros(shape), np.zeros(shape)
    aa[:a.shape[0], :a.shape[1]] = a
    bb[:b.shape[0], :b.shape[1]] = b
    return aa, bb


def compare_metrics(source, face, config):
    result = {}
    sy = source['height']
    for char in CORE_TEXT:
        a, b = aligned(source_line(source, char, config['spacing']),
                       font_line(face, char, sy, config['baseline'], sy))
        union = (a >= .5) | (b >= .5)
        mask = (a > 0) | (b > 0)
        result[char] = dict(iou=float(np.sum((a >= .5) & (b >= .5)) / max(1, union.sum())),
                            mae=float(np.abs(a-b)[mask].mean()) if mask.any() else 0,
                            ink_ratio=float(b.sum() / max(1, a.sum())))
    values = [v for c,v in result.items() if c != ' ']
    return dict(mean_iou=float(np.mean([v['iou'] for v in values])),
                mean_mae=float(np.mean([v['mae'] for v in values])),
                mean_ink_ratio=float(np.mean([v['ink_ratio'] for v in values])),
                per_glyph=result)


def proof(source_path, font_path, config_path, output):
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)
    source = read_source(source_path)
    face = freetype.Face(str(font_path))
    config = json.loads(Path(config_path).read_text())
    sy, baseline = source['height'], config['baseline']
    label = ImageFont.truetype(LABEL_FONT, 16)
    title = ImageFont.truetype(LABEL_FONT, 25)
    sheet = Image.new('RGB', (1440, 1430), BG)
    d = ImageDraw.Draw(sheet)
    d.text((24, 16), f"{config['id']}  /  {Path(source_path).name} > outline TTF", font=title, fill=FG)
    d.text((24, 54), 'Reference = normalized height map, NOT relief. TTF = actual FreeType rendering, hinting OFF.', font=label, fill=MUTED)
    y = 88
    for text in ['НОВАЯ ИГРА   ПРОДОЛЖИТЬ', 'Съешь ещё этих мягких булок',
                 'VANGERS  New Game  Quick brown fox', '0123456789  AaRWQg  ЖДЙЩФЁёыь']:
        zoom = config.get('proof_scale', 1)
        d.text((24, y + 9), f'SRC {sy}', font=label, fill=MUTED)
        d.text((24, y + 75), f'TTF {sy}', font=label, fill=MUTED)
        for arr, offset in [(source_line(source, text, config['spacing']),0),
                            (font_line(face, text, sy, baseline, sy),66)]:
            tile=colored(arr)
            sheet.paste(tile.resize((tile.width*zoom,tile.height*zoom),Image.Resampling.NEAREST), (112,y+offset))
        y += 140
    zoom = 4 if sy >= 30 else 8
    d.text((24, y), f'Enlarged {zoom}x: source pixels / real outline @ {sy*zoom} ppem', font=label, fill=MUTED)
    text = 'AaWgДЙЩЁё'
    a = source_line(source, text, config['spacing'], pad=2)
    b = font_line(face, text, sy*zoom, baseline, sy, pad=2*zoom)
    sheet.paste(colored(a).resize((a.shape[1]*zoom, a.shape[0]*zoom), Image.Resampling.NEAREST), (24,y+32))
    sheet.paste(colored(b), (24,y+258))
    y += 500
    ppem = 25 if sy >= 30 else 14
    d.text((24, y), f'Small size {ppem} ppem (vector, not downsampled screenshot)', font=label, fill=MUTED)
    sheet.paste(colored(font_line(face, 'Новая игра   Ёжик в тумане   New Game   0123456789', ppem, baseline, sy)), (24, y+27))
    y += 90
    a,b = aligned(source_line(source, 'AMQR  ЖДЙЩЁё', config['spacing']), font_line(face, 'AMQR  ЖДЙЩЁё', sy, baseline, sy))
    overlay = np.zeros((*a.shape,3), dtype=np.uint8)
    overlay[:,:,0] = np.uint8(a*240)
    overlay[:,:,1] = np.uint8(b*240)
    overlay[:,:,2] = np.uint8(np.minimum(a,b)*220)
    d.text((24, y), 'Native overlay: red = source, green = TTF, light = overlap', font=label, fill=MUTED)
    sheet.paste(Image.fromarray(overlay).resize((a.shape[1]*2,a.shape[0]*2),Image.Resampling.NEAREST), (24,y+26))
    sheet.save(output / 'proof.png')

    chars = CORE_TEXT[1:]
    atlas = Image.new('RGB', (1440, 1100), BG)
    d = ImageDraw.Draw(atlas)
    d.text((20, 10), config['id'] + '  /  all 160 visible source glyphs — SRC left, TTF right', font=title, fill=FG)
    for i,c in enumerate(chars):
        x,y = 10 + (i%16)*89, 60+(i//16)*100
        d.text((x,y), c + '  ' + f'{ord(c):04X}', font=label, fill=MUTED)
        a=source_line(source,c,config['spacing'],pad=0)
        b=font_line(face,c,sy,baseline,sy,pad=0)
        atlas.paste(colored(a), (x,y+27))
        atlas.paste(colored(b,(188,215,226)), (x+42,y+27))
    atlas.save(output/'atlas.png')
    metrics = compare_metrics(source,face,config)
    metrics['measurement'] = f'{sy} ppem, no hinting; source normalized heights/ranges are an ink proxy, not measured alpha coverage'
    metrics['freetype_version'] = freetype.version()
    (output/'metrics.json').write_text(json.dumps(metrics,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps({k:v for k,v in metrics.items() if k!='per_glyph'}))


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source',required=True,type=Path)
    p.add_argument('--font',required=True,type=Path)
    p.add_argument('--config',required=True,type=Path)
    p.add_argument('--output',required=True,type=Path)
    args=p.parse_args()
    proof(args.source,args.font,args.config,args.output)

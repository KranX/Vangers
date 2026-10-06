#!/usr/bin/env python3
"""Build an experimental outline TTF from explicitly supplied game artwork.

Original assets are read-only and not bundled. See README for artwork rights.
Potrace constructs initial curves; iteration settings control optical refinement.
"""
import argparse
import hashlib
import json
import struct
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path

import numpy as np
from PIL import Image
from scipy.ndimage import gaussian_filter, map_coordinates
from fontTools.fontBuilder import FontBuilder
from fontTools.pens.ttGlyphPen import TTGlyphPen
from fontTools.pens.cu2quPen import Cu2QuPen
from fontTools.pens.transformPen import TransformPen
from fontTools.svgLib.path import parse_path

UPM = 2000
EPOCH = 3874089600  # Fixed OpenType timestamp, reproducible builds.
RUSSIAN = ''.join(chr(c) for c in range(0x410, 0x450)) + 'Ёё'
CORE_TEXT = ''.join(chr(c) for c in range(32, 127)) + RUSSIAN


def read_source(path):
    """Return native ink/height field, not a screenshot of a relief effect."""
    path = Path(path)
    raw = path.read_bytes()
    glyphs = {}
    if path.suffix == '.bml':
        sx, sy, count = struct.unpack_from('<3h', raw)
        assert len(raw) == 6 + sx * sy * count
        values = np.frombuffer(raw[6:], np.uint8).reshape(count, sy, sx)
        for code, value in enumerate(values):
            glyphs[code] = value.astype(float) / 31
        kind = 'aci-range'
    else:
        assert raw[:9] == b'HFNT 1.01'
        sx, sy, start, end = struct.unpack_from('<4h', raw, 9)
        pos = 17
        for code in range(start, end + 1):
            width, height, flags = struct.unpack_from('<hhi', raw, pos)
            pos += 8
            field = np.zeros((height, width), dtype=float)
            if code and not flags & 1:
                values = np.frombuffer(raw[pos:pos + width * height], np.uint8)
                pos += width * height
                base, maximum = (128, 31) if path.suffix == '.fnh' else (0, 30)
                field = (values.reshape(height, width).astype(float) - base) / maximum
            glyphs[code] = field
        assert pos == len(raw)
        kind = 'height-map' if path.suffix == '.fnh' else 'acs-range'
    assert all(np.min(a) >= 0 and np.max(a) <= 1 for a in glyphs.values())
    return dict(width=sx, height=sy, glyphs=glyphs, kind=kind,
                sha256=hashlib.sha256(raw).hexdigest(), path=str(path))


def bearings(field, kind, code):
    height, width = field.shape
    occupied = np.where(np.any(field > 0, axis=0))[0]
    if kind == 'aci-range':
        limit = 1 if code == 32 and width < 8 else 3
    else:
        # Blank source rows cap both offsets in HChar::calcOffsets.
        limit = width // 3
    if not len(occupied):
        return limit, limit
    return min(limit, int(occupied[0])), min(limit, width - 1 - int(occupied[-1]))


def name_for(codepoint):
    return 'uni%04X' % codepoint if codepoint <= 0xFFFF else 'u%06X' % codepoint


def trace_glyph(field, left, settings, work, glyph_name, sy, baseline, guide_field=None):
    config = dict(settings)
    char = chr(int(glyph_name[3:], 16)) if glyph_name.startswith('uni') else ''
    config.update(settings.get('overrides', {}).get(char, {}))
    pad = 4
    scale = config['supersample']
    padded = np.pad(field, pad)
    if config.get('blur', 0):
        padded = gaussian_filter(padded, config['blur'])
    if scale != 1:
        mode = Image.Resampling.BICUBIC if config.get('resample') == 'bicubic' else Image.Resampling.BILINEAR
        padded = np.asarray(Image.fromarray(padded.astype('float32')).resize(
            (padded.shape[1] * scale, padded.shape[0] * scale), mode))
    if guide_field is not None and np.any(field) and np.any(guide_field):
        # Independent higher-resolution member of the original cfont family.
        # Register each axis by intensity moments; keep target metrics untouched.
        def moments(a):
            y, x = np.indices(a.shape)
            total = a.sum()
            mean = np.array([(y*a).sum(), (x*a).sum()]) / total
            std = np.sqrt(np.array([((y-mean[0])**2*a).sum(),
                                    ((x-mean[1])**2*a).sum()]) / total)
            return mean, np.maximum(std, .1)
        mean, std = moments(field)
        gmean, gstd = moments(guide_field)
        yy, xx = np.indices(padded.shape, dtype=float)
        coords = np.array([(yy+.5)/scale-pad-.5, (xx+.5)/scale-pad-.5])
        coords = (coords - mean[:,None,None]) * (gstd/std)[:,None,None] + gmean[:,None,None]
        detail = np.clip(map_coordinates(guide_field, coords, order=3, mode='constant'), 0, 1)
        blend = config.get('guide_blend', .85)
        padded = (1-blend)*padded + blend*detail
    threshold = config['threshold']
    if config.get('match_ink', False) and np.any(field):
        # Match normalized source ink per glyph, rather than uniformly embolden.
        # Height/range values are a reconstruction proxy, not physical coverage.
        target = field.sum() * scale * scale * config.get('ink_factor', 1.0)
        low, high = config.get('threshold_bounds', [0.28, 0.65])
        for _ in range(24):
            threshold = (low + high) / 2
            if (padded >= threshold).sum() > target:
                low = threshold
            else:
                high = threshold
    mask = padded >= threshold
    if not np.any(mask):
        return TTGlyphPen(None).glyph()
    pbm = work / (glyph_name + '.pbm')
    svg = work / (glyph_name + '.svg')
    # PBM black = ink. Preserve all components, including dots and accents.
    pbm.write_bytes(f'P4\n{mask.shape[1]} {mask.shape[0]}\n'.encode()
                    + np.packbits(mask, axis=1).tobytes())
    subprocess.run(['potrace', str(pbm), '--svg', '--output', str(svg),
                    '--turdsize', str(config.get('turdsize', 0)),
                    '--alphamax', str(config['alphamax']),
                    '--opttolerance', str(config['opttolerance']),
                    '--unit', '100'], check=True, capture_output=True)
    # Potrace raw path uses bottom-up coordinates, 100 units per traced pixel.
    unit = settings.get('upm', UPM) / sy
    xy_scale = unit / (scale * 100)
    transform = (xy_scale, 0, 0, xy_scale,
                 (-pad - left + config.get('dx', 0)) * unit,
                 (baseline - sy - pad + config.get('dy', 0)) * unit)
    pen = TTGlyphPen(None)
    converter = Cu2QuPen(pen, max_err=0.6, reverse_direction=config.get('truetype_winding', False))
    svg_pen = TransformPen(converter, transform)
    for path in ET.parse(svg).getroot().iter('{http://www.w3.org/2000/svg}path'):
        parse_path(path.attrib['d'], svg_pen)
    return pen.glyph()


def notdef_glyph():
    p = TTGlyphPen(None)
    p.moveTo((80, 0)); p.lineTo((80, 1200)); p.lineTo((680, 1200)); p.lineTo((680, 0)); p.closePath()
    p.moveTo((160, 80)); p.lineTo((600, 80)); p.lineTo((600, 1120)); p.lineTo((160, 1120)); p.closePath()
    return p.glyph()


def build(source, config, output, work):
    output = Path(output)
    work = Path(work)
    if output.exists():
        raise FileExistsError(f'Refusing to overwrite reviewed iteration: {output}')
    work.mkdir(parents=True, exist_ok=True)
    source = read_source(source)
    guide = read_source(config['guide_source']) if config.get('guide_source') else None
    sy = source['height']
    baseline = config.get('baseline', sy - 6)
    upm = config.get('upm', UPM)
    unit = upm / sy
    glyphs = {'.notdef': notdef_glyph()}
    widths = {'.notdef': 760}
    cmap = {}
    records = {}
    for char in CORE_TEXT:
        byte = char.encode('cp866')[0]
        field = source['glyphs'][byte]
        if char != ' ' and not np.any(field):
            raise ValueError(f'Core character missing in source: {char}')
        name = name_for(ord(char))
        left, right = bearings(field, source['kind'], byte)
        widths[name] = round((source['width'] - left - right + config['spacing']) * unit)
        glyphs[name] = trace_glyph(field, left, config, work, name, sy, baseline,
                                  guide['glyphs'][byte] if guide else None)
        cmap[ord(char)] = name
        records[char] = dict(source_byte=byte, left=left, right=right, advance=widths[name])
    # Non-breaking space is a real empty glyph with the same advance.
    cmap[0xA0] = cmap[32]
    # The historical '$' slot is a game currency pictogram, not a dollar sign.
    # Expose it explicitly for future text importers without altering legacy parity.
    cmap[0xE000] = cmap[36]
    order = list(glyphs)
    fb = FontBuilder(upm, isTTF=True)
    fb.setupGlyphOrder(order)
    fb.setupCharacterMap(cmap)
    fb.setupGlyf(glyphs)
    metrics = {name: (widths[name], getattr(glyph, 'xMin', 0)) for name, glyph in glyphs.items()}
    fb.setupHorizontalMetrics(metrics)
    ascent, descent = round(baseline * unit), round((baseline - sy) * unit)
    fb.setupHorizontalHeader(ascent=ascent, descent=descent, lineGap=0)
    family = config.get('family', 'Vangers Display Study') + ' ' + config['id']
    fb.setupNameTable(dict(
        familyName=family, styleName='Regular', fullName=family + ' Regular',
        psName=family.replace(' ', '') + '-Regular',
        uniqueFontIdentifier='VangersFontLab:' + config['id'] + ':' + source['sha256'][:12],
        version='Version 0.' + config['id'].split('-')[0].lstrip('0'),
        copyright='Experimental outlines from Vangers game artwork; original artwork rights remain with their owners.',
        description='Latin and Russian Cyrillic reconstruction; game currency icon occupies legacy dollar slot and U+E000.',
        licenseDescription='No new license for original game artwork is granted. See the font-lab README.'))
    xheight = glyphs[name_for(ord('x'))].yMax if config.get('measured_vertical_metrics') else round(18*unit)
    capheight = glyphs[name_for(ord('H'))].yMax if config.get('measured_vertical_metrics') else round(32*unit)
    fb.setupOS2(version=4, sTypoAscender=ascent, sTypoDescender=descent, sTypoLineGap=0,
                usWinAscent=max(ascent, max(getattr(g, 'yMax', 0) for g in glyphs.values())),
                usWinDescent=max(-descent, -min(getattr(g, 'yMin', 0) for g in glyphs.values())),
                sxHeight=xheight, sCapHeight=capheight,
                usWeightClass=400, usWidthClass=5, fsSelection=0x40 | 0x80,
                usFirstCharIndex=min(cmap), usLastCharIndex=max(cmap))
    fb.setupPost(keepGlyphNames=True)
    fb.setupMaxp()
    from fontTools.ttLib import newTable
    fb.font['gasp'] = newTable('gasp')
    fb.font['gasp'].gaspRange = {65535: 0x000A}
    fb.font['head'].created = EPOCH
    fb.font['head'].modified = EPOCH
    fb.font.recalcTimestamp = False
    output.parent.mkdir(parents=True, exist_ok=True)
    fb.save(output)
    metadata = dict(settings=config, source={k:v for k,v in source.items() if k != 'glyphs'},
                    units_per_em=upm, baseline=baseline, core_characters=CORE_TEXT,
                    cmap={str(cp): name for cp,name in cmap.items()}, glyphs=records,
                    ttf_sha256=hashlib.sha256(output.read_bytes()).hexdigest())
    if guide:
        metadata['guide_source'] = {k:v for k,v in guide.items() if k != 'glyphs'}
    output.with_suffix('.json').write_text(json.dumps(metadata, ensure_ascii=False, indent=2) + '\n')
    print(json.dumps(dict(font=str(output), mapped=len(cmap), glyphs=len(order), bytes=output.stat().st_size)))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', required=True, type=Path)
    parser.add_argument('--config', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--work', required=True, type=Path)
    args = parser.parse_args()
    build(args.source, json.loads(args.config.read_text()), args.output, args.work)

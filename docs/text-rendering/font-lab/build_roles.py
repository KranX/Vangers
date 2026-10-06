#!/usr/bin/env python3
"""Build static role TTFs, calibrating placement against each ORIGINAL raster.

Corrections are baked into glyf, never applied by an in-game jitter renderer.
The original per-character unevenness is the target, not a straight baseline.
"""
import argparse
import hashlib
import io
import json
from pathlib import Path

import freetype
import numpy as np
from fontTools.ttLib import TTFont

from build_font import CORE_TEXT, EPOCH, build, read_source
from render_proof import aligned, font_line, source_line


def centroid(a):
    yy, xx = np.indices(a.shape)
    return np.array([(xx*a).sum(), (yy*a).sum()]) / a.sum()


def face_from_font(font):
    buffer = io.BytesIO()
    font.save(buffer)
    return freetype.Face.from_bytes(buffer.getvalue())


def build_role(config_path, output, scratch):
    config_path, output, scratch = map(Path, (config_path, output, scratch))
    if output.exists():
        raise FileExistsError(output)
    scratch.mkdir(parents=True, exist_ok=False)
    config = json.loads(config_path.read_text())
    source = read_source(config['source'])
    seed = scratch/'seed.ttf'
    build(config['source'], config, seed, scratch/'traces')
    font = TTFont(seed, recalcTimestamp=False)
    cmap = font.getBestCmap()
    sy = source['height']
    unit = config['upm']/sy
    reference = {c: source_line(source, c, config['spacing']) for c in CORE_TEXT[1:]}
    corrections = {c: [0, 0] for c in CORE_TEXT[1:]}
    method = config.get('placement_method', 'centroid')
    if method == 'coverage-fit':
        face = face_from_font(font)
        for char in CORE_TEXT[1:]:
            def score(dx, dy):
                face.set_transform(freetype.Matrix(65536,0,0,65536),freetype.Vector(dx,dy))
                a,b = aligned(reference[char], font_line(face,char,sy,config['baseline'],sy))
                return float(np.square(a-b).sum())
            best = (score(0,0),0,0)
            # Small fixed authoring corrections only. Never align different
            # letters to a shared bottom or invent random line-time offsets.
            for dy in range(-16,17,4):
                for dx in range(-16,17,4):
                    loss = score(dx,dy)
                    if loss < best[0]-1e-8:
                        best=(loss,dx,dy)
            shift=np.rint(np.array(best[1:])*unit/64).astype(int)
            g=font['glyf'][cmap[ord(char)]]
            g.coordinates.translate(tuple(shift))
            g.recalcBounds(font['glyf'])
            advance,_=font['hmtx'][cmap[ord(char)]]
            font['hmtx'][cmap[ord(char)]]=advance,g.xMin
            corrections[char]=shift.tolist()
    # Retained to reproduce the rejected centroid-only study, not the default
    # release: optimizing a single statistic worsened pixel agreement.
    for _ in range(3 if method == 'centroid' else 0):
        face = face_from_font(font)
        for char in CORE_TEXT[1:]:
            raster = font_line(face, char, sy, config['baseline'], sy)
            a, b = aligned(reference[char], raster)
            delta = centroid(a)-centroid(b)
            delta[1] *= -1  # bitmap y-down -> outline y-up
            shift = np.rint(delta*unit).astype(int)
            if np.any(np.abs(shift) > unit*.4):
                raise ValueError(('Unexpectedly large placement correction',char,shift.tolist()))
            g = font['glyf'][cmap[ord(char)]]
            g.coordinates.translate(tuple(shift))
            g.recalcBounds(font['glyf'])
            advance, _ = font['hmtx'][cmap[ord(char)]]
            font['hmtx'][cmap[ord(char)]] = advance, g.xMin
            corrections[char] = [a+int(b) for a,b in zip(corrections[char], shift)]
    for name in font.getGlyphOrder():
        font['glyf'][name].recalcBounds(font['glyf'])
    font['OS/2'].sxHeight = font['glyf'][cmap[ord('x')]].yMax
    font['OS/2'].sCapHeight = font['glyf'][cmap[ord('H')]].yMax
    font['OS/2'].usWinAscent = max(font['hhea'].ascent,
        max(getattr(font['glyf'][name], 'yMax', 0) for name in font.getGlyphOrder()))
    font['OS/2'].usWinDescent = max(-font['hhea'].descent,
        -min(getattr(font['glyf'][name], 'yMin', 0) for name in font.getGlyphOrder()))
    family = config['release_family']
    version = config.get('release_version','0.8')
    # Preserve the historical nameID 3 typo when reproducing studies 08/09.
    unique_version = config.get('unique_version', '0.8' if version in ('0.8','0.9') else version)
    names = {1: family, 2: 'Regular', 3: family.replace(' ','')+':'+unique_version+':'+source['sha256'][:12],
             4: family+' Regular', 5: 'Version '+config.get('release_version','0.8'), 6: family.replace(' ','')+'-Regular',
             16: family, 17: 'Regular'}
    for name_id, value in names.items():
        for platform, encoding, language in [(3,1,0x409),(1,0,0)]:
            font['name'].setName(value,name_id,platform,encoding,language)
    font['head'].created = font['head'].modified = EPOCH
    output.parent.mkdir(parents=True, exist_ok=True)
    font.save(output)
    metadata = json.loads(seed.with_suffix('.json').read_text())
    metadata.update(role=config['role'], release_family=family,
        ttf_sha256=hashlib.sha256(output.read_bytes()).hexdigest(),
        corrections_font_units=corrections,
        correction_method=method)
    output.with_suffix('.json').write_text(json.dumps(metadata,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps({'font':str(output),'sha256':metadata['ttf_sha256']}))


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--config',required=True,type=Path)
    p.add_argument('--output',required=True,type=Path)
    p.add_argument('--scratch',required=True,type=Path)
    a=p.parse_args()
    build_role(a.config,a.output,a.scratch)

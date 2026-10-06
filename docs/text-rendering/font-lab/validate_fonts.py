#!/usr/bin/env python3
"""Validate final studies and byte-identical rebuilds. Never modify game assets."""
import argparse
import hashlib
import io
import json
import subprocess
from pathlib import Path

import freetype
from fontTools.pens.areaPen import AreaPen
from fontTools.ttLib import TTFont

from build_font import CORE_TEXT, bearings, build, read_source


def validate(lab, scratch):
    lab, scratch = Path(lab), Path(scratch)
    scratch.mkdir(parents=True, exist_ok=False)
    reports = []
    for prefix, source_path in [('', 'data/resource/iscreen/fonts/hfont00.fnh'),
                                ('dialogue-', 'data/resource/actint/fonts/cfont00.bml')]:
        name = prefix + '07-validated'
        path = lab / 'iterations' / name / 'font.ttf'
        config = json.loads((lab/'iterations'/f'{name}.json').read_text())
        source = read_source(source_path)
        font = TTFont(path, checkChecksums=2)
        cmap = font.getBestCmap()
        expected = set(map(ord, CORE_TEXT)) | {0xA0, 0xE000}
        assert set(cmap) == expected
        assert cmap[0xA0] == cmap[32] and cmap[0xE000] == cmap[36]
        assert not set(font.keys()) & {'EBDT','EBLC','CBDT','CBLC','sbix','SVG '}
        assert {'glyf','loca','cmap','hmtx','hhea','maxp','OS/2','name'} <= set(font.keys())
        assert font['OS/2'].sTypoAscender-font['OS/2'].sTypoDescender == font['head'].unitsPerEm
        assert font['hhea'].ascent == font['OS/2'].sTypoAscender
        assert font['hhea'].descent == font['OS/2'].sTypoDescender
        assert font['OS/2'].usWinAscent >= font['head'].yMax
        assert font['OS/2'].usWinDescent >= -font['head'].yMin
        assert font['OS/2'].sxHeight == font['glyf'][cmap[ord('x')]].yMax
        assert font['OS/2'].sCapHeight == font['glyf'][cmap[ord('H')]].yMax
        gs = font.getGlyphSet()
        points = 0
        for char in CORE_TEXT:
            glyph = font['glyf'][cmap[ord(char)]]
            byte = char.encode('cp866')[0]
            left,right = bearings(source['glyphs'][byte],source['kind'],byte)
            expected_advance = (source['width']-left-right+config['spacing']) * config['upm']/source['height']
            advance,lsb = font['hmtx'][cmap[ord(char)]]
            assert advance == expected_advance, (char,advance,expected_advance)
            if char == ' ':
                assert glyph.numberOfContours == 0
                continue
            assert glyph.numberOfContours > 0, char
            assert lsb == glyph.xMin
            coords,ends,flags = glyph.getCoordinates(font['glyf'])
            assert list(ends) == sorted(set(ends)) and ends[-1] == len(coords)-1
            assert all(-32768 <= v <= 32767 for xy in coords for v in xy)
            assert len(glyph.program.getBytecode()) == 0
            pen = AreaPen(gs)
            gs[cmap[ord(char)]].draw(pen)
            assert pen.value < 0, ('TrueType net winding',char,pen.value)
            points += len(coords)
        assert points > len(CORE_TEXT)*4
        # Compile/decompile every table, not just lazy-load the directory.
        buffer = io.BytesIO()
        font.save(buffer)
        copy = TTFont(io.BytesIO(buffer.getvalue()))
        for tag in copy.keys():
            if tag != 'GlyphOrder':
                copy[tag]
        face = freetype.Face(str(path))
        samples = 0
        for ppem in [14,18,25,50,100,200]:
            face.set_pixel_sizes(0,ppem)
            for char in CORE_TEXT:
                assert face.get_char_index(ord(char)) != 0
                face.load_char(char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_NO_HINTING | freetype.FT_LOAD_NO_AUTOHINT)
                if char != ' ':
                    assert any(face.glyph.bitmap.buffer), (char,ppem)
                samples += 1
        for char in 'äßҐї日':
            assert face.get_char_index(ord(char)) == 0, ('Do not claim unsupported coverage',char)
        rebuilt = scratch/name/'font.ttf'
        build(source_path,config,rebuilt,scratch/name/'traces')
        assert path.read_bytes() == rebuilt.read_bytes(), ('Non-reproducible build',name)
        scan = subprocess.run(['fc-scan','--format','%{family}\n%{outline}\n%{scalable}\n',str(path)],
                              check=True,capture_output=True,text=True).stdout
        assert scan.splitlines()[1:] == ['True','True']
        reports.append(dict(iteration=name,status='PASS',mapped_characters=len(cmap),
                            glyphs=font['maxp'].numGlyphs,points=points,rendered_samples=samples,
                            sizes=[14,18,25,50,100,200],exact_native_advances=True,
                            clockwise_net_winding=True,bitmap_tables=False,byte_identical_rebuild=True,
                            source_sha256=source['sha256'],font_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                            fc_scan=scan.splitlines()))
    result=dict(status='PASS',freetype_version=freetype.version(),fonts=reports)
    (scratch/'validation.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps(result,ensure_ascii=False,indent=2))


if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--lab',type=Path,default=Path(__file__).parent)
    parser.add_argument('--scratch',required=True,type=Path,help='A new directory outside the source tree')
    args=parser.parse_args()
    validate(args.lab,args.scratch)

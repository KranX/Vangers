#!/usr/bin/env python3
"""Validate three role fonts independently against originals; rebuild all three."""
import argparse,hashlib,io,json,subprocess
from pathlib import Path
import freetype,numpy as np
from fontTools.ttLib import TTFont
from fontTools.pens.areaPen import AreaPen
from build_font import CORE_TEXT,bearings,read_source
from build_roles import build_role
from build_menu import build_menu,read_menu,menu_bearings
from role_proof import corpus,wrap,ref_line
from render_proof import font_line,aligned


def validate(lab,scratch):
    lab=Path(lab);folder=lab/'roles-v10';scratch=Path(scratch);scratch.mkdir(parents=True,exist_ok=False)
    reports=[]
    for role in ['menu','display','text']:
        path=folder/f'Vangers{role.title()}-Regular.ttf';config_path=folder/(role+'.json')
        c=json.loads(config_path.read_text());meta=json.loads(path.with_suffix('.json').read_text())
        source=read_menu(c['source'],c['height_knee']) if role=='menu' else read_source(c['source'])
        sy=source['height'];font=TTFont(path,checkChecksums=2,recalcTimestamp=False);cmap=font.getBestCmap();upm=font['head'].unitsPerEm
        assert set(cmap)==set(map(ord,CORE_TEXT))|{0xA0,0xE000}
        assert cmap[0xA0]==cmap[32] and cmap[0xE000]==cmap[36]
        assert not set(font.keys())&{'EBDT','EBLC','CBDT','CBLC','sbix','SVG ','fvar','gvar'}
        assert font['hhea'].ascent-font['hhea'].descent==upm
        assert font['OS/2'].usWinAscent>=font['head'].yMax and font['OS/2'].usWinDescent>=-font['head'].yMin
        assert font['hhea'].ascent==font['OS/2'].sTypoAscender
        assert font['hhea'].descent==font['OS/2'].sTypoDescender
        assert font['OS/2'].sxHeight==font['glyf'][cmap[ord('x')]].yMax
        assert font['OS/2'].sCapHeight==font['glyf'][cmap[ord('H')]].yMax
        assert font['name'].getDebugName(1)=='Vangers '+role.title()
        assert font['name'].getDebugName(5)=='Version 0.10'
        gs=font.getGlyphSet();source_advances={};native_advances={};points=0
        for char in CORE_TEXT:
            name=cmap[ord(char)];g=font['glyf'][name];advance,lsb=font['hmtx'][name]
            native_advances[char]=advance*sy/upm
            assert advance==meta['glyphs'][char]['advance']
            if meta['glyphs'][char].get('original',True):
                byte=char.encode('cp866')[0]
                left,right=menu_bearings(source,byte) if role=='menu' else bearings(source['glyphs'][byte],source['kind'],byte)
                source_advances[char]=source['width']-left-right+c['spacing']
                assert native_advances[char]==source_advances[char],(role,char,'advance')
            if char==' ':assert g.numberOfContours==0;continue
            assert g.numberOfContours>0,(role,char)
            xy,ends,flags=g.getCoordinates(font['glyf']);assert list(ends)==sorted(set(ends)) and ends[-1]==len(xy)-1
            assert all(-32768<=v<=32767 for pair in xy for v in pair)
            assert lsb==g.xMin and not g.program.getBytecode()
            area=AreaPen(gs);gs[name].draw(area);assert area.value<0,(role,char,'winding')
            points+=len(xy)
        if role=='menu':
            assert meta['original_visible_count']==58 and meta['designed_visible_count']==102
            # Do not fake case support by mapping lowercase to uppercase.
            for char in 'abcdefghijklmnopqrstuvwxyzабвгдеёжзийклмнопрстуфхцчшщъыьэюя':
                assert cmap[ord(char)]!=cmap[ord(char.upper())]
        buf=io.BytesIO();font.save(buf);copy=TTFont(io.BytesIO(buf.getvalue()))
        for tag in copy.keys():
            if tag!='GlyphOrder':copy[tag]
        face=freetype.Face(str(path));samples=0
        for ppem in [14,18,25,50,100,200]:
            face.set_pixel_sizes(0,ppem)
            for char in CORE_TEXT:
                assert face.get_char_index(ord(char))
                face.load_char(char,freetype.FT_LOAD_RENDER|freetype.FT_LOAD_NO_HINTING|freetype.FT_LOAD_NO_AUTOHINT)
                if char!=' ':assert any(face.glyph.bitmap.buffer),(role,char,ppem)
                samples+=1
        for char in 'äßҐї日':assert not face.get_char_index(ord(char))
        # Check every real game phrase with independent source and TTF measures.
        real=[]
        for r in corpus():
            if r['role']!=role:continue
            src_width=lambda s:sum(source_advances[ch] for ch in s)
            ttf_width=lambda s:sum(native_advances[ch] for ch in s)
            width=440 if role=='text' else 960
            assert wrap(r['text'],width,src_width)==wrap(r['text'],width,ttf_width)
            assert src_width(r['text'])==ttf_width(r['text'])
            real.append(dict(source=r['path'],line=r['line'],native_width=ttf_width(r['text']),line_count=len(wrap(r['text'],width,ttf_width))))
        # Static glyf outlines and repeated glyph renders are deterministic.
        for char in 'eNkИЁё':
            a=font_line(face,char*3,sy,c['baseline'],sy)
            b=font_line(face,char*3,sy,c['baseline'],sy)
            assert np.array_equal(a,b)
        # Measure actual native placement error, not a promise to flatten glyphs.
        deltas=[]
        for char in source_advances:
            if char==' ':continue
            aa,bb=aligned(ref_line(source,char,c['spacing'],meta),font_line(face,char,sy,c['baseline'],sy))
            yy,xx=np.indices(aa.shape)
            deltas.append(abs(float((yy*aa).sum()/aa.sum()-(yy*bb).sum()/bb.sum())))
        assert max(deltas)<.75,(role,'unexpected vertical displacement',max(deltas))
        if role=='text':
            def bottom(char):
                a=font_line(face,char,18,15,18,pad=0);return int(np.where(a>0)[0].max())
            assert bottom('e')<bottom('c')<bottom('k'),('original unevenness lost',bottom('e'),bottom('c'),bottom('k'))
        rebuilt=scratch/(role+'.ttf')
        (build_menu if role=='menu' else build_role)(config_path,rebuilt,scratch/(role+'-work'))
        assert path.read_bytes()==rebuilt.read_bytes(),('not reproducible',role)
        scan=subprocess.run(['fc-scan','--format','%{family}\n%{outline}\n%{scalable}\n',str(path)],capture_output=True,text=True,check=True).stdout.splitlines()
        assert scan[1:]==['True','True']
        reports.append(dict(role=role,status='PASS',sha256=hashlib.sha256(path.read_bytes()).hexdigest(),mapped=len(cmap),glyphs=font['maxp'].numGlyphs,points=points,render_checks=samples,ppem=[14,18,25,50,100,200],outline_only=True,clockwise_net_winding=True,exact_native_advances=True,byte_identical_rebuild=True,original_vertical_centroid_error_px=dict(mean=float(np.mean(deltas)),max=max(deltas)),real_game_phrases=real,fc_scan=scan))
    result=dict(status='PASS',date='2026-10-06',freetype_version=freetype.version(),fonts=reports,render_checks=sum(r['render_checks'] for r in reports),game_phrases=sum(len(r['real_game_phrases']) for r in reports),limitations=['Native height/footprint proxies are not physical alpha or in-game relief parity.','Small-size nonempty renders do not prove readability.','No shaping/in-game integration; kerning/hinting and extended alphabets not supplied.'])
    (scratch/'validation.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps(result,ensure_ascii=False,indent=2))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--lab',type=Path,default=Path(__file__).parent);p.add_argument('--scratch',type=Path,required=True)
    a=p.parse_args();validate(a.lab,a.scratch)

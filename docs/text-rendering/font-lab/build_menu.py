#!/usr/bin/env python3
"""Menu-specific source reader and builder. Original hfont01 remains read-only."""
import argparse,hashlib,json,struct
from pathlib import Path
import numpy as np
from fontTools.fontBuilder import FontBuilder
from fontTools.ttLib import newTable
from build_font import CORE_TEXT,EPOCH,bearings,name_for,notdef_glyph,read_source,trace_glyph
from menu_extensions import additions


def read_menu(path='data/resource/iscreen/fonts/hfont01.fnh', height_knee=35):
    path=Path(path);raw=path.read_bytes();assert raw[:9]==b'HFNT 1.01'
    sx,sy,start,end=struct.unpack_from('<4h',raw,9);pos=17;fields={};occupied={};raw_fields={}
    for code in range(start,end+1):
        w,h,flags=struct.unpack_from('<hhi',raw,pos);pos+=8
        values=np.full((h,w),128,dtype=np.uint8)
        if code and not flags&1:
            values=np.frombuffer(raw[pos:pos+w*h],np.uint8).reshape(h,w);pos+=w*h
        raw_fields[code]=values
        # This source ranges 127..163, not 128..159. Negative one-level
        # depressions affect original bearings but are NOT positive ink.
        fields[code]=np.clip((values.astype(float)-128)/height_knee,0,1)
        occupied[code]=(values!=128).astype(float)
    assert pos==len(raw)
    return dict(width=sx,height=sy,glyphs=fields,bearing_fields=occupied,raw_fields=raw_fields,
                kind='height-map',sha256=hashlib.sha256(raw).hexdigest(),path=str(path),
                normalization=f'clip((height-128)/{height_knee},0,1); footprint proxy, NOT alpha; bearings use height != 128')


def menu_bearings(source,code):
    return bearings(source['bearing_fields'][code],source['kind'],code)


def build_menu(config_path,output,scratch):
    config=json.loads(Path(config_path).read_text());output=Path(output);scratch=Path(scratch)
    if output.exists():raise FileExistsError(output)
    scratch.mkdir(parents=True,exist_ok=False)
    source=read_menu(config['source'],config.get('height_knee',35));other=read_source('data/resource/iscreen/fonts/hfont00.fnh')
    ext,provenance=additions(source,config,other)
    upm=config['upm'];unit=upm/source['height'];baseline=config['baseline']
    glyphs={'.notdef':notdef_glyph()};metrics={'.notdef':(760,80)};cmap={};records={}
    for char in CORE_TEXT:
        code=char.encode('cp866')[0];name=name_for(ord(char))
        field=source['glyphs'][code];original=bool(np.any(field)) or char==' '
        if original:
            left,right=menu_bearings(source,code);advance=round((45-left-right+config['spacing'])*unit)
            origin='original hfont01' if char!=' ' else 'original blank advance'
        else:
            field=ext[char]
            cols=np.where(np.any(field>.02,axis=0))[0]
            left=int(cols[0])-2;right=45-int(cols[-1])-1-2
            advance=round((45-left-right)*unit)
            origin=provenance[char]
        settings=dict(config)
        if not original:
            settings.update(blur=config.get('extension_blur',.15),supersample=12,match_ink=True,ink_factor=1,threshold_bounds=[.25,.75])
        glyph=trace_glyph(field,left,settings,scratch,name,50,baseline)
        glyphs[name]=glyph;metrics[name]=(advance,getattr(glyph,'xMin',0));cmap[ord(char)]=name
        records[char]=dict(source_byte=code,original=original,provenance=origin,left=left,right=right,advance=advance)
    cmap[0xA0]=cmap[32];cmap[0xE000]=cmap[36]
    fb=FontBuilder(upm,isTTF=True);fb.setupGlyphOrder(list(glyphs));fb.setupCharacterMap(cmap);fb.setupGlyf(glyphs)
    if config.get('correct_lsb',False):
        metrics={n:(metrics[n][0],getattr(g,'xMin',0)) for n,g in glyphs.items()}
    fb.setupHorizontalMetrics(metrics)
    ascent=round(baseline*unit);descent=ascent-upm
    fb.setupHorizontalHeader(ascent=ascent,descent=descent,lineGap=0)
    fb.setupNameTable(dict(familyName='Vangers Menu',styleName='Regular',fullName='Vangers Menu Regular',
      psName='VangersMenu-Regular',uniqueFontIdentifier='VangersMenu:'+config['id']+':'+source['sha256'][:12],
      version='Version '+config['release_version'],
      copyright='Experimental derivative of Vangers artwork; original artwork rights remain with their owners.',
      description='Original hfont01 capitals; newly designed lowercase, numbers, punctuation and dieresis. Legacy dollar is beebs icon.',
      licenseDescription='No new license for original artwork is granted. See font-lab README.'))
    fb.setupOS2(version=4,sTypoAscender=ascent,sTypoDescender=descent,sTypoLineGap=0,
      usWinAscent=max(ascent,max(getattr(g,'yMax',0) for g in glyphs.values())),
      usWinDescent=max(-descent,-min(getattr(g,'yMin',0) for g in glyphs.values())),
      sxHeight=glyphs[cmap[ord('x')]].yMax,sCapHeight=glyphs[cmap[ord('H')]].yMax,
      usWeightClass=400,usWidthClass=5,fsSelection=0x40|0x80)
    fb.setupPost(keepGlyphNames=True);fb.setupMaxp();fb.font['gasp']=newTable('gasp');fb.font['gasp'].gaspRange={65535:0xA}
    fb.font['head'].created=fb.font['head'].modified=EPOCH;fb.font.recalcTimestamp=False
    output.parent.mkdir(parents=True,exist_ok=True);fb.save(output)
    meta=dict(settings=config,source={k:v for k,v in source.items() if k not in ['glyphs','bearing_fields','raw_fields']},
        extension_source_sha256=other['sha256'],glyphs=records,units_per_em=upm,baseline=baseline,
        original_visible_count=sum(r['original'] for c,r in records.items() if c!=' '),
        designed_visible_count=sum(not r['original'] for r in records.values()),
        ttf_sha256=hashlib.sha256(output.read_bytes()).hexdigest())
    output.with_suffix('.json').write_text(json.dumps(meta,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps(dict(font=str(output),original=meta['original_visible_count'],designed=meta['designed_visible_count'],sha256=meta['ttf_sha256'])))


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    for arg in ['config','output','scratch']:p.add_argument('--'+arg,required=True,type=Path)
    a=p.parse_args();build_menu(a.config,a.output,a.scratch)

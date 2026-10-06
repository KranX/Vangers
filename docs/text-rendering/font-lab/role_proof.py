#!/usr/bin/env python3
"""Actual FreeType proofs, with source-only comparisons kept separate from additions."""
import argparse,hashlib,json,re
from pathlib import Path
import freetype
import numpy as np
from PIL import Image,ImageDraw,ImageFont
from build_font import CORE_TEXT,read_source
from build_menu import read_menu
from render_proof import BG,FG,MUTED,LABEL_FONT,colored,font_line,source_line,aligned,paste_max

LAB=Path(__file__).parent

def corpus():
    p=Path('data/iscreen/scripts/strings.inc');raw=p.read_bytes();lines=raw.decode('cp866').splitlines();result=[]
    for role,keys in [('menu',['NEW_GAME','LOAD_GAME','OPTIONS']),('display',['Options','Graphics_Options','Credits','MAIN_MENU'])]:
        for key in keys:
            for language,n in [('en',1),('ru',2)]:
                ident='iSTR_'+key+str(n)
                found=[(i+1,re.search(r'"(.*)"',line).group(1)) for i,line in enumerate(lines) if re.match(r'\s*#define\s+'+ident+r'\s+',line)]
                assert len(found)==1,(ident,found)
                line,text=found[0]
                result.append(dict(role=role,language=language,text=text,path=str(p),encoding='cp866',line=line,id=ident,sha256=hashlib.sha256(raw).hexdigest()))
    p=Path('data/data/Podish.text');raw=p.read_bytes();lines=raw.decode('cp1251').splitlines()
    for line,lang in [(30,'ru'),(31,'ru'),(32,'en'),(33,'en')]:
        text=lines[line-1];assert text.startswith('"') and text.endswith('"'),(line,text)
        result.append(dict(role='text',language=lang,text=text[1:-1],path=str(p),encoding='cp1251',line=line,sha256=hashlib.sha256(raw).hexdigest()))
    return result


def ref_line(source,text,spacing,meta,pad=6):
    if 'bearing_fields' not in source:return source_line(source,text,spacing,pad)
    scale=source['height']/meta['units_per_em'];width=sum(meta['glyphs'][c]['advance']*scale for c in text)
    canvas=np.zeros((source['height']+2*pad,int(round(width))+2*pad));x=pad
    for c in text:
        r=meta['glyphs'][c]
        assert r['original'],('No historical source for new glyph',c)
        paste_max(canvas,source['glyphs'][c.encode('cp866')[0]],x-r['left'],pad)
        x+=r['advance']*scale
    return canvas


def wrap(text,max_width,measure):
    lines=[];line=''
    for word in text.split():
        trial=(line+' '+word).strip()
        if line and measure(trial)>max_width:lines.append(line);line=word
        else:line=trial
    if line:lines.append(line)
    return lines


def proof(role,font_path,config_path,output):
    output=Path(output);output.mkdir(parents=True,exist_ok=True)
    config=json.loads(Path(config_path).read_text());meta=json.loads(Path(font_path).with_suffix('.json').read_text())
    source=read_menu(config['source'],config.get('height_knee',35)) if role=='menu' else read_source(config['source'])
    sy=source['height'];face=freetype.Face(str(font_path));baseline=config['baseline']
    label=ImageFont.truetype(LABEL_FONT,17);title=ImageFont.truetype(LABEL_FONT,25)
    rows=[];game=[]
    def measure(s):return sum(meta['glyphs'][c]['advance']*sy/meta['units_per_em'] for c in s)
    for r in corpus():
        if r['role']!=role:continue
        limit=440 if role=='text' else 960
        strings=wrap(r['text'],limit,measure)
        for i,text in enumerate(strings):
            rows.append((r['id'] if 'id' in r else f"{r['path']}:{r['line']}",text))
        game.append({**r,'wrapped_lines':strings,'native_widths':[measure(s) for s in strings]})
    zoom=2 if role=='text' else 1
    rowh=32+2*(sy+12)*zoom+16
    sheet=Image.new('RGB',(1100,110+rowh*len(rows)),BG);d=ImageDraw.Draw(sheet)
    d.text((20,12),f'{role.title()} / {config["id"]} / real game strings',font=title,fill=FG)
    d.text((20,49),'SRC: normalized source field, not relief. TTF: FreeType, hinting OFF. No game integration.',font=label,fill=MUTED)
    y=95
    for ident,text in rows:
        d.text((20,y),ident,font=label,fill=MUTED);y+=27
        for tag,a in [('SRC',ref_line(source,text,config['spacing'],meta)),('TTF',font_line(face,text,sy,baseline,sy))]:
            tile=colored(a);tile=tile.resize((tile.width*zoom,tile.height*zoom),Image.Resampling.NEAREST)
            assert tile.width<=1000,(role,text,tile.width)
            d.text((20,y+6),tag,font=label,fill=MUTED);sheet.paste(tile,(88,y));y+=(sy+12)*zoom
        y+=21
    sheet.save(output/'game-text.png')
    (output/'game-text.json').write_text(json.dumps(game,ensure_ascii=False,indent=2)+'\n')
    atlas=Image.new('RGB',(1440,2300),BG);d=ImageDraw.Draw(atlas)
    d.text((20,12),f'{role.title()} / left: SRC / right: TTF. NEW = no original glyph.',font=title,fill=FG)
    metrics={}
    for i,c in enumerate(CORE_TEXT[1:]):
        x,y=12+(i%12)*119,60+(i//12)*158
        original=meta['glyphs'][c].get('original',True)
        d.text((x,y),f'{c} {ord(c):04X}'+(' NEW' if not original else ''),font=label,fill=MUTED)
        b=font_line(face,c,sy,baseline,sy,pad=2)
        z=2 if role=='text' else 1
        if original:
            a=ref_line(source,c,config['spacing'],meta,pad=2)
            aa,bb=aligned(a,b);mask=(aa>0)|(bb>0);union=(aa>=.5)|(bb>=.5)
            metrics[c]=dict(mae=float(np.abs(aa-bb)[mask].mean()),iou=float(((aa>=.5)&(bb>=.5)).sum()/max(1,union.sum())),ink_ratio=float(bb.sum()/aa.sum()))
            im=colored(a);atlas.paste(im.resize((im.width*z,im.height*z),Image.Resampling.NEAREST),(x,y+29))
        im=colored(b);atlas.paste(im.resize((im.width*z,im.height*z),Image.Resampling.NEAREST),(x+58,y+29))
    atlas.save(output/'atlas.png')
    # Large outline check and lowercase/numeric extensions, not fake source comparisons.
    samples=['NEW GAME','НОВАЯ ИГРА','AaWgДЩЁё','abcdefghijklmnopqrstuvwxyz','абвгдеёжзийклмнопрстуфхцчшщъыьэюя','0123456789 !?.,:;','Welcome back, poor pilgarlic!','Очнулся, жалкий?']
    ext=Image.new('RGB',(1250,1150),BG);d=ImageDraw.Draw(ext)
    d.text((20,12),'TTF at 50/100 ppem; lowercase/numbers in Menu are newly designed.',font=label,fill=FG);y=50
    for text in samples:
        ppem=100 if len(text)<12 else 50
        a=font_line(face,text,ppem,baseline,sy)
        im=colored(a);assert im.width<1230,(text,im.width)
        ext.paste(im,(20,y));y+=im.height+12
    ext.save(output/'outline.png')
    report=dict(original_compared=len(metrics),mean_mae=float(np.mean([v['mae'] for v in metrics.values()])),mean_iou=float(np.mean([v['iou'] for v in metrics.values()])),per_glyph=metrics)
    (output/'metrics.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='per_glyph'}))

if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--role',required=True,choices=['menu','display','text'])
    for arg in ['font','config','output']:p.add_argument('--'+arg,required=True,type=Path)
    a=p.parse_args();proof(a.role,a.font,a.config,a.output)

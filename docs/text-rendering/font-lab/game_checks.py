#!/usr/bin/env python3
"""Read-only original screenshots + offline TTF comparison. Never fake game integration."""
import argparse,hashlib,json
from pathlib import Path
import freetype,numpy as np
from PIL import Image,ImageDraw,ImageFont
from build_font import read_source
from render_proof import source_line,font_line,aligned,BG,FG,MUTED,LABEL_FONT

FIXTURES=[('en-new-game.png','Welcome back, poor pilgarlic!',122,47),
          ('ru-escave-dialogue.png','Вот так бывает',134,39),
          ('ru-escave.png','Очнулся, жалкий?',136,47)]


def check(font,output):
    output=Path(output);output.mkdir(parents=True,exist_ok=True)
    source=read_source('data/resource/actint/fonts/cfont00.bml');face=freetype.Face(str(font))
    root=Path('docs/text-rendering/baseline/8713913/images');records=[];tiles=[];lut={}
    for filename,text,x,y in FIXTURES:
        p=root/filename;raw=np.array(Image.open(p).convert('RGB'))
        a=source_line(source,text,1,pad=0);h,w=a.shape
        actual=raw[y:y+h,x:x+w]
        mask=(actual[:,:,0]>0)&(actual[:,:,1]>0)&(actual[:,:,2]==0)
        xor=int(np.count_nonzero(mask!=(a>0)));assert xor==0,(filename,xor)
        index=np.rint(a*31).astype(int)
        for k in np.unique(index):
            colors=np.unique(actual[index==k],axis=0)
            assert len(colors)==1,(filename,k,colors.tolist())
            if int(k) in lut:assert lut[int(k)]==colors[0].tolist()
            lut[int(k)]=colors[0].tolist()
        b=font_line(face,text,18,15,18,pad=0)
        assert b.shape == a.shape, ('native cell dimensions differ',filename,a.shape,b.shape)
        aa,bb=aligned(a,b)
        tile=dict(actual=actual,a=a,b=b)
        tiles.append(tile)
        records.append(dict(screenshot=str(p),sha256=hashlib.sha256(p.read_bytes()).hexdigest(),text=text,
              crop=[x,y,w,h],original_source_nonzero_xor=xor,
              ttf_native_advance=b.shape[1],source_native_advance=a.shape[1],
              source_vs_ttf_mae=float(np.abs(aa-bb)[(aa>0)|(bb>0)].mean())))
    # Palette comes only from the matched screenshots. Interpolate unused levels,
    # explicitly as offline preview, not the original game's renderer.
    keys=sorted(lut);palette=np.stack([np.interp(np.arange(32),keys,[lut[k][c] for k in keys]) for c in range(3)],axis=1)
    label=ImageFont.truetype(LABEL_FONT,18);sheet=Image.new('RGB',(1040,710),BG);d=ImageDraw.Draw(sheet)
    d.text((20,15),'Original game crop / offline TTF with sampled palette (pixels x3)',font=label,fill=FG)
    d.text((20,43),'Not an in-game TTF screenshot. No renderer change. The source mask matches all 3 crops exactly.',font=label,fill=MUTED)
    y=88
    for record,t in zip(records,tiles):
        d.text((20,y),Path(record['screenshot']).name+' — '+record['text'],font=label,fill=FG);y+=32
        reconstructed=np.uint8(palette[np.clip(np.rint(t['b']*31).astype(int),0,31)])
        for name,tile in [('Game',t['actual']),('TTF',reconstructed)]:
            im=Image.fromarray(tile);sheet.paste(im.resize((im.width*3,im.height*3),Image.Resampling.NEAREST),(100,y))
            d.text((20,y+13),name,font=label,fill=MUTED);y+=65
        y+=30
    sheet.save(output/'game-crops.png')
    result=dict(status='PASS',note='Original cfont00 mask check; TTF preview is offline, no in-game integration.',fixtures=records,palette_observed_levels=keys,palette={str(k):v for k,v in lut.items()})
    (output/'game-crops.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps(result,ensure_ascii=False))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--font',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();check(a.font,a.output)

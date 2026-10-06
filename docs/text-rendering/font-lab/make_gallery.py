#!/usr/bin/env python3
"""Compact comparisons and final readable atlases; renders the saved TTF files."""
import json
from pathlib import Path

import freetype
from PIL import Image, ImageDraw, ImageFont

from build_font import CORE_TEXT, read_source
from render_proof import BG, FG, MUTED, LABEL_FONT, colored, font_line, source_line


def gallery(lab):
    label=ImageFont.truetype(LABEL_FONT,14)
    for prefix,source_path in [('', 'data/resource/iscreen/fonts/hfont00.fnh'),
                                ('dialogue-','data/resource/actint/fonts/cfont00.bml')]:
        source=read_source(source_path)
        sy=source['height']
        for config_path in sorted((lab/'iterations').glob(prefix+'[0-9][0-9]-*.json')):
            config=json.loads(config_path.read_text())
            folder=config_path.with_suffix('')
            face=freetype.Face(str(folder/'font.ttf'))
            sheet=Image.new('RGB',(720,420),BG)
            d=ImageDraw.Draw(sheet)
            native_zoom=1 if sy==50 else 2
            large_zoom=2 if sy==50 else 6
            text='НОВАЯ ИГРА   New Game'
            a=source_line(source,text,config['spacing'],pad=0)
            b=font_line(face,text,sy,config['baseline'],sy,pad=0)
            c=source_line(source,'AaWgДЩЁё',config['spacing'],pad=0)
            e=font_line(face,'AaWgДЩЁё',sy*large_zoom,config['baseline'],sy,pad=0)
            for caption,arr,y,zoom in [('Растр',a,12,native_zoom),('TTF',b,77,native_zoom),
                                       ('Растр',c,174,large_zoom),('TTF',e,294,1)]:
                d.text((10,y+14),caption,font=label,fill=MUTED)
                tile=colored(arr)
                if zoom!=1:
                    tile=tile.resize((tile.width*zoom,tile.height*zoom),Image.Resampling.NEAREST)
                assert tile.width <= 634, (config_path,tile.width)
                sheet.paste(tile,(84,y))
            d.text((10,141),f'Сверху: {sy} ppem'+(' (пиксели ×2)' if native_zoom==2 else '')+
                   f'   /   Снизу: исходные пиксели ×{large_zoom}, TTF {sy*large_zoom} ppem',font=label,fill=MUTED)
            sheet.save(folder/'comparison.webp',quality=78,method=6)
            if config['id'] != '07-validated':
                continue
            atlas=Image.new('RGB',(1440,1180),BG)
            d=ImageDraw.Draw(atlas)
            d.text((16,12),'Все 160 видимых глифов: слева исходник, справа настоящий TTF; одинаковый цвет.',font=label,fill=FG)
            zoom=1 if sy==50 else 3
            for i,char in enumerate(CORE_TEXT[1:]):
                x,y=10+(i%16)*89,50+(i//16)*110
                d.text((x,y),f'{char} {ord(char):04X}',font=label,fill=MUTED)
                for tile,dx in [(source_line(source,char,config['spacing'],pad=0),0),
                                (font_line(face,char,sy,config['baseline'],sy,pad=0),42)]:
                    im=colored(tile)
                    atlas.paste(im.resize((im.width*zoom,im.height*zoom),Image.Resampling.NEAREST),(x+dx,y+26))
            atlas.save(folder/'atlas-detail.png')


if __name__=='__main__':
    gallery(Path(__file__).parent)

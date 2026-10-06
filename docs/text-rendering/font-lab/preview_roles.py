#!/usr/bin/env python3
"""Self-contained browser specimen of the exact validated TTF files."""
import base64,json
from pathlib import Path
import freetype,numpy as np
from PIL import Image
from build_font import read_source
from build_menu import read_menu
from role_proof import ref_line
from render_proof import font_line,colored,BG

lab=Path(__file__).parent.resolve();folder=lab/'roles-v10'
html=(folder/'vangers-fonts.template.html').read_text();css=[]
for role in ['menu','display','text']:
    path=folder/f'Vangers{role.title()}-Regular.ttf'
    css.append("@font-face{font-family:Vangers"+role.title()+";src:url(data:font/ttf;base64,"+base64.b64encode(path.read_bytes()).decode()+") format('truetype');font-weight:400;font-style:normal;font-display:block}")
    c=json.loads((folder/(role+'.json')).read_text());meta=json.loads(path.with_suffix('.json').read_text());face=freetype.Face(str(path))
    text={'menu':'НОВАЯ ИГРА','display':'Настройка графики','text':'Welcome back, poor pilgarlic!'}[role]
    sy=18 if role=='text' else 50
    if role=='text':
        a=Image.open(lab.parent/'baseline/8713913/images/en-new-game.png').crop((122,47,340,65)).convert('RGB')
        b=font_line(face,text,18,15,18,pad=0)
        lut=json.loads((folder/'game-checks/game-crops.json').read_text())['palette']
        palette=np.array([lut[str(i)] for i in range(32)],dtype=np.uint8)
        b=Image.fromarray(palette[np.clip(np.rint(b*31).astype(int),0,31)])
        a=a.resize((a.width*2,a.height*2),Image.Resampling.NEAREST);b=b.resize((b.width*2,b.height*2),Image.Resampling.NEAREST)
    else:
        source=read_menu(c['source'],c['height_knee']) if role=='menu' else read_source(c['source'])
        a=colored(ref_line(source,text,c['spacing'],meta));b=colored(font_line(face,text,sy,c['baseline'],sy))
    canvas=Image.new('RGB',(max(a.width,b.width),a.height+b.height+12),BG)
    canvas.paste(a,(0,0));canvas.paste(b,(0,a.height+12))
    target=folder/(role+'-comparison.png');canvas.save(target)
    html=html.replace('__'+role.upper()+'_PROOF__',str(target))
html=html.replace('__FONT_FACES__','\n'.join(css))
assert '__MENU_PROOF__' not in html and len(html)<512000
(folder/'vangers-fonts.html').write_text(html)
print(json.dumps(dict(html=str(folder/'vangers-fonts.html'),characters=len(html))))

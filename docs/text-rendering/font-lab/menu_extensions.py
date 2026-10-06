"""Authored Menu additions, NOT recovered hfont01 artwork.

Centerline Beziers are an editable design source. Variable-width strokes are
sampled at 12x; the common Potrace/TrueType pipeline constructs smooth outlines.
Caps-derived lowercase shapes and the original ornamental stem are explicit
reuse, not external-font substitution. No system font supplies game glyphs.
"""
import numpy as np
from PIL import Image, ImageDraw
from fontTools.pens.basePen import BasePen
from fontTools.svgLib.path import parse_path

# Native design canvas: 45x50. x-height at y=19, lowercase bottom y=43;
# ascenders at y=5, descenders at y=48. Capitals retain original positions.
PATHS = {
 'a':['M29 22 C12 13 6 25 9 36 C12 48 30 45 29 29 L29 21 L30 43'],
 'b':['M10 5 L10 43','M11 25 C23 12 35 19 34 32 C33 45 19 48 11 39'],
 'd':['M31 5 L30 43','M30 24 C17 12 5 23 9 37 C12 48 24 45 30 37'],
 'e':['M9 31 L32 28 C32 14 9 16 9 30 C8 44 23 48 32 39'],
 'f':['M27 7 C14 2 13 11 13 23 L13 44','M5 21 L25 20'],
 'g':['M30 22 C16 14 7 22 9 34 C12 44 25 40 30 31','M30 21 L29 41 C29 50 14 50 9 45'],
 'h':['M10 5 L10 43','M10 29 C16 13 32 16 31 31 L31 43'],
 'i':['M14 21 L14 43','M14 10 L14 10.1'],
 'j':['M21 21 L21 41 C21 49 12 51 7 45','M21 10 L21 10.1'],
 'k':['M10 5 L10 43','M30 20 L11 34','M19 29 L32 43'],
 'l':['M13 5 L13 37 Q13 45 23 42'],
 'm':['M7 21 L7 43','M7 29 C11 16 21 18 21 30 L21 43','M21 29 C27 16 37 18 37 30 L37 43'],
 'n':['M10 21 L10 43','M10 28 C20 14 32 18 31 32 L31 43'],
 'p':['M10 21 L10 48','M10 25 C21 14 34 19 33 31 C33 43 17 45 10 37'],
 'q':['M31 21 L31 48','M31 24 C16 13 6 23 10 35 C14 47 25 40 31 34'],
 'r':['M10 21 L10 43','M10 28 C17 18 22 18 29 22'],
 't':['M14 10 L14 36 Q14 47 26 41','M6 22 L28 21'],
 'y':['M9 21 L19 39','M31 21 L22 43 Q18 50 10 47'],
 'б':['M30 6 Q15 8 11 16 L9 31 C8 47 32 48 32 32 C32 18 15 17 10 29'],
 'ф':['M21 8 L21 49','M21 22 C8 14 4 23 7 34 C10 45 20 43 21 33 C23 46 36 44 36 31 C36 17 26 17 21 24'],
 '0':['M29 6 C12 -1 4 12 6 29 C6 45 16 49 27 41 C37 33 39 13 29 6 Z'],
 '1':['M8 15 L21 6 L20 43','M10 44 L30 44'],
 '2':['M8 14 C12 0 37 2 34 17 C31 28 17 33 8 44 L36 43'],
 '3':['M8 10 C20 0 37 6 31 19 Q28 25 20 25 C39 21 38 40 26 44 Q16 48 7 40'],
 '4':['M28 6 L8 32 L36 32','M28 6 L27 45'],
 '5':['M33 7 L12 7 L10 25 C30 17 39 30 31 40 C23 49 12 44 7 39'],
 '6':['M32 8 C16 -1 6 13 8 31 C9 46 25 49 32 38 C42 21 19 17 9 31'],
 '7':['M7 7 L36 7 C25 20 21 28 16 44'],
 '8':['M21 24 C1 18 9 1 25 6 C42 11 28 23 21 24 C1 28 4 45 21 45 C42 45 39 27 21 24 Z'],
 '9':['M33 20 C21 33 4 24 9 13 C14 0 34 3 34 18 C34 31 28 47 10 42'],
 '.':['M13 42 L13 42.1'], ',':['M15 40 Q17 44 10 48'],
 ':':['M13 23 L13 23.1','M13 41 L13 41.1'],
 ';':['M13 23 L13 23.1','M15 39 Q17 44 10 48'],
 '!':['M15 7 L14 31','M14 42 L14 42.1'],
 '?':['M7 12 C14 0 34 6 29 18 Q27 24 17 28 L16 33','M16 43 L16 43.1'],
 '"':['M11 7 L10 16','M24 7 L23 16'], "'":['M14 7 L13 16'],
 '`':['M10 7 L18 15'], '-':['M8 28 L29 27'], '_':['M5 47 L36 47'],
 '+':['M6 27 L35 27','M21 12 L21 42'], '=':['M6 21 L34 21','M6 33 L34 33'],
 '/':['M6 46 L32 4'], '\\':['M6 4 L32 46'], '|':['M17 4 L17 47'],
 '(':['M25 4 C7 14 7 36 25 47'], ')':['M10 4 C28 14 28 36 10 47'],
 '[':['M26 5 L12 5 L12 46 L26 46'], ']':['M10 5 L24 5 L24 46 L10 46'],
 '{':['M28 4 Q15 3 18 17 Q20 24 9 25 Q21 27 18 34 Q14 47 28 47'],
 '}':['M9 4 Q22 3 19 17 Q17 24 28 25 Q16 27 19 34 Q23 47 9 47'],
 '<':['M29 12 L9 27 L29 42'], '>':['M9 12 L29 27 L9 42'],
 '^':['M8 18 L21 5 L34 18'], '~':['M5 26 C16 13 25 38 36 23'],
 '*':['M21 9 L21 35','M9 16 L33 29','M9 29 L33 16'],
 '#':['M15 6 L10 44','M29 6 L24 44','M5 19 L36 19','M4 33 L35 33'],
 '%':['M6 45 L36 4','M13 6 C1 4 2 20 12 20 C23 20 23 6 13 6 Z','M30 31 C19 29 18 45 29 45 C40 45 40 31 30 31 Z'],
 '&':['M33 43 C19 30 6 20 11 10 C17 0 32 8 26 16 C21 23 3 26 8 38 C12 52 31 45 36 30'],
 '@':['M29 20 C15 12 10 31 20 34 Q29 35 29 21 L28 33 C39 40 46 18 32 9 C12 -4 -4 16 5 35 C12 50 31 47 38 41'],
}
CAP_DERIVED = {'c':'C','o':'O','s':'S','u':'U','v':'V','w':'W','x':'X','z':'Z'}
# Russian lowercase convention: these letters retain cap skeleton at x-height.
RU_CAPS = 'вгджзийклмнтцчшщъыьэюя'
RU_ALIAS = {'а':'a','е':'e','о':'o','п':'n','р':'p','с':'c','у':'y','х':'x'}


class Samples(BasePen):
    def __init__(self):
        super().__init__(None); self.paths=[]; self.points=[]
    def _moveTo(self,p):
        if self.points:self.paths.append(self.points)
        self.points=[p]
    def _lineTo(self,p):
        a=np.array(self.points[-1]);b=np.array(p)
        self.points.extend(a+(b-a)*t for t in np.linspace(0,1,max(2,int(np.linalg.norm(b-a)*8)))[1:])
    def _curveToOne(self,b,c,d):
        a=np.array(self.points[-1]);b,c,d=map(np.array,(b,c,d))
        self.points.extend((1-t)**3*a+3*(1-t)**2*t*b+3*(1-t)*t*t*c+t**3*d for t in np.linspace(0,1,160)[1:])
    def _qCurveToOne(self,b,c):
        a=np.array(self.points[-1]);b,c=map(np.array,(b,c))
        self.points.extend((1-t)**2*a+2*(1-t)*t*b+t*t*c for t in np.linspace(0,1,100)[1:])
    def _closePath(self):self._lineTo(self.points[0]);self._endPath()
    def _endPath(self):
        if self.points:self.paths.append(self.points);self.points=[]


def draw_paths(paths,weight=3.8,bulbs=.5):
    scale=12
    im=Image.new('L',(45*scale,50*scale));d=ImageDraw.Draw(im)
    for path in paths:
        p=Samples();parse_path(path,p);p._endPath()
        for points in p.paths:
            for i,(x,y) in enumerate(points):
                t=i/max(1,len(points)-1)
                # Broad terminal, narrower middle: modeled on hfont01 ends.
                r=weight/2*(.82+bulbs*(np.exp(-t*12)+np.exp(-(1-t)*12)))
                d.ellipse(((x-r)*scale,(y-r)*scale,(x+r)*scale,(y+r)*scale),fill=255)
    return np.asarray(im.resize((45,50),Image.Resampling.LANCZOS),dtype=float)/255


def resized_ink(a,w,h,x,y):
    ys,xs=np.where(a>.02)
    crop=a[ys.min():ys.max()+1,xs.min():xs.max()+1]
    out=np.zeros((50,45))
    small=np.asarray(Image.fromarray(crop.astype('float32')).resize((w,h),Image.Resampling.BICUBIC))
    out[y:y+h,x:x+w]=np.clip(small,0,1)
    return out


def additions(source,config,other):
    fields={};provenance={}
    caps=source['glyphs'];weight=config.get('extension_weight',3.8)
    for char,paths in PATHS.items():
        w=config.get('digit_weight',weight) if char.isdigit() else weight
        fields[char]=draw_paths(paths,w,config.get('extension_bulbs',.5))
        provenance[char]='authored variable-width Bezier strokes'
    for char,cap in {**CAP_DERIVED,**{c:c.upper() for c in RU_CAPS}}.items():
        field=caps[cap.encode('cp866')[0]]
        ys,xs=np.where(field>.1)
        ratio=(xs.max()-xs.min()+1)/(ys.max()-ys.min()+1)
        h=27;w=max(9,round(h*ratio))
        fields[char]=resized_ink(field,w,h,5,18)
        provenance[char]='derived lowercase from original '+cap+'; new size/placement'
    for c,latin in RU_ALIAS.items():
        fields[c]=fields[latin].copy();provenance[c]='new lowercase using compatible '+latin+' skeleton'
    # Use the original I ornamental stem as a localized vocabulary, not a
    # system font. Fine stems remain readable; ornament intensity is explicit.
    if config.get('ornaments',True):
        stem=caps[ord('I')]
        for char,x,y,h in [('b',6,4,41),('h',6,4,41),('k',6,4,41),('d',26,4,41),
                           ('p',6,18,31),('q',26,18,31),('n',6,19,26),('r',6,19,26),
                           ('m',3,19,26),('f',9,4,41),('l',9,4,41),('i',10,20,25),('j',17,20,25)]:
            ornament=resized_ink(stem,9,h,x,y)
            fields[char]=np.maximum(fields[char],ornament)
        # Aliases must receive the same optical update as their Latin source.
        for c,latin in RU_ALIAS.items():fields[c]=fields[latin].copy()
        if config.get('ornamental_one',False):
            fields['1']=np.maximum(fields['1'],resized_ink(stem,10,40,16,5))
    # Dieresis is a newly designed mark. Compress E vertically so original
    # ornate capital remains inside the historical cell; do not imply parity.
    dots=draw_paths(['M15 4 L15 4.1','M29 4 L29 4.1'],weight*1.1,0.5)
    fields['Ё']=np.maximum(resized_ink(caps['Е'.encode('cp866')[0]],35,38,4,11),dots)
    fields['ё']=np.maximum(fields['е'],draw_paths(['M14 11 L14 11.1','M27 11 L27 11.1'],weight*.85,.5))
    provenance['Ё']='new dieresis + vertically fitted original Е'
    provenance['ё']='new lowercase е + new dieresis'
    # Historical $ is a beebs icon throughout this study, explicitly not USD.
    fields['$']=resized_ink(other['glyphs'][ord('$')],30,35,5,9)
    provenance['$']='game beebs icon derived from hfont00; not a dollar sign'
    return fields,provenance

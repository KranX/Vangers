#!/usr/bin/env python3
"""Add European accents to the game outlines, preserving existing glyphs/metrics.
Requires fontTools. Usage: extend.py SOURCE_DIRECTORY OUTPUT_DIRECTORY
The unchanged three v0.10 faces are the inputs (see fonts/README.md).
"""
import pathlib
import hashlib
import sys
import unicodedata as ud
from copy import deepcopy
from array import array
from fontTools.ttLib.tables._g_l_y_f import GlyphCoordinates
from fontTools.ttLib import TTFont
from fontTools.pens.ttGlyphPen import TTGlyphPen


def polygon(pen, points):
    pen.moveTo(points[0])
    for p in points[1:]: pen.lineTo(p)
    pen.closePath()


def extend(src, dst):
    inputs = {
        'VangersText-Regular.ttf':'5165e59e38703ad60adb1c83d10292c2a6cef0a2f2efecb764c503f60c011cf4',
        'VangersMenu-Regular.ttf':'45e44b66abd41d65bc4529c9361c08c3bbd55fa9784b487dd4373eff9f966b37',
        'VangersDisplay-Regular.ttf':'dd64ebdcb8108b58806f34a9ebeb27f083a633a7a31175ab7945b0c4e0227f49',
    }
    if hashlib.sha256(src.read_bytes()).hexdigest()!=inputs[src.name]:
        raise SystemExit('Use the unmodified v0.10 inputs from integration 337dfbc')
    font = TTFont(src, recalcTimestamp=False)
    cmap = font.getBestCmap()
    original = dict(cmap)
    glyf, metrics = font['glyf'], font['hmtx'].metrics
    em = font['head'].unitsPerEm
    unit = em / (18 if 'Text' in src.name else 50)
    # Font-space geometry, not copied pixels. Match the face's own stroke scale.
    stroke = unit * (0.9 if 'Text' in src.name else 2.0)
    top = font['hhea'].ascent - unit
    bottom = font['hhea'].descent + unit
    # Accented i/j replace the dot, rather than adding a second accent to it.
    dotless = {}; dot_positions = {}
    for letter in 'ij':
        name=original[ord(letter)];g=glyf[name]
        coordinates,ends,flags=g.getCoordinates(glyf)
        contours=[];start=0
        for end in ends:
            indices=list(range(start,end+1));start=end+1
            ys=[coordinates[i][1] for i in indices]
            contours.append((min(ys),max(ys),indices))
        body=max(contours,key=lambda c:len(c[2]))
        kept=[i for lo,hi,indices in contours if lo<=body[1] for i in indices]
        dots=[i for lo,hi,indices in contours if lo>body[1] for i in indices]
        if not dots: raise ValueError('Cannot identify the original i/j dot')
        dot_positions[letter]=((min(coordinates[i][0] for i in dots)+max(coordinates[i][0] for i in dots))/2,
                               (min(coordinates[i][1] for i in dots)+max(coordinates[i][1] for i in dots))/2)
        result=deepcopy(g);result.coordinates=GlyphCoordinates([coordinates[i] for i in kept])
        result.flags=array('B',[flags[i] for i in kept]);result.endPtsOfContours=[];offset=-1
        for lo,hi,indices in contours:
            if lo<=body[1]: offset+=len(indices);result.endPtsOfContours.append(offset)
        result.numberOfContours=len(result.endPtsOfContours)
        result.program.fromBytecode([]);result.recalcBounds(glyf)
        target=f'vgdotless{ord(letter):04X}';glyf[target]=result
        metrics[target]=(metrics[name][0],result.xMin);dotless[letter]=target
    marks = {}
    for mark in ('\u0300','\u0301','\u0302','\u0303','\u0304','\u0306','\u0307','\u0308','\u030a','\u030b','\u030c','\u0327','\u0328','\u0326'):
        pen = TTGlyphPen(None)
        s = stroke
        if mark in '\u0307\u0308':
            for x in ((-1.5*s,1.5*s) if mark=='\u0308' else (0,)):
                polygon(pen, [(x-s/2,0),(x+s/2,0),(x+s/2,s),(x-s/2,s)])
        elif mark in '\u0300\u0301\u030b':
            for off in ((-1.4*s,1.4*s) if mark=='\u030b' else (0,)):
                sign = -1 if mark=='\u0300' else 1
                polygon(pen, [(off-1.1*s*sign,0),(off-.25*s*sign,0),(off+1.1*s*sign,1.6*s),(off+.25*s*sign,1.6*s)])
        elif mark in '\u0302\u030c\u0306':
            points=[(-2*s,0),(-s,0),(0,s),(s,0),(2*s,0),(0,2*s)]
            if mark!='\u0302': points=[(x,2*s-y) for x,y in reversed(points)]
            polygon(pen,points)
        elif mark=='\u030a':
            polygon(pen,[(-s,0),(s,0),(1.5*s,s),(s,2*s),(-s,2*s),(-1.5*s,s)])
            polygon(pen,[(-.4*s,.6*s),(-.6*s,s),(-.4*s,1.4*s),(.4*s,1.4*s),(.6*s,s),(.4*s,.6*s)])
        elif mark=='\u0304': polygon(pen,[(-2*s,0),(2*s,0),(2*s,.7*s),(-2*s,.7*s)])
        elif mark=='\u0303': polygon(pen,[(-2*s,0),(-s,s),(s,0),(2*s,s),(2*s,1.8*s),(s,.8*s),(-s,1.8*s),(-2*s,.8*s)])
        else: polygon(pen,[(-s,0),(0,0),(0,-s),(s,-s),(s,-2*s),(-s,-2*s)])
        name=f'vgmark{ord(mark):04X}'
        glyf[name]=pen.glyph(); metrics[name]=(0,0); marks[mark]=name
    generated = {0x131:dotless['i'],0x237:dotless['j']}
    for cp in list(range(0xC0,0x180))+list(range(0x218,0x21C)):
        if cp in cmap: continue
        parts=ud.normalize('NFD',chr(cp))
        if len(parts)!=2 or ord(parts[0]) not in original or parts[1] not in marks: continue
        accent=parts[1]; above=ud.combining(accent)!=202 and accent not in '\u0326\u0328'
        base=dotless[parts[0]] if above and parts[0] in dotless else original[ord(parts[0])]
        glyph=glyf[base]; glyph.recalcBounds(glyf)
        mark=marks[accent]; mg=glyf[mark]; mg.recalcBounds(glyf)
        # Preserve the fixed UI cell; leave room for the accent where needed.
        sy=1.0
        if above and glyph.yMax+unit+mg.yMax>top:
            sy=(top-unit-mg.yMax)/max(1,glyph.yMax)
        if not above and glyph.yMin-unit+mg.yMin<bottom:
            sy=min(sy,(bottom+unit-mg.yMin)/min(-1,glyph.yMin)) if glyph.yMin<0 else sy
        sy=max(.72,min(1,sy))
        pen=TTGlyphPen(font.getGlyphSet())
        pen.addComponent(base,(1,0,0,sy,0,0))
        y=round(glyph.yMax*sy+unit) if above else round(glyph.yMin*sy-unit)
        x=round((glyph.xMin+glyph.xMax)/2)
        if above and parts[0] in dot_positions:
            dx,dy=dot_positions[parts[0]];x=round(dx);y=round(dy-(mg.yMin+mg.yMax)/2)
        pen.addComponent(mark,(1,0,0,1,x,y))
        name=f'vgext{cp:04X}';glyf[name]=pen.glyph();metrics[name]=metrics[base];generated[cp]=name
    # Non-decomposing European forms retain the same hand-drawn components.
    for cp, pair in {0xC6:'AE',0xE6:'ae',0x152:'OE',0x153:'oe'}.items():
        if cp in cmap: continue
        pen=TTGlyphPen(font.getGlyphSet());x=0
        advance=sum(metrics[original[ord(c)]][0] for c in pair)
        scale=max(metrics[original[ord(c)]][0] for c in pair)/advance
        for c in pair:
            name=original[ord(c)];pen.addComponent(name,(scale,0,0,1,x,0));x+=round(metrics[name][0]*scale)
        name=f'vgext{cp:04X}';glyf[name]=pen.glyph();metrics[name]=(round(advance*scale),0);generated[cp]=name
    for cp,base in {0xDF:'h',0x1E9E:'B'}.items():
        if cp in cmap: continue
        b=original[ord(base)];g=glyf[b];g.recalcBounds(glyf)
        w=metrics[b][0]*.85;h=g.yMax;s=stroke
        pen=TTGlyphPen(None)
        pen.moveTo((0,0));pen.lineTo((0,h*.70))
        pen.qCurveTo((0,h),(w*.45,h));pen.qCurveTo((w*.95,h),(w*.95,h*.72))
        pen.qCurveTo((w*.95,h*.59),(w*.68,h*.52))
        pen.qCurveTo((w,h*.46),(w,h*.24));pen.qCurveTo((w,0),(w*.48,0))
        pen.lineTo((w*.35,0));pen.lineTo((w*.35,s));pen.lineTo((w*.48,s))
        pen.qCurveTo((w-s,s),(w-s,h*.24));pen.qCurveTo((w-s,h*.37),(w*.37,h*.43))
        pen.lineTo((w*.37,h*.57));pen.qCurveTo((w*.95-s,h*.62),(w*.95-s,h*.72))
        pen.qCurveTo((w*.95-s,h-s),(w*.45,h-s));pen.qCurveTo((s,h-s),(s,h*.70))
        pen.lineTo((s,0));pen.closePath()
        name=f'vgext{cp:04X}';glyf[name]=pen.glyph();metrics[name]=(round(w+s),0);generated[cp]=name
    for cp,base in {0xD8:'O',0xF8:'o',0x110:'D',0x111:'d',0x126:'H',0x127:'h',0x141:'L',0x142:'l',0x166:'T',0x167:'t'}.items():
        if cp in cmap: continue
        b=original[ord(base)];g=glyf[b];g.recalcBounds(glyf)
        pen=TTGlyphPen(font.getGlyphSet());pen.addComponent(b,(1,0,0,1,0,0))
        # Composite pen cannot mix contours and components: a separate stroke.
        stroke_pen=TTGlyphPen(None)
        if cp in (0xD8,0xF8):
            polygon(stroke_pen,[(g.xMin,g.yMin),(g.xMin,g.yMin+stroke),(g.xMax,g.yMax),(g.xMax,g.yMax-stroke)])
        else:
            mid=(g.yMin+g.yMax)*.5;left=g.xMin-stroke;right=g.xMax*.7
            rise=(right-left)*.4 if cp in (0x141,0x142) else 0
            polygon(stroke_pen,[(left,mid-stroke),(left,mid),(right,mid+rise),(right,mid+rise-stroke)])
        sn=f'vgstroke{cp:04X}';glyf[sn]=stroke_pen.glyph();metrics[sn]=(0,0);pen.addComponent(sn,(1,0,0,1,0,0))
        name=f'vgext{cp:04X}';glyf[name]=pen.glyph();metrics[name]=metrics[b];generated[cp]=name
    for name in generated.values():
        glyf[name].recalcBounds(glyf)
        metrics[name]=(metrics[name][0],glyf[name].xMin)
    order=font.getGlyphOrder();order+=sorted(set(glyf.glyphs)-set(order));font.setGlyphOrder(order)
    for table in font['cmap'].tables:
        if table.isUnicode(): table.cmap.update(generated)
    font['maxp'].numGlyphs=len(order)
    dst.parent.mkdir(parents=True,exist_ok=True);font.save(dst)
    print(src.name,len(generated),'new characters')

if __name__=='__main__':
    source,output=map(pathlib.Path,sys.argv[1:])
    for path in sorted(source.glob('Vangers*-Regular.ttf')): extend(path,output/path.name)

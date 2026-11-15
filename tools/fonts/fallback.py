#!/usr/bin/env python3
"""Freeze the pinned Noto Sans JP variable face at medium weight; fontTools required.
Usage: fallback.py NotoSansJP-variable.ttf OUTPUT.ttf OFL.txt
The upstream revision and SHA-256 are documented in fonts/README.md.
"""
import pathlib
import hashlib
import sys
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont
expected='c2f3b4d463500a2ddcd3849cded1fceeb9fd6d1c32e6cbecd568453ba50fc68f'
if hashlib.sha256(pathlib.Path(sys.argv[1]).read_bytes()).hexdigest()!=expected:
    raise SystemExit('Not the pinned upstream Noto Sans JP source')
font=TTFont(sys.argv[1],recalcTimestamp=False)
font=instantiateVariableFont(font,{'wght':500},inplace=True,updateFontNames=True)
license=pathlib.Path(sys.argv[3]).read_text()
for record in font['name'].names:
    if record.nameID==13: record.string=license.encode(record.getEncoding(),errors='replace')
font.save(sys.argv[2])

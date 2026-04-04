#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
from pathlib import Path

SECTION_RE = re.compile(r'(?m)^\[[^\n]+$')
QUOTE_RE = re.compile(r'"(.*?)"', re.S)


def split_sections(text: str):
    matches = list(SECTION_RE.finditer(text))
    if not matches:
        return [('', text)]
    sections = []
    preamble = text[:matches[0].start()]
    if preamble.strip():
        sections.append(('', preamble))
    for i, m in enumerate(matches):
        start = m.start()
        end = matches[i + 1].start() if i + 1 < len(matches) else len(text)
        title = m.group(0).strip()
        sections.append((title, text[start:end]))
    return sections


def detect_lang(s: str) -> str:
    cyr = sum('\u0400' <= ch <= '\u04ff' for ch in s)
    asc = sum(ch.isascii() and ch.isalpha() for ch in s)
    return 'RU' if cyr >= asc else 'EN'


def format_section(title: str, body: str) -> str:
    out = []
    if title:
        out.append(title)
        if body.startswith(title):
            body = body[len(title):]

    matches = list(QUOTE_RE.finditer(body))
    if not matches:
        if body.strip():
            out.append(body.rstrip())
        return '\n'.join(out).rstrip() + '\n'

    cursor = 0
    counts = {'RU': 0, 'EN': 0}
    for m in matches:
        prefix = body[cursor:m.start()]
        if prefix.strip():
            out.append(prefix.rstrip())
        quote = m.group(1).rstrip()
        lang = detect_lang(quote)
        counts[lang] += 1
        out.append(f'<<{lang} {counts[lang]}>>')
        out.append(quote)
        cursor = m.end()

    tail = body[cursor:]
    if tail.strip():
        out.append(tail.rstrip())
    return '\n'.join(out).rstrip() + '\n'


def main() -> int:
    ap = argparse.ArgumentParser(description='View dual-language Vangers text/query assets with RU/EN quote tags.')
    ap.add_argument('path', help='Path to original dual-language .text/.query file')
    ap.add_argument('--section', action='append', default=[], help='Only print sections whose title contains this substring')
    ap.add_argument('--encoding', default='cp1251', help='Input encoding (default: cp1251)')
    args = ap.parse_args()

    path = Path(args.path)
    text = path.read_text(encoding=args.encoding)
    filters = [s.lower() for s in args.section]

    printed = False
    for title, body in split_sections(text):
        if filters:
            hay = title.lower()
            if not any(f in hay for f in filters):
                continue
        rendered = format_section(title, body)
        if rendered.strip():
            if printed:
                print()
            print(rendered, end='')
            printed = True

    return 0


if __name__ == '__main__':
    raise SystemExit(main())

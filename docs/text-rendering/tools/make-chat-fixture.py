#!/usr/bin/env python3
"""Navigation-only diagnostic fixture, NOT a multiplayer test.

Use only on data made by prepare-data.sh. Compile both oftr scripts afterwards.
The real Chat screen and rendering/input implementation are not changed.
"""
import argparse
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("sandbox", type=Path)
args = parser.parse_args()
data = args.sandbox.resolve()
if not (data / ".baseline-isolated").is_file():
    parser.error("Requires a new isolated data directory made by prepare-data.sh")
main = data / "iscreen/scripts/main.inc"
old = main.read_bytes()
default = b'default_scr\t"Main Screen"'
if old.count(default) != 1:
    parser.error("Unexpected script or fixture already installed")
menu = (data / "iscreen/scripts/mainmenu.scr").read_bytes()
start = menu.index(b'Object "Chat Option"', menu.index(b'Screen "Server Info screen"'))
end = menu.index(b'Object "HallOfFame Option"', start)
launcher = (
    b'\nScreen "Baseline Chat Launch" {\n screen_offs 0\n'
    b' block_global_obj\n default_obj "Chat Option"\n'
    + menu[start:end].rstrip() + b'\n}\n'
)
scripts = [data / "iscreen" / name for name in ("oftr.scr", "oftr2.scr")]
originals = [p.read_bytes() for p in scripts]
main.write_bytes(old.replace(default, b'default_scr\t"Baseline Chat Launch"'))
for path, original in zip(scripts, originals):
    path.write_bytes(original + launcher)
print("Fixture installed. Return on startup opens the unchanged Chat screen.")
print("No server, player-list or message-delivery verification is provided.")

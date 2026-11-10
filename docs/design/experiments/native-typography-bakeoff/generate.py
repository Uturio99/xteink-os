#!/usr/bin/env python3
import hashlib
import json
import subprocess
import sys
from pathlib import Path

from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[3]
FONTS = ROOT / "lib/EpdFont/builtinFonts"
ROLES = ((8, 400, "regular"), (10, 700, "bold"), (11, 400, "regular"),
         (13, 400, "regular"), (21, 400, "regular"))
SOURCES = {"noto-serif": "NotoSerif", "newsreader": "Newsreader", "literata": "Literata"}
OUTPUTS = ("notoserif_8_regular.h", "notoserif_10_bold.h", "notoserif_11_regular.h",
           "notoserif_13_regular.h", "notoserif_21_regular.h")

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    if len(sys.argv) != 2 or sys.argv[1] not in SOURCES:
        raise SystemExit("usage: generate.py noto-serif|newsreader|literata")
    candidate = sys.argv[1]
    source = HERE / "sources" / f"{SOURCES[candidate]}.ttf"
    if not source.is_file():
        raise SystemExit("Run ./prepare.sh first")
    marker = HERE / "generated-state.json"
    if not marker.is_file():
        raise SystemExit("Run sync_bakeoff.ps1 first; it records the copied font-header hashes")
    state = json.loads(marker.read_text())
    for name, expected in state["last"].items():
        if sha(FONTS / name) != expected:
            raise SystemExit(f"Refusing modified experiment output: {name}")
    for name, expected in state["original"].items():
        if name not in state["last"] and sha(FONTS / name) != expected:
            raise SystemExit(f"Refusing unrecognized WIP: {name}")
    run_manifest = json.loads((HERE / "run-manifest.json").read_text(encoding="utf-8"))
    item = next(c for c in run_manifest["candidates"] if c["name"].lower().replace(" ", "-") == candidate)
    if sha(source) != item.get("source_sha256"):
        raise SystemExit(f"Pinned font source hash mismatch: {source.name}")
    face = TTFont(source)
    axes = {axis.axisTag: axis for axis in face["fvar"].axes}
    axis_info = {tag: [a.minValue, a.defaultValue, a.maxValue] for tag, a in axes.items()}
    outputs = {}
    for size, weight, style in ROLES:
        inst = TTFont(source)
        axes = {axis.axisTag: axis for axis in inst["fvar"].axes}
        coords = {"wght": min(max(weight, axes["wght"].minValue), axes["wght"].maxValue)}
        if "opsz" in axes:
            coords["opsz"] = min(max(size, axes["opsz"].minValue), axes["opsz"].maxValue)
        if "wdth" in axes:
            coords["wdth"] = axes["wdth"].defaultValue
        inst = instantiateVariableFont(inst, coords, inplace=True, optimize=True)
        static = HERE / "sources" / f"{candidate}-{size}-{style}.ttf"
        inst.save(static)
        name = f"notoserif_{size}_{style}"
        output = FONTS / f"{name}.h"
        converter = ROOT / "lib/EpdFont/scripts/fontconvert.py"
        with output.open("w", encoding="utf-8", newline="\n") as out:
            subprocess.run([sys.executable, str(converter), name, str(size), str(static),
                            "--2bit", "--compress", "--pnum", "--zopfli"], check=True, stdout=out)
        outputs[output.name] = sha(output)
    state["last"] = outputs
    state["candidate"] = candidate
    marker.write_text(json.dumps(state, indent=2) + "\n", encoding="utf-8")
    item["observed_axes"] = axis_info
    item["generated_header_sha256"] = outputs
    (HERE / "run-manifest.json").write_text(json.dumps(run_manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Generated {candidate}: {outputs}")

if __name__ == "__main__":
    main()

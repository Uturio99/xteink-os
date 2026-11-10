#!/usr/bin/env bash
set -euo pipefail
HERE="$(cd -- "$(dirname -- "$0")" && pwd)"
PYTHON=/home/uturio/noveos-simulator/.venv/bin/python
mkdir -p "$HERE/sources"
fetch() { local url="$1" out="$2"; test ! -e "$out" || { echo "Refusing existing source: $out" >&2; exit 2; }; curl --fail --location --silent --show-error "$url" -o "$out"; }
fetch 'https://raw.githubusercontent.com/notofonts/latin-greek-cyrillic/c4a321e123e4d4ff315f57f4e0adf294fe3a95be/NotoSerif/googlefonts/variable-ttf/NotoSerif%5Bwdth,wght%5D.ttf' "$HERE/sources/NotoSerif.ttf"
fetch 'https://raw.githubusercontent.com/notofonts/latin-greek-cyrillic/c4a321e123e4d4ff315f57f4e0adf294fe3a95be/OFL.txt' "$HERE/sources/NotoSerif-OFL.txt"
fetch 'https://raw.githubusercontent.com/productiontype/NewsReader/1ece6a8bfe5db1a2b90c76cc1fe5d3b2eed5dcf3/fonts/variable/ttf/Newsreader%5Bopsz,wght%5D.ttf' "$HERE/sources/Newsreader.ttf"
fetch 'https://raw.githubusercontent.com/productiontype/NewsReader/1ece6a8bfe5db1a2b90c76cc1fe5d3b2eed5dcf3/OFL.txt' "$HERE/sources/Newsreader-OFL.txt"
fetch 'https://raw.githubusercontent.com/googlefonts/literata/0c2761b727a1b3a7cffd313c37f0f5163dfc7a63/fonts/variable/Literata%5Bopsz,wght%5D.ttf' "$HERE/sources/Literata.ttf"
fetch 'https://raw.githubusercontent.com/googlefonts/literata/0c2761b727a1b3a7cffd313c37f0f5163dfc7a63/OFL.txt' "$HERE/sources/Literata-OFL.txt"
"$PYTHON" - "$HERE" <<'PY'
import hashlib, json, pathlib, sys
h = pathlib.Path(sys.argv[1])
sources = h / 'sources'
data = json.loads((h / 'manifest.json').read_text(encoding='utf-8'))
for c in data['candidates']:
    base = c['name'].replace(' ', '')
    font_path = sources / f'{base}.ttf'
    c['source_sha256'] = hashlib.sha256(font_path.read_bytes()).hexdigest()
    c['ofl_sha256'] = hashlib.sha256((sources / f'{base}-OFL.txt').read_bytes()).hexdigest()
    from fontTools.ttLib import TTFont
    font = TTFont(font_path)
    c['source_axes'] = {a.axisTag: [a.minValue, a.defaultValue, a.maxValue] for a in font['fvar'].axes}
(h / 'run-manifest.json').write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
PY
printf 'Pinned font sources fetched; hashes recorded in %s\n' "$HERE/run-manifest.json"

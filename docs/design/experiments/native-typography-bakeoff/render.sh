#!/usr/bin/env bash
set -euo pipefail
HERE="$(cd -- "$(dirname -- "$0")" && pwd)"
ROOT="$(git -C "$HERE" rev-parse --show-toplevel)"
PIO=/home/uturio/noveos-simulator/.venv/bin/pio
PYTHON=/home/uturio/noveos-simulator/.venv/bin/python
candidate="${1:?usage: render.sh noto-serif|newsreader|literata}"
case "$candidate" in noto-serif|newsreader|literata) ;; *) exit 2 ;; esac
test ! -e "$HERE/captures/$candidate" || { echo "Refusing existing capture directory: $HERE/captures/$candidate" >&2; exit 2; }
"$PYTHON" "$HERE/apply_scene.py"
"$PYTHON" "$HERE/generate.py" "$candidate"
cmake --build "$ROOT/build/wp1-host" --target VietnameseFontCoverageTest
ctest --test-dir "$ROOT/build/wp1-host" -R VietnameseFontCoverageTest --output-on-failure
"$PYTHON" - "$HERE/run-manifest.json" "$candidate" <<'PY'
import json, pathlib, sys
p = pathlib.Path(sys.argv[1])
m = json.loads(p.read_text(encoding='utf-8'))
c = next(c for c in m['candidates'] if c['name'].lower().replace(' ', '-') == sys.argv[2])
c['native_unicode_test'] = 'PASS (existing VietnameseFontCoverageTest; ctest exit 0)'
p.write_text(json.dumps(m, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
PY
"$PIO" run -e simulator_x4_pro
"$PYTHON" - "$HERE/run-manifest.json" "$candidate" <<'PY'
import json, pathlib, sys
p = pathlib.Path(sys.argv[1])
m = json.loads(p.read_text(encoding='utf-8'))
c = next(c for c in m['candidates'] if c['name'].lower().replace(' ', '-') == sys.argv[2])
c['simulator_build'] = 'PASS (pio run -e simulator_x4_pro; exit 0)'
p.write_text(json.dumps(m, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
PY
"$PYTHON" "$HERE/capture.py" "$candidate"

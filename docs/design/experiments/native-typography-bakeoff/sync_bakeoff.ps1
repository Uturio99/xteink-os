$ErrorActionPreference = 'Stop'
$sourceArtifacts = (Resolve-Path -LiteralPath $PSScriptRoot).Path
$sourceWsl = (& wsl.exe -e wslpath -a $sourceArtifacts).Trim()
if ($LASTEXITCODE -ne 0 -or !$sourceWsl.StartsWith('/mnt/')) { throw 'Cannot resolve experiment artifacts inside WSL.' }
$script = @'
set -euo pipefail
src=/home/uturio/noveos-font-gate-check
dst=/home/uturio/noveos-font-bakeoff
rel=docs/design/experiments/native-typography-bakeoff
host_src=__HOST_SRC__
test -e "$src/.git"
test ! -e "$src/$rel"
test "$(git -C "$src/freeink-sdk" rev-parse HEAD)" = 946e6ac3d7a9d590ba3224428e6183d55ac7917c
grep -Fq '#6730fe5fe576c9d924e44096a9e267d8538d184c' "$src/platformio.ini"
grep -Fq 'loadRecentBooks(coverGridUi ? CoverGridHomeUi::MAX_BOOKS : metrics.homeRecentBooksCount);' "$src/src/activities/home/HomeActivity.cpp"
grep -Fq 'SCREENSHOT_START:' "$src/src/main.cpp"
test -f "$src/lib/EpdFont/scripts/fontconvert.py"
test -f "$src/test/vietnamese_font_coverage/CMakeLists.txt"
test ! -e "$dst"
cp -a "$src" "$dst"
target="$dst/$rel"
test ! -e "$target"
mkdir -p "$(dirname "$target")"
cp -a "$host_src" "$target"
chmod +x "$target/prepare.sh" "$target/render.sh"
test ! -e "$target/generated-state.json"
python3 - "$dst" "$target/generated-state.json" <<'PY'
import hashlib, json, pathlib, sys
root, marker = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
names = ('notoserif_8_regular.h', 'notoserif_10_bold.h', 'notoserif_11_regular.h', 'notoserif_13_regular.h', 'notoserif_21_regular.h')
files = root / 'lib/EpdFont/builtinFonts'
original = {n: hashlib.sha256((files / n).read_bytes()).hexdigest() for n in names}
marker.write_text(json.dumps({'original': original, 'last': {}}, indent=2) + '\n', encoding='utf-8')
PY
python3 - "$target" <<'PY'
from pathlib import Path
import sys
root = Path(sys.argv[1])
for p in root.rglob('*'):
    if p.is_file() and p.suffix.lower() in {'.md', '.json', '.sh', '.ps1', '.py'}:
        p.write_bytes(p.read_bytes().replace(b'\r\n', b'\n').replace(b'\r', b'\n'))
PY
python3 - "$host_src" "$target" <<'PY'
from pathlib import Path
import hashlib, sys
source, target = Path(sys.argv[1]), Path(sys.argv[2])
def files(root):
    return {p.relative_to(root).as_posix(): p for p in root.rglob('*') if p.is_file()}
s, t = files(source), files(target)
if set(t) != set(s) | {'generated-state.json'}:
    raise SystemExit('Unexpected files in isolated experiment overlay')
lines = []
for name, path in sorted(s.items()):
    expected = path.read_bytes().replace(b'\r\n', b'\n').replace(b'\r', b'\n')
    actual = t[name].read_bytes()
    if expected != actual:
        raise SystemExit(f'Overlay hash/content mismatch: {name}')
    lines.append(f'{hashlib.sha256(actual).hexdigest()}  {name}\n')
(target / 'sync-sha256.txt').write_text(''.join(lines), encoding='utf-8', newline='\n')
PY
cd "$target"
sha256sum -c sync-sha256.txt
printf 'Isolated copy ready: %s\n' "$dst"
'@
$script = $script.Replace('__HOST_SRC__', $sourceWsl)
$encoded = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($script))
& wsl.exe -e bash -lc "echo '$encoded' | base64 -d | bash"
if ($LASTEXITCODE -ne 0) { throw "WSL isolated sync failed with exit code $LASTEXITCODE" }

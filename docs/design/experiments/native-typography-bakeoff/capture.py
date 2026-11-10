#!/usr/bin/env python3
import json
import hashlib
import os
import select
import struct
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[3]
PREFIX = b"SCREENSHOT_START:"
SUFFIX = b"SCREENSHOT_END\n"

def read_until(stream, marker, timeout, initial=b""):
    data = bytearray(initial)
    end = time.monotonic() + timeout
    fd = stream.fileno()
    while marker not in data:
        remaining = end - time.monotonic()
        if remaining <= 0 or not select.select([fd], [], [], remaining)[0]:
            raise TimeoutError(f"simulator did not emit {marker!r}")
        chunk = os.read(fd, 1)
        if not chunk:
            raise EOFError("simulator exited before screenshot marker")
        data.extend(chunk)
    return bytes(data)

def read_exact(stream, size, timeout):
    data = bytearray()
    end = time.monotonic() + timeout
    fd = stream.fileno()
    while len(data) < size:
        remaining = end - time.monotonic()
        if remaining <= 0 or not select.select([fd], [], [], remaining)[0]:
            raise TimeoutError("simulator framebuffer read timed out")
        chunk = os.read(fd, size - len(data))
        if not chunk:
            raise EOFError("simulator exited during framebuffer read")
        data.extend(chunk)
    return data

def make_bmp(raw, path):
    width, height = 800, 480
    if len(raw) != width * height // 8:
        raise ValueError(f"unexpected framebuffer length: {len(raw)}")
    out_w, out_h = 480, 800
    stride = ((out_w + 31) // 32) * 4
    pixels = bytearray(stride * out_h)
    for out_y in range(out_h):
        row = out_h - 1 - out_y
        for out_x in range(out_w):
            src_x = width - 1 - out_y
            src_y = out_w - 1 - out_x
            bit = (raw[src_y * (width // 8) + src_x // 8] >> (7 - src_x % 8)) & 1
            if bit:
                pixels[row * stride + out_x // 8] |= 0x80 >> (out_x % 8)
    offset, size = 14 + 40 + 8, 14 + 40 + 8 + len(pixels)
    header = struct.pack("<2sIHHI", b"BM", size, 0, 0, offset)
    dib = struct.pack("<IiiHHIIiiII", 40, out_w, out_h, 1, 1, 0, len(pixels), 2835, 2835, 2, 2)
    palette = bytes((0, 0, 0, 0, 255, 255, 255, 0))
    path.write_bytes(header + dib + palette + pixels)

def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: capture.py candidate-name")
    candidate = sys.argv[1]
    exe = ROOT / ".pio/build/simulator_x4_pro/program"
    out = HERE / "captures" / candidate
    out.mkdir(parents=True, exist_ok=True)
    log = (out / "simulator.log").open("wb")
    proc = subprocess.Popen([str(exe)], cwd=ROOT, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=log)
    try:
        startup = bytearray()
        end = time.monotonic() + 8
        while time.monotonic() < end:
            if select.select([proc.stdout.fileno()], [], [], min(.25, end - time.monotonic()))[0]:
                chunk = os.read(proc.stdout.fileno(), 65536)
                if not chunk:
                    raise EOFError("simulator exited during startup")
                startup.extend(chunk)
        proc.stdin.write(b"CMD:SCREENSHOT\n")
        proc.stdin.flush()
        marker = read_until(proc.stdout, PREFIX, 35, startup)
        line = read_until(proc.stdout, b"\n", 2, marker[marker.rfind(PREFIX) + len(PREFIX):])
        size = int(line.strip())
        if size != 48000:
            raise ValueError(f"expected 48000-byte X4 Pro framebuffer, got {size}")
        raw = read_exact(proc.stdout, size, 10)
        tail = read_until(proc.stdout, SUFFIX, 10)
        (out / "framebuffer-800x480-1bpp.bin").write_bytes(raw)
        make_bmp(raw, out / "home-480x800.bmp")
        source_revision = subprocess.check_output(["git", "-C", str(ROOT), "rev-parse", "HEAD"], text=True).strip()
        simulator_hash = hashlib.sha256(exe.read_bytes()).hexdigest()
        (out / "provenance.json").write_text(json.dumps({"candidate": candidate, "capture": "native GfxRenderer framebuffer via existing CMD:SCREENSHOT", "logical_size": [480, 800], "source_framebuffer": [800, 480, "1bpp"], "bytes": size, "captured_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()), "checkout_revision": source_revision, "simulator": str(exe), "simulator_sha256": simulator_hash, "screen_settle_seconds": 8, "scene": "fixed HomeActivity overlay; fake book paths; no SD fixture"}, indent=2) + "\n")
        manifest_path = HERE / "run-manifest.json"
        if manifest_path.exists():
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            item = next(c for c in manifest["candidates"] if c["name"].lower().replace(" ", "-") == candidate)
            item["screenshot"] = str((out / "home-480x800.bmp").relative_to(HERE))
            item["screenshot_sha256"] = __import__("hashlib").sha256((out / "home-480x800.bmp").read_bytes()).hexdigest()
            item["screenshot_status"] = "captured"
            manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        print(f"Saved {out / 'home-480x800.bmp'}; trailing marker={SUFFIX in tail}")
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=3)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()
        log.close()

if __name__ == "__main__":
    main()

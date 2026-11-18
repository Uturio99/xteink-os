#!/usr/bin/env python3
"""Convert a NoveOS logo PNG into C++ 2-bit grayscale lockup and 1-bit emblem arrays."""

import argparse
from pathlib import Path
from PIL import Image


def load_grayscale_white_bg(path: Path) -> Image.Image:
    im = Image.open(path).convert("RGBA")
    bg = Image.new("RGBA", im.size, (255, 255, 255, 255))
    bg.paste(im, mask=im.split()[3])
    return bg.convert("L")


def ink_bbox(gray: Image.Image, threshold: int = 245, pad: int = 4) -> tuple[int, int, int, int]:
    w, h = gray.size
    mask = gray.point(lambda p: 255 if p < threshold else 0)
    box = mask.getbbox()
    if box is None:
        return (0, 0, w, h)
    x0 = max(0, box[0] - pad)
    y0 = max(0, box[1] - pad)
    x1 = min(w, box[2] + pad)
    y1 = min(h, box[3] + pad)
    return (x0, y0, x1, y1)


def split_emblem_from_lockup(cropped: Image.Image, threshold: int = 240) -> Image.Image:
    """Locate a clean horizontal whitespace gap in the lower half to isolate the emblem."""
    w, h = cropped.size
    px = cropped.load()
    start_y = int(h * 0.45)
    end_y = int(h * 0.90)
    gap_start = None
    best_gap = None
    for y in range(start_y, end_y):
        has_ink = any(px[x, y] < threshold for x in range(w))
        if not has_ink:
            if gap_start is None:
                gap_start = y
        elif gap_start is not None:
            gap_len = y - gap_start
            if gap_len >= 6 and (best_gap is None or gap_len > best_gap[1] - best_gap[0]):
                best_gap = (gap_start, y)
            gap_start = None
    if best_gap is not None and best_gap[0] > h * 0.35:
        emblem = cropped.crop((0, 0, w, best_gap[0]))
        return emblem.crop(ink_bbox(emblem, pad=2))
    return cropped


def quantize_2bpp(lum: int) -> int:
    if lum >= 212:
        return 0
    if lum >= 140:
        return 1
    if lum >= 68:
        return 2
    return 3


def pack_2bpp(gray: Image.Image) -> list[int]:
    pixels = list(gray.getdata())
    packed = []
    for i in range(0, len(pixels), 4):
        byte = 0
        for j in range(4):
            shade = quantize_2bpp(pixels[i + j]) if (i + j) < len(pixels) else 0
            byte |= (shade & 0x03) << (6 - j * 2)
        packed.append(byte)
    return packed


def pack_1bpp_portrait(emblem: Image.Image, size: int, threshold: int = 140) -> list[int]:
    ew, eh = emblem.size
    scale = min((size - 4) / max(1, ew), (size - 4) / max(1, eh))
    nw = max(1, round(ew * scale))
    nh = max(1, round(eh * scale))
    resized = emblem.resize((nw, nh), Image.Resampling.LANCZOS)
    canvas = Image.new("L", (size, size), 255)
    canvas.paste(resized, ((size - nw) // 2, (size - nh) // 2))
    # GfxRenderer::drawImage in Portrait blits rows bottom-to-top along logical X
    rotated = canvas.rotate(90, expand=True)
    pixels = list(rotated.getdata())
    row_bytes = (size + 7) // 8
    packed = []
    for y in range(size):
        for xb in range(row_bytes):
            byte = 0
            for b in range(8):
                x = xb * 8 + b
                if x < size:
                    bit = 1 if pixels[y * size + x] >= threshold else 0
                    byte |= bit << (7 - b)
            packed.append(byte)
    return packed


def format_hex_array(values: list[int], cols: int = 16) -> str:
    lines = []
    for i in range(0, len(values), cols):
        chunk = values[i : i + cols]
        lines.append("    " + ", ".join(f"0x{v:02X}" for v in chunk) + ",")
    if lines:
        lines[-1] = lines[-1].rstrip(",")
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate NoveOsLogo.h from PNG")
    parser.add_argument("input", type=Path, help="Path to source logo PNG")
    parser.add_argument("--width", type=int, default=220, help="Target lockup width in pixels")
    parser.add_argument("--emblem-size", type=int, default=120, help="Target 1bpp emblem square size")
    parser.add_argument(
        "--output",
        type=Path,
        default=Path(__file__).resolve().parents[1] / "src" / "images" / "NoveOsLogo.h",
        help="Output header path",
    )
    args = parser.parse_args()

    gray = load_grayscale_white_bg(args.input)
    cropped = gray.crop(ink_bbox(gray, pad=4))
    cw, ch = cropped.size
    lockup_w = args.width
    lockup_h = max(1, round(lockup_w * ch / cw))
    lockup_img = cropped.resize((lockup_w, lockup_h), Image.Resampling.LANCZOS)
    lockup_2bpp = pack_2bpp(lockup_img)

    emblem_src = split_emblem_from_lockup(cropped)
    emblem_1bpp = pack_1bpp_portrait(emblem_src, args.emblem_size)

    header = f"""#pragma once

#include <cstdint>

namespace NoveOsBranding {{
inline constexpr const char* OS_NAME = "NoveOS";
inline constexpr const char* OS_EYEBROW = "00.1 — NOVEOS SYSTEM";

#if !defined(ARDUINO) || FREEINK_DEVICE_X4PRO
inline constexpr int LOCKUP_WIDTH = {lockup_w};
inline constexpr int LOCKUP_HEIGHT = {lockup_h};

// 2-bit packed (4 pixels per byte, MSB first, shade 0..3) for 4-level grayscale AA boot/sleep screen
alignas(4) inline constexpr uint8_t NOVEOS_LOCKUP_2BPP[{len(lockup_2bpp)}] = {{
{format_hex_array(lockup_2bpp)}
}};
#endif

inline constexpr int LOGO_WIDTH = {args.emblem_size};
inline constexpr int LOGO_HEIGHT = {args.emblem_size};

// 1-bit packed (8 pixels per byte, 1=white/0=black, Portrait-rotated) for legacy drawImage fallback
alignas(4) inline constexpr uint8_t NOVEOS_LOGO_1BPP[{len(emblem_1bpp)}] = {{
{format_hex_array(emblem_1bpp)}
}};
}}  // namespace NoveOsBranding
"""
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(header, encoding="utf-8")
    print(
        f"Generated {args.output}: lockup={lockup_w}x{lockup_h} ({len(lockup_2bpp)}B), "
        f"emblem={args.emblem_size}x{args.emblem_size} ({len(emblem_1bpp)}B)"
    )


if __name__ == "__main__":
    main()

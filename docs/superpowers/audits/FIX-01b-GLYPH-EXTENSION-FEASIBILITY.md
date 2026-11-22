# FIX-01b — Atelier glyph extension feasibility (read-only)

Date: 2026-11-22. Scope: can Cyrillic and Greek be baked into `src/components/AtelierFontData.h` in the Atelier typeface, and at what cost? No firmware edits were made; all scratch work lives in `~/noveos-rec02b/_fix01b/` (WSL).

## Verdict

**Feasible. The generator is fully reproduced — byte-exact on all 15 fonts (11,580 glyphs, 0 mismatches), including metrics and 2-bit bitmaps.** Extension glyphs were generated in scratch only.

## 1. Sources

`AtelierFontData.h` header: "Auto-generated from repo NotoSans/NotoSerif TTFs at 1:1 CSS pixel sizes (2-bit AA)". The repo ships only Regular/Bold/Italic cuts of NotoSans and NotoSerif (`lib/EpdFont/builtinFonts/source/{NotoSans,NotoSerif}/`, same files also in `docs/design/prototype/fonts/`). There is **no Medium/Light TTF in the repo** — and none is needed:

| Atelier face | Source TTF | Weight |
|---|---|---|
| `*Reg`, `*Light` | `NotoSans-Regular.ttf` (v2.015, non-autohinted) | plain |
| `*Bold` | `NotoSans-Bold.ttf` | plain |
| `16Med` | `NotoSans-Regular.ttf` + `FT_Outline_EmboldenXY(18,18)` | synthetic |
| `17Serif` | `NotoSerif-Regular.ttf` | plain |

("Light"/"Med" are names only; Light is regular weight, Med is regular + a fixed embolden.)

## 2. Generator recipe (byte-exact)

- `face.set_pixel_sizes(ph, ph)` — `pixelHeight` = CSS px size.
- Load flags: `FT_LOAD_RENDER` (hinted, normal target) for plain faces; for Med: `FT_LOAD_DEFAULT`, then `FT_Outline_EmboldenXY(&outline, 18, 18)`, then `FT_RENDER_MODE_NORMAL`.
- Glyph record: `left = bitmap_left`, `top = bitmap_top`, `width/height = bitmap.width/rows`, `advFp4 = round(advance_px * 16)` — **plus a constant +2 for 16Med**.
- 2-bit quantization (empirical, unambiguous across the full corpus): gray `<42 → 0`, `42–111 → 1`, `112–187 → 2`, `≥188 → 3`. Packed MSB-first, 4 px/byte, `ceil(w*h/4)` bytes per glyph.
- `offset` = cumulative byte offset into the font's bitmap blob.
- Font metrics: `ascender = ph + 1`, `lineHeight ≈ ph * 4/3` (8→11, 12→16, 16→22, 27→37); all 772 glyphs are codepoints 0x20–0x2250 (Latin + Vietnamese + punctuation/symbols), sorted ascending.

Reproduction script: `~/noveos-rec02b/_fix01b/{parse.py,repro.py,map.py,regen.py,extend.py,sheet.py}` (venv: `freetype-py 2.5.1`, `Pillow 13.0.0`, `fonttools 4.68.0`; Python 3.14).

## 3. Reproduction results

| Font | Glyphs | Mismatches | Bitmap bytes gen=stored |
|---|---|---|---|
| all 15 | 772 each | **0 each** | identical (e.g. 12Reg 12,212; 27Light 53,297; 34Light 82,977) |

16Med initially mismatched against every weight; solved as Regular + `EmboldenXY(18,18)` + `advFp4+2` — exact.

## 4. Extension cost (bitmap + 12 B/glyph table entry)

Ranges: Cyrillic U+0400–045F, U+0490–0491 (98 cps); Greek monotonic U+0386, 0388–038A, 038C, 038E–03A1, 03A3–03CE (69 cps). All present in the source faces (0 missing).

| Font | Cyrillic B | Greek B | Font | Cyrillic B | Greek B |
|---|---|---|---|---|---|
| 8Bold | 2,086 | 1,495 | 14Reg | 3,316 | 2,399 |
| 10Bold | 2,539 | 1,819 | 15Light | 3,665 | 2,634 |
| 11Reg | 2,526 | 1,841 | 16Med | 4,115 | 2,957 |
| 11Bold | 2,724 | 1,968 | 17Serif | 4,847 | 3,311 |
| 12Reg | 2,803 | 2,045 | 20Light | 5,457 | 3,915 |
| 12Bold | 2,974 | 2,152 | 27Light | 8,548 | 6,204 |
| 13Reg | 3,060 | 2,226 | 34Light | 12,686 | 9,188 |
| 13Bold | 3,271 | 2,401 | | | |

- **Totals:** Cyrillic **64,617 B**, Greek **46,555 B**, combined **111,172 B** (+167 table entries per font ×12 B included).
- **Flash estimate:** 5,937,274 + 111,172 ≈ **6,048,446 / 6,553,600 (92.3%)** — fits, ~505 KB headroom. Cyrillic-only would be ~6.00 MB.
- IRAM/RAM impact: none — tables are `inline constexpr` in flash, same as today.

## 5. Integration options for `findAtelierGlyphRaw` (AtelierChrome.cpp:43)

The lookup is a single `std::lower_bound` over `font.glyphs`/`glyphCount`.

- **Option A (recommended): regenerate each font's glyph array as one merged, sorted table** — generator emits the 939 cps (772 + 167) in ascending order into the existing arrays and bumps `glyphCount`. Zero code change; search, coverage checks, and `?`-fallback all keep working unchanged. New cps all sort above 0x03CE > existing max 0x2250? No — Cyrillic/Greek cps (0x386–0x491) sort *below* many existing Latin-ext/Vietnamese cps; sorting the union is still trivial at generation time.
- **Option B: separate extension table + two-range search** — a second sorted array searched when the primary misses. More code (extra pointer/count or a wrapper), no benefit unless extensions must stay optional at build time.
- **Option C: runtime merge** — pointless heap/flash cost; rejected.

Option A is strictly better: the search is range-agnostic, so merged sorted tables are a data-only change inside `AtelierFontData.h` regeneration.

## 6. Risks / notes

- **Hinting sensitivity:** byte-exactness depends on the repo's non-autohinted NotoSans v2.015 cuts. A different TTF build (e.g. ttfautohint'd Medium) changes metrics — do not substitute font files.
- **`fallbackChainCovers`/R4 path:** after extension, Cyrillic/Greek pass `findAtelierGlyphRaw` so the Atelier path is taken naturally; Hebrew/Arabic/CJK still fall through to the SD/built-in fallback. No logic change needed.
- **Greek tonos/precomposed forms** render correctly (see contact sheets); polytonic (U+1F00+) is out of scope, as is Cyrillic beyond U+0491. Ukrainian is fully covered: Є/І/Ї/є/і/ї (U+0404/0406/0407/0454/0456/0457) are inside U+0400–U+045F and Ґ/ґ are U+0490/0491.
- **Contact sheets** for PO review: `\\wsl$\Ubuntu\home\uturio\noveos-rec02b\_fix01b\contact-16Med.png`, `contact-27Light.png` (Latin A/g/R/k/Q/Z beside Cyrillic А/Б/Ж/У/я/Ґґ and Greek Α/Β/Γ/Ω/α/ε/π/ω/Ά/ά, at 4×).
- Downloads during investigation (scratch only): notofonts Medium/SemiBold/unhinted + google/fonts variable NotoSans — none matched 16Med; not needed for generation.

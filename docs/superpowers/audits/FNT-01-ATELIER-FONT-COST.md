# FNT-01 — Atelier font cost and usage

- **Base:** `63c9baea50f7183dc045affe8727551e90d2e425` (`chore/update-local-ignore`)
- **Date:** 2026-11-22
- **Scope:** flash cost and usage of the baked 2-bit Atelier fonts in `src/components/AtelierFontData.h`, measured on the linked `x4pro` ELF. Facts only; no recommendation.
- **Counts note:** the data file contains **15 fonts, not 14** — `kAtelierFont11Bold` exists and is linked, but has no `AtelierChrome::font*()` accessor; it is referenced directly inside `AtelierChrome.cpp` (active bottom-nav label, swipe-delete pill).

## Method

```
~/.platformio/packages/toolchain-xtensa-esp-elf/bin/xtensa-esp-elf-nm \
    --size-sort -S .pio/build/x4pro/firmware.elf | grep -i atelierfont
~/.platformio/packages/toolchain-xtensa-esp-elf/bin/xtensa-esp-elf-objdump \
    -h .pio/build/x4pro/firmware.elf
nm -C .pio/build/simulator/program | grep -ci AtelierFont      # -> 0
nm -C .pio/build/simulator_x4_pro/program | grep -ci AtelierFont # -> 64
grep -rln "AtelierFontData.h" src/                            # -> AtelierChrome.h/.cpp only
```

Build: `pio run -e simulator` re-run OK (artifact was absent); `pio run -e default` **FAILED for an environment reason** (`ModuleNotFoundError: No module named 'SCons.Tool.FortranCommon'`, `_fnt01/pio-default.log:417`) — not a source problem; skipped per brief.

## Placement

- All `kAtelierFont*` symbols land in `.flash.rodata` (section VMA `0x3c2a0120`–`0x3c58a928`; symbols span `0x3c3b81bc`–`0x3c42bd…`), type `V` (weak object). **Nothing in `.dram0.data`/`.dram0.bss`/`.iram0.*`.**
- `AtelierFontData.h` is a single-TU header included only by `AtelierChrome.h`/`AtelierChrome.cpp`, and both sit inside `#if FREEINK_DEVICE_X4PRO` (`AtelierChrome.cpp:3`, `AtelierChrome.h:18`). `nm` confirms **0 AtelierFont symbols in the non-X4 `simulator` binary**; 64 in `simulator_x4_pro`.
- **No RAM copy:** `drawText` reads glyph bitmaps directly via `font.bitmaps + g->offset` (`AtelierChrome.cpp:285`); the only `memcpy`s in the file are string normalization/ellipsis stack buffers (lines 149–351). Fonts are consumed in place from memory-mapped flash.

## Per-font flash cost (x4pro ELF)

Every font carries the same 9,264 B glyph table (772 glyphs × 12 B) plus a 16 B `AtelierFont` struct; only the bitmap array scales.

| Font (accessor) | Bitmap B | Table+struct B | Total B | Used on (non-chrome callers) |
|---|---|---|---|---|
| font8Bold | 6,679 | 9,280 | **15,959** | CoverGridHomeUi (cover author/footer) |
| font10Bold | 9,976 | 9,280 | **19,256** | CatalogScreens, CoverGridHomeUi, UiSliderDialog, TextSettingsPreview, KOReaderSync, ReaderToolbarUi, FrontlightPanel, LibraryList |
| font11Reg | 10,575 | 9,280 | **19,855** | CoverGridHomeUi, UiSliderDialog, ButtonRemap, WebServer, EpubReaderMenu, ReaderToolbarUi, KeyboardEntry |
| font11Bold *(no accessor)* | 11,659 | 9,280 | **20,939** | none (internal: nav active label, delete pill — AtelierChrome.cpp:91,827,881,960) |
| font12Reg | 12,212 | 9,280 | **21,492** | CoverGridHomeUi, WebServer, KOReaderSync, ReaderToolbarUi, ClipSelection |
| font12Bold | 13,372 | 9,280 | **22,652** | CatalogScreens, BaseTheme, ReaderToolbarUi, FrontlightPanel |
| font13Reg | 13,978 | 9,280 | **23,258** | CatalogScreens, CoverGridHomeUi, WebServer, CalibreConnect, WifiSelection, KOReaderSync |
| font13Bold | 15,210 | 9,280 | **24,490** | CatalogScreens, BaseTheme, CoverGridHomeUi, UiSliderDialog, WebServer, WifiSelection |
| font14Reg | 15,825 | 9,280 | **25,105** | CoverGridHomeUi, OpdsServerList, KOReaderSync, Bookmarks, ClippingList, BmpViewer, FileBrowser, LibraryList |
| font15Light | 18,302 | 9,280 | **27,582** | CoverGridHomeUi, OtaUpdate (+ internal list index font AtelierChrome.cpp:872,1297) |
| font16Med | 21,888 | 9,280 | **31,168** | CoverGridHomeUi, UiSliderDialog, OtaUpdate, WebServer, CalibreConnect, WifiSelection, EndOfBook, KOReaderSync, FrontlightPanel |
| font17Serif | 24,893 | 9,280 | **34,173** | CoverGridHomeUi only (cover title) |
| font20Light | 30,747 | 9,280 | **40,027** | CatalogScreens, CoverGridHomeUi, EndOfBook, KOReaderSync, ReaderToolbarUi, EpubReader |
| font27Light | 53,297 | 9,280 | **62,577** | CoverGridHomeUi hero title + status-screen/option-sheet headlines (AtelierChrome.cpp:1102,1198) |
| font34Light | 82,977 | 9,280 | **92,257** | CoverGridHomeUi empty headline, UiSliderDialog readout, status-screen large headline (AtelierChrome.cpp:1100) |
| **Total** | 388,530 | 139,230 | **480,790** | |

## Totals vs flash budget

- Font data total: **480,790 B (≈469.5 KiB)**
- vs used flash `5,935,698 B` → **8.10 %** of what is flashed
- vs partition `6,553,600 B` → **7.34 %** of total
- If all 15 were removed, used flash would drop to ≈5,454,908 B (83.2 % of partition). Free headroom is 617,902 B; the fonts are ~78 % the size of that headroom.
- Top two fonts (`34Light` 92,257 + `27Light` 62,577 = 154,834 B) are **32 %** of the font total.
- Glyph coverage identical across fonts: 772 glyphs, ranges `0x20–0x7E`, `0xA0–0x24F`, `0x300–0x36F`, `0x1EA0–0x1EF9`, `0x2010–0x203A` (Latin + Vietnamese + general punctuation only; no Cyrillic/Greek/CJK — see FIX-01 Gate 2 report).
- `gc-sections` drops nothing: every font is referenced (14 via accessors, `font11Bold` via direct `kAtelierFont11Bold` refs inside AtelierChrome.cpp).

## Surprises

1. 15 fonts, not 14 — `font11Bold` is real and linked (internal use only, 20,939 B).
2. The two display sizes (27/34 px) cost more than the seven smallest fonts combined.
3. `env:default` could not be checked (host SCons module missing); source guards (`#if FREEINK_DEVICE_X4PRO` + `nm` on the non-X4 simulator binary = 0 symbols) already prove AtelierFontData is excluded from non-X4 Pro builds.

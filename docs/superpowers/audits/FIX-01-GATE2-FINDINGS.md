# FIX-01 — Gate 2 findings: Atelier glyph fallback

- **Base:** `63c9baea50f7183dc045affe8727551e90d2e425` (`chore/update-local-ignore`), main checkout `D:\Work\AI\xteink-os` + WSL clone `~/noveos-rec02b`
- **Date:** 2026-11-22 — **Gates 1–2 only; no code changes made.**
- **Gate 1 record:** main checkout @ `63c9baea` on `chore/update-local-ignore`; freeink-sdk `946e6ac3d7a9d590ba3224428e6183d55ac7917c` both checkouts (nested lucide `c81680e0`). Dirty paths: `AGENTS.md`, `docs/EXECUTION_CONTRACT.md`, `docs/PROJECT_STATUS.md`, `docs/design/specs/STEP-09-*.md`, `docs/design/step07b/STEP-07B.1R-DECISIONS.md` modified + 26 untracked entries (all preserved). WSL clone clean except audit/log dirs.
- **Evidence labels:** SOURCE_VERIFIED / SIMULATOR_BEHAVIOR_VERIFIED / NOT TESTED.

## 1. Code-path trace (`src/components/AtelierChrome.cpp`)

Glyph resolution (lines 31–46):

- `findAtelierGlyphRaw` (31–39) binary-searches the sorted glyph table; `cp > 0xFFFF` collapses to the `?` key. Returns `nullptr` on miss.
- `findAtelierGlyph` (41–46): direct hit → en/em-dash (0x2013/0x2014) map to `-` → **any other missing codepoint returns the `?` glyph**. Only fails if `?` itself is absent.
- `fontCoversText` (48–58): uses `findAtelierGlyphRaw` (no `?` fallback). True only if every codepoint (except space/dash) has a real glyph. This is the coverage oracle used by the hook.

Measurement vs drawing:

- `measureNormalized` (60–71) and `drawText` (260–299) both call `findAtelierGlyph` **with** the `?` fallback. A missing glyph contributes the `?` advance to the measured width and draws `?` — measurement and drawing are internally consistent for direct callers; the width is real but the pixels are `?`.
- `measureText` (254–258), `wrapText` (301+), `truncateTailWithEllipsis` (133–151) all go through `measureNormalized`, so wrap/ellipsis positions are computed on `?` advances for uncovered text — no error is signalled at any layer of the direct path.

Hook path (how renderer-routed text already falls back):

- `resolveOverrideFont` (88–100): `SMALL_FONT_ID→11Reg/11Bold`, `UI_10_FONT_ID→12Reg/12Bold`, `UI_12_FONT_ID→16Med/13Bold`.
- `hookDrawText` (118–129): returns `false` when the mapped Atelier font can't cover → `GfxRenderer::drawText` then runs its normal path (`GfxRenderer.cpp:712–717`): `resolveTextFontId` (270–299) substitutes the registered SD **fallback font** if the primary lacks the script, and centers the fallback's line box inside the requested font's line box (`GfxRenderer.cpp:726–729`). `ensureSdGlyphsResident` batch-loads glyphs.
- `hookMeasureWidth` (112–116) returns `-1` on no-cover → `getTextWidth` measures with the **same resolved font the draw would use** (`GfxRenderer.cpp:669–692`) — hook-path measure and draw stay consistent.
- `hookLineHeight`/`hookAscender` (102–109) feed `getLineHeight`/`getFontAscenderSize` (`GfxRenderer.cpp:2320–2360`).
- **Why direct callers print `?`:** every `AtelierChrome::drawText` call site (header, editorial list rows, tab bar, status screens, option sheets — ~50 sites in AtelierChrome.cpp + all `drawEditorialList` users) bypasses the renderer hook entirely. `findAtelierGlyph` always returns the `?` glyph, so uncovered text renders as `?` with no fallback.

`renderGrayscalePass` (610–651): installs `textHook(...)` for the whole paint — the BW fast-only pass (617–625) **and** all three planes BW → LSB → MSB (627–650). `ditherAsGrayscalePlanes` additionally routes `LightGray`/`DarkGray` fills to gray planes (`GfxRenderer.cpp:1053–1070`). When `hookDrawText` declines inside LSB/MSB, the EpdFont draw runs under the current `renderMode`, so fallback text lands in the correct planes (black = shade 3 in every pass). **Renderer text drawing is already correct inside all 3 passes — the infrastructure exists and runs today.**

## 2. Direct callers that can receive non-Latin runtime strings

All of these reach `AtelierChrome::drawText` directly (via `drawEditorialList`/row providers), bypassing the renderer hook:

| Caller | Runtime string | Path |
|---|---|---|
| `FileBrowserActivity` | file/folder names | `renderList` → `drawEditorialList` → `drawText` (FileBrowserActivity.cpp:153, 643–727; AtelierChrome.cpp:901) |
| `LibraryListActivity` | book titles, authors | `renderList` (LibraryListActivity.cpp:1031) |
| `CoverGridHomeUi` | book titles/authors on covers + hero | direct `AtelierChrome::drawText`/`wrapText` (CoverGridHomeUi.cpp:86–100, 358–361) |
| `LanguageSelectActivity` | language names (`_language_name`: Russian `Русский`, Hebrew `עברית`, Arabic `العربية`, …) | `renderList` (LanguageSelectActivity.cpp:66–110) |
| `WifiSelectionActivity` | SSIDs | `renderList`/row provider |
| `OpdsServerListActivity` / `CatalogScreens` | OPDS feed titles | direct + list rows |
| `EpubReaderActivity` header | book title (`upperTitle`) | AtelierChrome.cpp:1045 |
| `ReaderToolbarUi`, `EndOfBookOptions`, `KOReaderSyncActivity`, `BmpViewerActivity` | chapter/book/file names | direct |
| `KeyboardEntryActivity` | key labels (Latin layout only; typed text is echoed elsewhere) | direct |

Non-Latin strings that reach `renderer.drawText(fontId,…)` (theme/`GUI` path, e.g. BaseTheme headers, OptionPopup rows via `UiScreen`) go through the hook and already fall back to EpdFont + SD fallback.

## 3. Fallback capability

**Built-in UI EpdFonts** (main.cpp:110–118, inserted 553–555): `SMALL→ubuntu_8_regular`, `UI_10→ubuntu_10_regular/bold`, `UI_12→ubuntu_12_regular/bold`. Interval table (`lib/EpdFont/builtinFonts/ubuntu_10_regular.h:3042`):

| Script | Covered? |
|---|---|
| Latin + Ext-A, combining marks, Vietnamese (0x1EA0–0x1EF9) | ✅ |
| Cyrillic (0x0400–0x04FF full) | ✅ |
| Hebrew (0x05D0–0x05EA) | ✅ |
| Arabic | ⚠️ sparse: punctuation/digits + a few letters (0x60C, 0x61B, 0x61F, 0x621, 0x640, 0x654, 0x660–0x669, 0x6BA, 0x6D4–0x6D5, 0x6F0–0x6F9) + presentation forms FB56–FEFC; **not** the main 0x0620–0x064A letter block |
| Greek (0x0370–0x03FF) | ❌ |
| CJK, Thai, Armenian, Georgian, Devanagari | ❌ |

**SD/TTF fallback:** `SdCardFontSystem::setupUiFallbacks`/`setupTtfUiFallbacks` registers same-point-size SD fonts for the 3 UI IDs via `renderer.setFallbackFont` (SdCardFontSystem.cpp:209–260, 602; `resolveTextFontId` GfxRenderer.cpp:270–299). Coverage then equals whatever `.fonts`/`fonts` families the user put on the SD card — **empty in this simulator run, so no CJK fallback today**.

**Measure compatibility:** `EpdFontFamily::getTextDimensions` (via `getTextWidth`, GfxRenderer.cpp:687–692) exists; `resolveTextFontId` + the line-box centering (726–729) already reconcile height/baseline for substituted fonts. A fix can measure with the same font it draws — no new metric API needed.

**Atelier font coverage (all 15, identical):** 772 glyphs — `0x20–0x7E`, `0xA0–0x24F`, `0x300–0x36F`, `0x1EA0–0x1EF9`, `0x2010–0x203A`. Latin + Vietnamese + punctuation only. Cyrillic/Greek/Hebrew/Arabic/CJK are all missing → `?`.

## 4. Reproduction — SIMULATOR_BEHAVIOR_VERIFIED

Temp SD `~/noveos-rec02b/_fix01/sd/` with `.epub` stubs (ASCII, Vietnamese NFC, Vietnamese NFD, Cyrillic, Greek, Hebrew, CJK). Script: Home → `TAP:300,768` (Files nav) → shot. Log `_fix01/files-run3.log`, screenshot `\_fix01\files-unicode.bmp` → `files-unicode.png`.

Result (`\\wsl$\Ubuntu\home\uturio\noveos-rec02b\_fix01\files-unicode.png`):

- `ascii-book` — correct.
- `Sách tiếng Việt` (NFC) **and** `Sách tiếng Việt NFD` (NFD) — both correct → `normalizeUtf8Stack` (AtelierChrome.cpp:206–252) normalizes to NFC and the Atelier fonts cover Vietnamese.
- `Кириллическая книга` → `??????? ??????`
- `Ελληνικό βιβλίο` → `?????????? ?????`
- `ספר עברי` → `??? ????`
- `中文书籍` → `????`

Cyrillic and Hebrew are covered by the built-in `ubuntu_10`/`ubuntu_12` EpdFonts, so a fix that routes uncovered strings through the existing hook/`resolveTextFontId` chain would render them correctly **today with no SD fonts**. Greek and CJK still need SD fallback families (none on this SD → would remain `?`).

## 5. Constraints that bind a fix

- `drawText`/`measureText`/`wrapText` use fixed 256 B stack buffers (`normalizeUtf8Stack`); row providers and `paintPass` must stay heap-free — a coverage check per string must be stack-only. `fontCoversText` already is.
- `paintPass` runs 3× (BW/LSB/MSB); any fallback must draw correctly in each `renderMode` — the hook path already does (§1).
- No recursion: a fallback must not re-enter `hookDrawText` through `renderer.drawText` while the hook is installed — the hook returns `false` for uncovered text, so delegating to `renderer.drawText` with a mapped EpdFont ID is safe, but delegating with an Atelier-mapped ID must not loop.
- IRAM is 100 % full — no new `IRAM_ATTR` code.
- X4 Pro portrait code must stay behind `#if FREEINK_DEVICE_X4PRO` + `isPortrait480x800`.
- Measurement must use the same font choice as drawing, or wrap/truncate/align (lines 133–151, 269–277) drift from the rendered width.
- A substituted font has different metrics than the Atelier row design; the renderer's line-box centering (GfxRenderer.cpp:726–729) mitigates but multi-line `wrapText` layouts need an equivalent measure.

## 6. Candidate approaches (no pick — for the lead/PO)

**A. Coverage check + EpdFont delegation at the direct-draw layer.**
In `AtelierChrome::drawText`/`measureText`/`wrapText` (or at `drawEditorialList` row level), run `fontCoversText` once per string; when it fails, delegate that string to `renderer.drawText`/`getTextWidth` with a size-matched built-in ID (e.g. `UI_10_FONT_ID`), letting `resolveTextFontId` + SD fallbacks do the rest.
Mechanism: reuses the entire existing fallback chain incl. grayscale-pass correctness and baseline centering. Trade-offs: mixed typography (1-bit Ubuntu inside 2-bit Atelier rows); `wrapText`'s multi-line layout needs a parallel wrap for the fallback font's metrics; per-string coverage scan cost (bounded, stack-only).

**B. Route direct callers through the installed hook.**
Give direct-call sites a font-ID alias and call `renderer.drawText(fontId,…)` so `hookDrawText` either draws with the Atelier font (covered) or returns `false` → EpdFont/SD fallback.
Mechanism: single choke point already wired for all 3 passes. Trade-offs: `resolveOverrideFont` covers only 3 IDs today; the other ~12 Atelier fonts need an ID mapping or an alternate hook signature carrying the `AtelierFont*`, and measure/lineHeight hooks need the same mapping or layout breaks.

**C. Regenerate AtelierFontData with more scripts.**
Re-run the font generator with e.g. Cyrillic+Greek intervals added to selected sizes.
Mechanism: zero runtime cost, identical look. Trade-offs: flash is 90.6 % used (617,902 B free) and fonts already cost 480,790 B — each added script multiplies across whichever sizes get it; cannot practically cover CJK; doesn't use the SD fallback system users may already have.

## 7. NOT TESTED / questions

- Language-select screen reproduction (option a) not run — FileBrowser repro covers the same `drawEditorialList`→`drawText` path; language names would fail identically (Hebrew/Arabic/Cyrillic `_language_name` values confirmed in YAML).
- Behavior of EpdFont fallback *with* an SD fallback font present (real `.fonts` dir) — NOT TESTED; infrastructure inspected only.
- Arabic RTL shaping through the fallback path on X4 Pro — built-in coverage is sparse; real Arabic text needs SD fonts + the renderer's bidi path (`resolveVisualText`) — NOT TESTED.
- env:default (ESP32-C3) build fails on a host SCons issue — irrelevant to this path (Atelier chrome is X4 Pro-only) but noted in FNT-01.

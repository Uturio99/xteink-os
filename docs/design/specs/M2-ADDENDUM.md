# XTEInk OS — M2 Technical Design Addendum

> **Document ID:** M2-ADDENDUM
> **Status:** GOVERNANCE RECONCILIATION APPROVED — implementation remains bounded
> by the permissions recorded below.
> **Authorization state:** M2 governance is approved. Existing WP2.1–WP2.3
> checkpoint approvals and WP2.4 implementation/test-only authorizations are
> confirmed without inventing historical approval dates. WP2.4 remains
> uncommitted pending verification and firmware-handoff requirements.
> **Supersedes:** nothing. This addendum amends the frozen M1 specification
> ([`UNIFIED_UI_COLLECTION.md`](UNIFIED_UI_COLLECTION.md) @ commit `1225e580`)
> without modifying it. Where this addendum conflicts with the M1 spec text,
> this approved reconciliation governs M2; M1 remains frozen and unchanged.
> **Repository:** `xteink-os` on branch `feat/m1-unified-ui-collection`
> **SDK pin:** `freeink-sdk @ 946e6ac` (boox-picco-131)
> **Date:** 2026-11-03

---

## 1. Conflict & Reconciliation Matrix

All evidence from the current branch at `1225e580`. "Spec fix" rows are changes
*proposed* for this addendum, not edits to the frozen M1 document.

| ID | Conflict (M1 ↔ firmware) | Evidence (path:line) | Minimal compatible solution | Spec fix | PO gate |
| :-- | :-- | :-- | :-- | :-- | :-- |
| C1 | FR-07/FR-13 list `.fb2`, `.pdf` as supported; no FB2/PDF parser exists, PDF is explicitly out of scope | `lib/` (no fb2/pdf library); `SCOPE.md` §4 "PDF Rendering … Out of scope"; actual formats: EPUB/TXT/XTC (`lib/Epub`, `lib/Txt`, `lib/Xtc`) | Format guide shows `.epub, .txt, .xtc` only | FR-07 footer, FR-13 fallback copy | PD-1 |
| C2 | FR-02 shows Web portal at `:8080`; firmware web server is fixed port 80, no port setting exists | `src/network/CrossPointWebServer.h:82` (`uint16_t port = 80`) | FR-02 shows `http://<device-ip>` (IP resolved at runtime); port customization deferred | FR-02 row 2 | PD-7 |
| C3 | FR-15 offers refresh every {5,10,20,never}; firmware offers {1,5,10,15,30,never} | `src/SettingsList.cpp:48-53` (`refreshFrequency` enum); runtime counter `src/activities/reader/ReaderActivity.h:17` | Reuse firmware options; addendum restates the radio list | FR-15 Setting 1 | PD-7 |
| C4 | FR-14/FR-15 expose "Sunlight Fading Fix"; firmware hides `fadingFix` on touch boards (X4 Pro has touch) | `src/SettingsList.cpp:56-57` (`if (!BoardConfig::hasTouch())`) | Keep hidden on X4 Pro pending investigation of why it is touch-gated; addendum removes that row for X4 Pro | FR-14 Group 1, FR-15 Setting 2 | PD-6 |
| C5 | FR-14 says default font "16px"; FR-11 says standard is 19px; both claim to be the default | `src/CrossPointSettings.h:325` (`fontPointSize = DEFAULT_FONT_POINT_SIZE`, consumed as pixels: `freeink-sdk/libs/font/FreeInkFont/include/Font.h:45-96`) | Reader target = Serif 19px / line-height 1.65; Settings shows the *live* setting value, never a hardcoded number | FR-14 Group 2 | PD-2 |
| C6 | FR-08 tap zones: left 30% back / center 20% HUD / right 50% forward; firmware splits screen into thirds + center-third menu rectangle | `src/activities/reader/ReaderUtils.h:110-145` (`width/3` splits; `isTouchMenuTap` center box); gestures configurable via `CrossPointSettings.h:395` | Parameterize zone boundaries in `detectTouchPageTurn`; convert `isTouchMenuTap` to a horizontal band [30%..50%]; RTL/Inverted logic already handled in place | none (firmware change) | PD-3 |
| C7 | FR-08 footer & FR-10 TOC show whole-book page numbers ("Trang 42/280"); firmware paginates per chapter, position + percent exist | `src/activities/reader/ChapterPosition.h:9-17` (chapter-local `pageIndex` + `displayPage()` + fraction); `EpubReaderPercentSelectionActivity` (percent navigation); `SettingsList.cpp:327` (`statusBarChapterPageCount`) | Use chapter-relative page + whole-book percent everywhere (footer, TOC, HUD); no full-book pagination pass in M2 | FR-08 footer format, FR-10 row subtitle | D8 |
| C8 | `PROJECT_STATUS.md` M1 notes say native search will use "live SQLite"; there is no SQLite — the catalog is the custom CLIX index | `lib/LibraryIndex/LibraryIndexFile.h:75-98` (`ClixRecord` accessor API); no sqlite anywhere in `lib/` | Documentation correction: "CLIX LibraryIndex catalog" | doc only | closed (engineering) |

## 2. Decision Register

### 2.1 Product decisions — PO-approved defaults

These decisions authorize only the bounded work described in this addendum;
they do not authorize unrelated implementation or scope expansion.

| ID | Recommendation | Evidence anchor | Impact if reversed |
| :-- | :-- | :-- | :-- |
| PD-1 | Formats: EPUB, TXT, XTC only where verified; exclude PDF/FB2 | C1 | Approved |
| PD-2 | Reader target typography: Serif 19px / 1.65 | C5 | Approved |
| PD-3 | Tap zones: left 30% / center 20% / right 50% (replacing thirds) | C6 | Approved |
| PD-4 | Full-library progress sort must NOT silently use incomplete recent-book data; defer if accuracy is not achievable | D10 | Approved |
| PD-5 | Home curation: rule-based on existing CLIX metadata (e.g. newest unstarted) | FR-03 | Approved |
| PD-6 | Preserve existing touch-board handling of Sunlight Fading Fix (hidden on X4 Pro); investigation explicitly deferred | C4 | Approved; investigation deferred |
| PD-7 | Reuse existing refresh-frequency options and port-80 web server; do not add "20 pages" or a port setting | C2, C3 | Approved |

### 2.2 Resolved decision defaults (from PO directive, still require sign-off)

| ID | Decision | Bound |
| :-- | :-- | :-- |
| D8 | Chapter-relative page numbers + whole-book percentage; no full-book pagination without measured feasibility | Confirmed unchanged; bound to M2 scope (C7) |
| D9 | NotoSans remains the mandatory fallback; Reader Serif remains SD-loaded TTF; evaluate only a minimal bundled UI Serif subset | Accepted WP1 direction; exact subset and weight remain open |
| D10 | Implement genuine full-library progress sort only with accurate data; otherwise defer | Confirmed unchanged; bound by WP3 data audit |

### 2.3 Governance approvals and boundaries

- PD-1 through PD-7 are approved using the documented defaults; PD-6's
  investigation is explicitly deferred.
- D8 and D10 remain unchanged. The accepted WP1 D9 direction is preserved;
  exact bundled-Serif subset and weight remain open.
- D11 analysis is delegated to engineering. Engineering must provide a measured
  flash-reserve recommendation; the PO must approve it before any bundled Serif
  integration.
- WP2.1–WP2.3 task-specific implementation and checkpoint approvals are
  confirmed. WP2.4 has separate implementation and test-only authorization;
  its current six-file patch remains uncommitted pending verification and
  firmware-handoff requirements.
- These approvals do not expand M2 scope. Hardware validation is an early,
  separately authorized activity, not a release-only activity.

### 2.4 Verified engineering constraints (evidence, no approval needed)

| ID | Constraint | Evidence |
| :-- | :-- | :-- |
| VE-1 | X4 Pro = ESP32-S3, 16 MB flash, 8 MB PSRAM, panel 800×480, controller auto-select SSD1677/UC8179/UC8279 | `platformio.ini:392-412`; `freeink-sdk/.../BoardConfig.h:122-144` |
| VE-2 | GT911 touch at I2C 0x5D (alt probe 0x14), portrait-native 480×800 | `freeink-sdk/docs/xteink-x4pro-support.md:168-169, 285`; `BoardConfig.h:629` |
| VE-3 | Vietnamese glyph coverage already generated into shipped built-in fonts (U+1EA0-1EF9, U+01A0-01AF) | `lib/EpdFont/builtinFonts/notosans_16_regular.h` (contains 0x1EA0/0x1EF9/0x01A0/0x01AF); `lib/EpdFont/scripts/fontconvert.py:57-68, 827` |
| VE-4 | Vector TTF runtime exists (stb_truetype, pixel-sized, PSRAM arena cache with high-water diagnostics); SD TTF loading API exists | `freeink-sdk/.../TtfFont.cpp:62-166`; `FontArena.h:5-84`; `src/SdCardFontSystem.h:43, 152` |
| VE-5 | Diacritic-insensitive search + fold sorting exist | `lib/LibraryIndex/LibraryText.cpp:69`; `LibraryText.h:44` |
| VE-6 | Vietnamese UI translations exist and are wired into the STR_ key system | `lib/I18n/translations/vietnamese.yaml` (e.g. STR_SETTINGS_TITLE:12, STR_LIBRARY:138) |
| VE-7 | Full-refresh counter machinery exists and is user-configurable | `ReaderActivity.h:17` (`pagesUntilFullRefresh`); `SettingsList.cpp:48-53` |
| VE-8 | H1.1A native home commit `877461d3` is an ancestor of this branch; reuse is sanctioned by `docs/design/BASELINE.md` §3 | `git branch -a --contains 877461d3` (verified 2026-11-03) |
| VE-9 | NO PlatformIO simulator environment exists in this repository. Verified environments: `default`, `gh_release(_rc)`, `slim`, `sticky*`, `x4pro*`, `metalio_eink4*`, `x4c*`, `papermono*`, `eego_a4*`, `picco*`. Host verification is the native gtest suite under `test/`. | `platformio.ini:306-740` (env table); `test/README` |
| VE-10 | EPUB reflow/pagination engine is complete and must be reused, not duplicated | `lib/Epub/`, `src/activities/reader/ChapterBuildGate.h`, `ChapterPosition.h` |

## 3. M2 Native Architecture (binding for implementation work)

Principle: **no competing UI framework, no second EPUB engine.** Every frame is
an activity on the existing stack (`UiAppHost` / `UiListActivity` /
`UiTabListActivity` / `OptionPopup` / `fui::` primitives), reusing the CLIX
index, `RecentBooksStore`, EPUB reflow, touch/display HAL, and the font
pipeline. Component-level mapping is maintained in the M2a audit and is
normative for work packages:

- **Home FR-01..03** — extend `src/components/CoverGridHomeUi.*`,
  `X4ProHomeLayout.h`, `src/activities/home/HomeActivity.cpp`;
  FR-02 ingestion rows reuse the web server (`src/network/`), USB MSC
  (`platformio.ini:400` `FREEINK_CAP_USB_MSC`), and CLIX scan
  (`lib/LibraryIndex/LibraryBuilder.*`); FR-03 curation is rule-based (PD-5).
- **Library FR-04..07** — extend `LibraryListActivity.*`; FR-04 adds one grid
  component reusing `HomeCoverCache`; search states reuse `STR_LIBRARY_EMPTY`
  vs `STR_LIBRARY_NO_RESULTS` (`lib/I18n/I18nKeys.h:280-281`); progress sort
  bounded by PD-4/D10.
- **Reader FR-08..11** — reuse the EPUB engine and chapter navigation
  (VE-10); zones parameterized per C6; pagination display per D8; HUD/TOC/Aa
  rework `EpubReaderMenuActivity`, `EpubReaderChapterSelectionActivity`
  (windowed TOC, 24-row, SD-backed), and the existing Text Settings tabs.
- **Files FR-12..13** — extend `FileBrowserActivity.*`; FR-13 modal reuses
  `epub.loadMetadata` (pattern: `lib/LibraryIndex/LibraryBuilder.cpp:362`).
- **Settings FR-14..15** — regroup `src/SettingsList.cpp`; add
  "Device info" group sourced from `CROSSPOINT_VERSION` (`platformio.ini:403`);
  FR-15 uses `OptionPopup` + `SettingInfo::Enum/Toggle`.

## 4. WP1 — Font Feasibility Verification Contract

Executable only after PO approval of the named permission tiers (§4.5).
Produces the measurements D9 needs. Adds exactly one host test target under
`test/` and zero firmware changes. All three strategies are measured
independently; one failing does not block the others.

### 4.1 Strategy matrix

| Strategy | Source & mechanism | Availability at startup | Missing-font / missing-SD behavior | Files touched if adopted |
| :-- | :-- | :-- | :-- | :-- |
| **A. Built-in bitmap** (status quo) | Shipped NotoSans 12/14/16/18 px ROM headers; Vietnamese glyphs already embedded (VE-3) | Always — static const data in flash | Never missing; this IS the fallback floor | none (fallback only) |
| **B. Bundled UI Serif** | In-repo NotoSerif TTFs (OFL: `docs/design/prototype/fonts/NotoSerif-*.ttf`) → `lib/EpdFont/scripts/fontconvert.py` ROM headers at M1 UI sizes {12,13,15,17,30,34} px | Always, after the bundling decision | Registration failure → existing fallback chain to NotoSans | + generated headers + ≤5-line font-set registration |
| **C. SD vector Serif** (Reader) | Runtime TTF via `src/SdCardFontSystem.h:43/152` (`resolveFontId`, `loadTtfVariant`); engine = SDK stb_truetype, pixel-sized (VE-4) | Only when the user installs a TTF family on SD | No SD / load failure → `setupUiFallbacks`/`setupTtfUiFallbacks` chain (`SdCardFontSystem.h:66-84`) → NotoSans; Reader re-snaps sizes (`EpubReaderActivity.cpp:2541`) | none in firmware; distribution = docs + SD copy step |

### 4.2 Required measurements per strategy

1. **Glyph coverage sweep (A, B, C).** Enumerate U+1EA0-1EF9, U+01A0-01AF,
   U+0300-036F, plus NFC and NFD encodings of the fixture sentences used by
   `test/utf8_compose` and `test/combining_marks`, at each target size
   (A: 12/14/16/18 · B: 12/13/15/17/30/34 · C: reader 16/18/19/22 + UI sizes).
   Method: one new host gtest `test/vietnamese_font_coverage/` that decodes
   A's shipped `.h` tables, decodes B's generated tables, and compiles the
   existing SDK engine sources read-only (`TtfFont.cpp` + stb_truetype) for
   C's glyph-index and rasterization checks. Evidence: per-size missing-glyph
   table. **Accept: 0 missing on every shipped path. Reject: any missing
   Vietnamese glyph on a path that ships.**
2. **Availability at startup.** A/B: static registration asserted via the
   existing font-set wiring (`src/fontIds.h`). C: `discover` +
   `loadTtfVariant` against the fake-HalStorage pattern proven in
   `test/sd_font_registry/SdFontRegistryTest.cpp:1-50`. **Accept:
   deterministic availability state; no crash when SD is absent.**
3. **Fallback proof (B-fail, C-absent).** Through the real fallback chain
   (`SdCardFontSystem.h:66-84`), every Vietnamese codepoint must resolve to a
   built-in NotoSans glyph. **Accept: 0 missing after fallback.**
4. **Flash measurement.** F1 (host): parse shipped + generated header arrays
   → exact flash bytes per font (no firmware build needed). F2 (Tier F):
   `pio run -e x4pro` baseline vs +B, PlatformIO size report, same tree,
   headers-only change. Evidence: delta table. No fixed budget number exists
   yet — `.agents/rules/hardware-resources.md` sets a 16 MB flash baseline
   and requires checking the build map, so the headroom threshold is PO
   decision D11.
5. **PSRAM / glyph-cache diagnostics.** Tier F: `FontArena.h:81-84`
   high-water and largest-refused while rendering the longest fixture chapter
   with C active; A/B record `data`/`BSS` deltas instead. Host-run arena
   numbers are labeled host-derived. **Accept: high-water within the
   startup-configured arena budget; no new internal-RAM claim on C3
   (rule §8-9).**
6. **Startup cost (C only).** Host-timed `loadTtfVariant` on
   `NotoSerif-Regular.ttf`. Recorded, non-gating, labeled host-derived.
7. **Hardware-only measurements (out of WP1).** Page-render latency,
   ghosting, refresh behavior — deferred to WP7/M6 (VE-9). Never quoted from
   host runs.

### 4.3 Commands (verified against this repository)

```text
# Tier H — host tests (WP1 adds test/vietnamese_font_coverage/ plus ONE
# add_subdirectory line in test/CMakeLists.txt; per test/README):
cmake -S test -B build/test
cmake --build build/test
ctest --test-dir build/test --output-on-failure -j
# single suite:
cmake --build build/test --target VietnameseFontCoverageTest
build/test/vietnamese_font_coverage/VietnameseFontCoverageTest

# Tier F — firmware size report (NO upload, NO flash):
pio run -e x4pro
```

No PlatformIO simulator environment exists in this repository (VE-9). Host
timings are never e-paper timings.

### 4.4 Exit criteria → D9

- Coverage table complete for A, B, C → D9 decidable with numbers.
- Decision rule (proposed): if B's flash delta ≤ PO-set headroom (D11) →
  ship B for UI typography + C for Reader serif, A as fallback floor.
  If B exceeds → C-only + A; B headers stay shelved on a branch.
- If C fails coverage at reader sizes → the serif default (PD-2) falls back
  to sans vector sizes 16-22 and PD-2 returns to the PO.

### 4.5 Permission boundary

| Tier | Allowed scope | Explicitly forbidden |
| :-- | :-- | :-- |
| **H — host tests** | create `test/vietnamese_font_coverage/`; add one `add_subdirectory` line to `test/CMakeLists.txt`; run cmake/ctest; compile SDK font sources read-only | any change under `src/`, `freeink-sdk/`, `platformio.ini`, `.agents/`, M1 documents, submodules, dependencies |
| **F — firmware build** | `pio run -e x4pro` for size reports; stage generated B headers on a separate branch for measurement F2 | upload, flash, serial monitor on hardware; editing `platformio.ini`; touching submodules or dependencies |
| **W — hardware** | flashing and panel measurement | everything in WP1; requires its own later approval (M6/WP7) |

WP1 requests tiers **H** and **F** only. Each tier runs only after the PO
explicitly names it. Everything forbidden in H stays forbidden in F; the
Tier W boundary is absolute for WP1.

## 5. Work-Package Register (acceptance criteria)

| WP | Scope | Sim/host acceptance | Hardware acceptance (M6/M7 gate) |
| :-- | :-- | :-- | :-- |
| WP0 | This addendum + PO sign-off | PO sign-off recorded | — |
| WP1 | Font experiment (§4) | Coverage 0-missing on shipped paths; flash/PSRAM numbers recorded | startup + page-render sanity on X4 Pro |
| WP2 | Home FR-01..03 | Host tests for layout math (`test/x4pro_home_layout` pattern); FR-02 shows real IP; FR-03 rules deterministic | ghosting/refresh check on panel |
| WP3 | Library FR-04..07 | search `chien tranh` → `Nỗi buồn chiến tranh`; sort modes; empty vs no-results distinction | 1000+ book SD scan stays within C3 budget |
| WP4 | Files FR-12..13 | breadcrumb depth; FR-13 metadata + unsupported-file fallback | real SD card, USB-MSC transfer path |
| WP5 | Reader FR-08..11 | zone unit tests (30/20/50); TOC windowed render; reflow 16↔22 keeps position | page-turn latency target on-panel (M6 measurement) |
| WP6 | Settings FR-14..15 | groups per addendum; values reflect live settings; `vietnamese.yaml` complete | — |
| WP7 | Hardware validation | — | per `PROJECT_STATUS.md` M6/M7 rows |

Simulator-timing caveat stands: **no simulator timings may be quoted as e-paper
behavior** (VE-9). All page-turn/refresh latency acceptance is deferred to
hardware (WP7).

## 6. Documentation Corrections (proposed, none applied)

1. `docs/PROJECT_STATUS.md` — replace "live SQLite/catalog index" (M1 notes)
   with "CLIX LibraryIndex catalog" (C8).
2. `docs/PROJECT_MAP.md` §2 — `src/hal/`, `src/models/`, `src/services/` do
   not exist; correct the tree to actual directories.
3. `docs/PROJECT_MAP.md` — remove "GT911 subject to SDK board-profile
   verification"; it is verified (VE-2).
4. `docs/design/BASELINE.md` §2 — note H1.1A is ancestor of the M1 design
   branch; still not merged to `develop`.
5. M1 limitation "Reader dynamic reflow must be implemented by native engine" —
   closable: engine verified present (VE-10).

## 7. Remaining Product Owner Decisions

1. Engineering must complete D11 analysis and return a measured flash-reserve
   recommendation; bundled Serif integration requires later PO approval.
2. PO approval remains required for the exact D9 bundled-Serif subset and
   weight after WP2 evidence.
3. Name the X4 Pro panel variant present on PO hardware (SSD1677 / UC8179 /
   UC8279) if known — this reduces the early hardware-validation surface.

# Claude Code Handoff — NoveOS UI & Firmware (`xteink-os-antigravity-ui`)

> **Handoff Date:** 2026-11-18  
> **Repository Root:** `xteink-os-antigravity-ui` (standalone Git repo, branch `experiment/antigravity-crosspoint-ui`)  
> **Target Device:** XTEInk X4 Pro (ESP32-S3, 480x800 Portrait SSD1677 4-level grayscale E-Ink, GT911 touch + hardware buttons)  
> **SDK Submodule:** `freeink-sdk` pinned at `946e6ac3d7a9d590ba3224428e6183d55ac7917c` (`lucide` at `c81680e066f45b640743ca78ae36cdedda3f0318`) — keep pinned unless explicitly upgrading SDK.

---

## 1. Current Verified Project State

### What Was Completed Before Handoff
1. **Concept 01 — Atelier Typographic UI (`480x800` Portrait):**
   - Custom 4-level grayscale AA editorial engine (`AtelierChrome`, 14 baked 2-bit NotoSans/NotoSerif fonts covering 772 Latin/Vietnamese/punctuation glyphs in `src/components/AtelierFontData.h`).
   - Full X4 Pro UI coverage across Home, Library, File Browser, Settings & sub-screens, OPDS/Plugins/Catalog, Network/Wi-Fi/Calibre/WebServer/USB, Reader toolbar & modals, Boot/Sleep/About, and Crash/FullScreen messages.
   - Row selection (`AtelierSelection.h`): `5px` solid Ink left indicator bar, `16px` gap, `+21px` text indent, bold Ink index number, and zero Bayer dither ghosting.
   - Periodic focus refresh cadence (`AtelierFocusRefreshTracker` + `planAtelierFocusRefresh`): 7 BW-only `FAST_REFRESH` steps followed by 1 full 3-pass `HALF_REFRESH` cleanup.
2. **NoveOS Branding & Assets:**
   - Replaced CrossPoint bootloader/standby/About branding with the **NoveOS Crane Emblem Logo**:
     - Generator script: `scripts/convert_noveos_logo.py`
     - Header asset: `src/images/NoveOsLogo.h` (`NoveOsBranding::OS_NAME` `"NoveOS"`, `OS_EYEBROW` `"00.1 — NOVEOS SYSTEM"`, `NOVEOS_LOCKUP_2BPP` `220x226` MSB-first 2-bit packed, `NOVEOS_LOGO_1BPP` `120x120` 90° CCW pre-rotated fallback for legacy `GfxRenderer::drawImage`).
     - Splash renderer: `AtelierChrome::drawNoveOsSplashScreen` used by `BootActivity` (`BootActivity.cpp`) and `SleepActivity` (`SleepActivity.cpp`).
     - `AboutActivity` (`src/activities/settings/AboutActivity.cpp`): displays `NoveOS <version>`.
3. **Full `I18N` System Language Restoration (`tr(STR_...)`):**
   - Removed the `#if FREEINK_DEVICE_X4PRO` language hardcode in `src/main.cpp` (default is `Language::EN`, persisted in `/.crosspoint/settings.json`, switchable live via `LanguageSelectActivity`).
   - Replaced all hardcoded Vietnamese strings across `AtelierChrome.cpp`, `CoverGridHomeUi.cpp`, `LibraryListActivity.cpp`, `FileBrowserActivity.cpp`, `SettingsListActivity.cpp`, `ConfirmationActivity.cpp`, `FullScreenMessageActivity.cpp`, `ReaderToolbarUi.cpp`, and all footer hints (`CrashActivity`, `CalibreConnectActivity`, `CrossPointWebServerActivity`, `UsbDriveActivity`, `WifiSelectionActivity`, `EpubReaderActivity`, `KOReaderSyncActivity`, `QrDisplayActivity`, `XtcReaderActivity`, `ClearCacheActivity`, `ClockSyncActivity`, `FontDownloadActivity`, `KOReaderAuthActivity`, `OtaUpdateActivity`, `SdFirmwareUpdateActivity`).
   - Added 23 new keys cleanly in `lib/I18n/translations/english.yaml` and `lib/I18n/translations/vietnamese.yaml` (679 keys total, 0 unused keys against English/reference scan).
   - Preserved all Vietnamese/Unicode reader fonts (`src/fonts/`, `lib/EpdFont/`) and verified Vietnamese book titles/authors continue rendering with 2-bit AA even when system UI is English.

### Latest Verified Build & Test Baseline (Before Final Handoff Commit)
- **Python host tests (`python3 -m unittest discover -s test -p 'test_*.py' -t test`):** `8/8` suites PASSED (`0.038s`).
- **Host C++ CTest (`ctest --output-on-failure`):** `906/906` PASSED (`20.57s`), including `X4ProHomeLayoutTest` and `VietnameseFontCoverageTest`.
- **PlatformIO `env:simulator` (non-X4 Pro):** `SUCCESS`.
- **PlatformIO `env:simulator_x4_pro` (`480x800` X4 Pro simulator):** `SUCCESS` — 13 UI & live language-switch (`VI -> EN`) screenshots captured via WSLg and visually verified.
- **PlatformIO `env:x4pro` (ESP32-S3 firmware):** `SUCCESS`:
  - `RAM`: `34.1%` (`111,780 / 327,680 B`)
  - `Flash`: `90.6%` (`5,935,698 / 6,553,600 B`)
  - `DIRAM`: `59.12%` (`202,038 / 341,760 B`)
  - `IRAM`: `100.0%` (`16,384 / 16,384 B` — **0 bytes free**)
- **Known host tool notes from earlier runs:**
  - `clang-format-21` is not installed on Windows/WSL (`bin/clang-format-fix.ps1` unavailable).
  - `pio run -e default` (ESP32-C3) failed in local pioarduino SCons environment (`ModuleNotFoundError: No module named 'SCons.Tool.FortranCommon'`); non-X4 Pro paths are verified via `env:simulator`.

---

## 2. Architecture & Key File Map

### Core UI & Rendering (`src/components/`)
- **`AtelierChrome.h` / `AtelierChrome.cpp`**:
  - Grayscale 4-shade drawing primitives (`paintPixel`, `fillRect`, `drawLineAA`, `fillCapsule`, `strokeCapsule`, `drawCapsuleButton`, `fillCircle`, vector icons).
  - Shared screen chrome (`drawStatusBar`, `drawHeader`, `drawSubHeader`, `drawTabBar`, `drawBottomNav`, `handleBottomNavTap`, `takePendingBottomNavTab`, `drawEditorialList`, `drawFooterHint`, `drawStatusScreen`, `renderOptionSheet`, `drawReaderChrome`, `drawScrubberTrack`, `drawEditorialMessageScreen`, `drawNoveOsSplashScreen`).
  - `textHook(ditherAsGrayscalePlanes)` & `renderGrayscalePass(...)`: redirects `GfxRenderer` `SMALL_FONT_ID` / `UI_10_FONT_ID` / `UI_12_FONT_ID` to 2-bit AA `AtelierFont`s when `fontCoversText()` succeeds, and executes 3-pass grayscale (`BW` -> `GRAYSCALE_LSB` -> `GRAYSCALE_MSB`).
- **`AtelierFontData.h`**: 14 auto-generated 2-bit AA NotoSans/NotoSerif fonts (`font8Bold` .. `font34Light`), 772 glyphs covering ASCII, Latin-1/Latin-Ext, Vietnamese (`U+1EA0..1EF9`), and editorial punctuation (`U+2010..203A`).
- **`AtelierSelection.h`** & **`X4ProHomeLayout.h`**: Pure constexpr geometry helpers shared between firmware and host unit tests (`test/x4pro_home_layout/X4ProHomeLayoutTest.cpp`).
- **`CoverGridHomeUi.cpp`**: X4 Pro `480x800` Home screen (`drawX4ProPortrait`) for empty state and active reading hero + editorial cover + recent books shelf.
- **`CatalogScreens.h` / `.cpp`**: Shared catalog/OPDS/plugin header, empty/status message block, and download progress UI.
- **`OptionPopup.h`** & **`UiSliderDialog.h`**: Atelier modal bottom sheet overlays.
- **`themes/BaseTheme.cpp`, `lyra/LyraTheme.cpp`, `roundedraff/RoundedRaffTheme.cpp`**: X4 Pro uses `LyraTheme` (`COVER_GRID`); `LyraTheme` & `RoundedRaffTheme` delegate header/subHeader/buttonHints to `BaseTheme` on `480x800` portrait so `AtelierChrome` takes effect consistently.

### Activities (`src/activities/`)
- **Base list/tab hosts:** `UiListActivity.h/.cpp` (with `x4ProFocus`, swipe-to-delete, bottom nav routing in `loop()`), `UiTabListActivity.cpp`.
- **Home & Files:** `home/HomeActivity.cpp`, `home/FileBrowserActivity.cpp`, `home/CrashActivity.cpp`.
- **Library:** `library/LibraryListActivity.cpp` (tabs: `Recent`, `Title`, `Author`; custom `02.1` header with refresh/search buttons and book/group counter).
- **Reader:** `reader/ReaderToolbarUi.cpp`, `reader/ClipSelectionActivity.cpp`, `reader/EpubReaderActivity.cpp`, `reader/EpubReaderFootnoteSelectActivity.cpp`, `reader/EpubReaderMenuActivity.cpp`, `reader/DictionaryWordSelectActivity.cpp`, `reader/XtcReaderActivity.cpp`, `reader/KOReaderSyncActivity.cpp`, `reader/QrDisplayActivity.cpp`.
- **Settings:** `settings/SettingsListActivity.cpp`, `settings/SettingsActivity.cpp`, `settings/LanguageSelectActivity.cpp`, `settings/TextSettingsActivity.cpp` & `TextSettingsPreview.cpp`, `settings/AboutActivity.cpp`, `settings/OpdsServerListActivity.cpp`, `settings/OpdsSettingsActivity.cpp`, `settings/ClockSettingsActivity.cpp`, `settings/ClockSyncActivity.cpp`, `settings/ClearCacheActivity.cpp`, `settings/FontDownloadActivity.cpp`, `settings/OtaUpdateActivity.cpp`, `settings/SdFirmwareUpdateActivity.cpp`.
- **Network & Utilities:** `network/WifiSelectionActivity.cpp`, `network/CalibreConnectActivity.cpp`, `network/CrossPointWebServerActivity.cpp`, `network/UsbDriveActivity.cpp`, `network/NetworkModeSelectionActivity.cpp`, `util/ConfirmationActivity.cpp`, `util/FullScreenMessageActivity.cpp`, `util/BmpViewerActivity.cpp`, `util/FrontlightPanelActivity.cpp`, `util/KeyboardEntryActivity.cpp`.

### Internationalization (`lib/I18n/`)
- Translation sources: `lib/I18n/translations/*.yaml` (`english.yaml` is the fallback reference; all missing keys in other languages automatically inherit English).
- Generator: `scripts/gen_i18n.py` (automatically invoked by PlatformIO `pre:scripts/gen_i18n.py`; outputs gitignored `lib/I18n/I18nKeys.h`, `I18nStrings.h`, `I18nStrings.cpp`).
- Important C++ macro rule: `tr(id)` expands to `I18n::getInstance().get(StrId::id)` — **never** pass ternary expressions inside `tr(...)` (always write `cond ? tr(STR_A) : tr(STR_B)`).

---

## 3. Build, Test, and Simulator Workflow

> On Windows host with WSL: run PlatformIO/CMake builds inside WSL on a synced Linux copy (or native Linux environment) for fast builds and simulator execution. Remember Windows CRLF must not be piped into bash scripts.

### 1. Regenerate I18n Headers (after YAML edits)
```bash
python3 scripts/gen_i18n.py lib/I18n/translations lib/I18n/
```

### 2. Host Tests (Python + CMake/CTest)
```bash
# Python regression tests
python3 -m unittest discover -s test -p 'test_*.py' -t test

# GoogleTest host suite (configure once, then build & run)
cmake -S test -B build/ctest
cmake --build build/ctest -j
ctest --test-dir build/ctest --output-on-failure
```

### 3. PlatformIO Builds (Simulator & Device Firmware)
```bash
pio run -e simulator          # Non-X4 Pro regression guard
pio run -e simulator_x4_pro   # 480x800 X4 Pro desktop simulator
pio run -e x4pro              # XTEInk X4 Pro ESP32-S3 firmware
```

### 4. Headless Simulator & Screenshot Automation (`simulator_x4_pro`)
```bash
unset SDL_VIDEODRIVER
export DISPLAY=:0 WAYLAND_DISPLAY=wayland-0 XDG_RUNTIME_DIR=/mnt/wslg/runtime-dir
export CROSSPOINT_SIM_SD=/path/to/test-sd
export CROSSPOINT_SIM_WAIT_TIMEOUT_MS=20000
# Note: timestamps inside each WAIT segment MUST be strictly increasing!
export CROSSPOINT_SIM_INPUT_SCRIPT='0:WAIT:Entering activity: Home;600:SHOT:/tmp/home.bmp;750:TAP:420,768;0:WAIT:Entering activity: SettingsList;600:SHOT:/tmp/settings.bmp;800:QUIT'
.pio/build/simulator_x4_pro/program
```

---

## 4. Open Priorities for Claude Code (Features, Interactions, Display Optimization)

### Priority A — Display & Typography Optimizations
1. **Fallback rendering for non-Latin scripts in `AtelierChrome::drawText`:**
   - Currently `AtelierChrome::hookDrawText` checks `fontCoversText()` and falls back to `GfxRenderer`'s `EpdFont` when non-Latin characters appear, **but direct callers of `AtelierChrome::drawText`** (such as `drawEditorialList` in `LanguageSelectActivity` or book rows with Arabic/Cyrillic/CJK titles) render missing glyphs in `AtelierFontData.h` as `?`.
   - **Action:** Add a clean fallback inside `AtelierChrome::drawText` (or `drawEditorialList` / `LanguageSelectActivity`) to route strings where `!fontCoversText(font, text)` through `EpdFont`/renderer text drawing without breaking 3-pass grayscale planes (`renderGrayscalePass`) or adding heap allocations.
2. **E-Ink Grayscale Contrast & Refresh Tuning on Real Hardware:**
   - Verify ghosting, 4-level AA contrast ratios, `ATELIER_FOCUS_CLEAN_INTERVAL` (currently 7 fast BW steps before 1 clean 3-pass `HALF_REFRESH`), and partial-refresh responsiveness on physical X4 Pro hardware (`Boot`, `Sleep`, `Home`, `Library`, `Reader`, `TextSettings`).
   - Fine-tune `UiSliderDialog`, `FrontlightPanelActivity`, and `ReaderToolbarUi` scrubber hit targets and visual contrast on physical e-paper.
3. **Reader & Vietnamese Glyph Polish:**
   - Audit line breaking, hyphenation, justification, and Vietnamese diacritic vertical clearance in `EpubReaderActivity`, `TextSettingsPreview`, and `ClipSelectionActivity` across all built-in and SD fonts.
   - Measure `AtelierChrome::drawReaderChrome` margins and bottom progress scrubber spacing on long Vietnamese chapter names.

### Priority B — Interactions & Controls (UX / Touch + Physical Buttons)
1. **Touch vs. Physical Button Coexistence:**
   - Verify consistent `BACK`, `ENTER`, `UP`/`DOWN`, `LEFT`/`RIGHT`, long-press, and swipe-to-delete interactions across `LibraryListActivity`, `FileBrowserActivity`, `SettingsActivity`, `WifiSelectionActivity`, and `ReaderToolbarUi` (both touch-only and button navigation flows).
2. **Bottom Navigation & Stack Depth Edge Cases:**
   - Audit cross-tab navigation (`Home <-> Library <-> FileBrowser <-> SettingsList`) via `AtelierChrome::handleBottomNavTap` / `takePendingBottomNavTab` when sub-activities or overlays (`OptionPopup`, `ConfirmationActivity`) are open.
3. **Popup & Keyboard Entry Ergonomics:**
   - Verify touch hit targets (>= 44px) and dismissal behavior on `KeyboardEntryActivity`, `OptionPopup`, `UiSliderDialog`, `SleepActivity` wake popup (`drawSleepPopupPreservingFrame`), and `CatalogScreens`.

### Priority C — Feature Completion & Stability
1. **End-to-End Hardware Verification (`HARDWARE_VERIFIED`):**
   - Test firmware `.pio/build/x4pro/firmware.bin` on real XTEInk X4 Pro hardware: SD mount/unmount, Wi-Fi & AP hotspot QR onboarding, Calibre wireless transfer, OPDS download, Plugin Hub, OTA check/guard, BLE page turner, KOReader sync, and sleep cover modes.
2. **Remaining Legacy / Edge Screens Audit:**
   - Check less-visited flows (`WifiSelectionActivity` hidden SSID & password prompts, `OpdsBookBrowserActivity` loan/LCP states, `DictionaryDefinitionActivity`, `PluginCatalogActivity` OAuth/install flows, `ButtonRemapActivity` / `HomeButtonSettingsActivity`) on `480x800` simulator and device to ensure every state, error banner, and footer hint stays balanced and unclipped across languages.
3. **Heap / Memory Budget Discipline:**
   - Monitor `.text` in IRAM (`16,384 / 16,384` full), Flash usage (`90.6%`), internal heap / PSRAM largest free blocks on device during long EPUB reading sessions, library indexing, and WebDAV/Wi-Fi transfers.

---

## 5. Git & Submodule Notes

- Standalone repo: `xteink-os-antigravity-ui/` contains its own complete `.git/` directory, `.git/modules/freeink-sdk/`, and nested `.git/modules/.../lucide/`.
- Active branch: `experiment/antigravity-crosspoint-ui` (`origin` points to `https://github.com/Uturio99/xteink-os.git`, `upstream` points to `https://github.com/crosspoint-reader/crosspoint-reader.git`).
- Generated artifacts (`lib/I18n/I18nKeys.h`, `I18nStrings.*`, `*.generated.h`, `.pio/`, `build/`) are properly ignored via `.gitignore`.

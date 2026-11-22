# UX-01 — X4 Pro interaction audit

- **Base:** `63c9baea50f7183dc045affe8727551e90d2e425` (`chore/update-local-ignore`, Atelier baseline d779c0bd + ignore rules)
- **Date:** 2026-11-22
- **Scope:** read-only audit of the X4 Pro `480x800` portrait path (`AtelierChrome::isPortrait480x800`). Areas: A touch vs physical buttons, B bottom nav and stack depth, C hit targets ≥ 44 px. Contract: `.agents/rules/ui-activities.md` ("Shared UI and input", "Input frames and ownership", "Activity and popup transitions", verification matrix). No source files changed.
- **Evidence labels:** SOURCE_VERIFIED (file:line read), SIMULATOR_BEHAVIOR_VERIFIED (observed in `simulator_x4_pro` run), NOT TESTED (needs hardware or an unreachable flow).
- **Simulator artifacts:** `\\wsl$\Ubuntu\home\uturio\noveos-rec02b\_ux01\` (`nav-cycle.log`, `popup-nav.log`, BMP+PNG screenshots).
- **Script capabilities** (`.pio/libdeps/simulator_x4_pro/simulator/src/HalGPIO.cpp:354-493`): `TAP:x,y`, `SWIPE:x1,y1,x2,y2[,ms]`, named buttons `BACK/CONFIRM|ENTER/LEFT/RIGHT/UP/DOWN/P|POWER[:holdMs]`, `HOME[:holdMs]`, `PREV/NEXT[:holdMs]`, `SLEEP`, `SWITCH_ON/OFF`, `WAIT:<regex>` (log lines, per-segment timestamps must be increasing), `SHOT:<path>`, `QUIT`. No multi-touch; touch long-press only via SWIPE duration.

## A. Input map (X4 Pro 480x800)

`UiListActivity::loop()` order (UiListActivity.cpp:179-232): `handleCustomInput()` (popups/guards) → bottom-nav tap check (X4 Pro, tab ≥ 0) → `handleSwipeDeleteInput()` → `handleButtons()` → `routeListTouch()` (FUI row hits) → Up/Down swipe = viewport scroll → `navigateButtons()`.

| Activity | Back (button / header tap / edge swipe) | Confirm | Up/Down, Left/Right | Long-press | Touch |
|---|---|---|---|---|---|
| `LibraryListActivity` (UiTabListActivity, tab 1) | `handleBackAction()` ladder: clear query → restore groups → focus tabs → `onGoHome()` (LibraryListActivity.cpp:771-789, 873-876) | row → `activateIndex`; on tabs → `stepTab(1)` (873-885) | ring walk incl. tab band; hold = page jump in rows / tab step on band (888-933) | Confirm hold → recent options / delete / collapse / activate (858-871); touch row hold → `onRowLongPress` (341-349) | rows + tab hits via FUI; swipe-delete via `reveal` |
| `FileBrowserActivity` (tab 2, Books mode) | short release → up dir / `onGoHome()` at root / cancel in PickFirmware; hold ≥ GO_HOME_MS → jump to `/` (Books only) (557-602, 534-555) | `activateSelected` (564-567) | list nav + page jumps (base) | Confirm hold → `showEntryActions` (Books) (558-562); touch row hold → `onRowLongPress` (339-348); swipe-left → delete (350-355) | rows via FUI; swipe-delete reveal |
| `SettingsListActivity` (tab 3) | `onBackButton()` → `onGoHome()` (SettingsListActivity.cpp:118) | opens `SettingsActivity` via `startActivityForResult` → push (103-116) | list nav (base) | — | rows via FUI |
| `SettingsActivity` (tab 3, pushed child) | base `finish()` → pop to SettingsList | row → toggle / enum `optionPopup` / action (SettingsActivity.cpp:189-296) | list nav (base) | — | rows via FUI; `optionPopup` owns all input while active (`handleCustomInput` → `optionPopup.handleInput`, 211-213) |
| `WifiSelectionActivity` | `wasPressed(Back)` cancels per state (SCANNING aborts scan, CONNECTING aborts, NETWORK_LIST → `onComplete(false)`, FAILED/FORGET prompts) (597-778) | `wasReleased(Confirm)` selects/rescans (769-777); `wasPressed(Confirm)` skips auto-connect (602-621) | Up/Down+Left/Right move selection; Right = rescan; Left = forget prompt (658-796) | — | rows via `routeTouch` w/ long-press = forget; swipes scroll (794-820) |
| `EpubReaderActivity` (+`ReaderToolbarUi`) | `handleBackNavigation`: physical Back only — `wasBackGesture()` deliberately ignored on page surface (ReaderUtils.h:256-273); long ≥ GO_BACK_OR_HOME_MS → FileBrowser else Home (EpubReaderActivity.cpp:961) | opens toolbar/menu; long-press menu actions (863-908) | Left/Right/Up/Down page turns or overlay focus (ReaderUtils.h:66-84; 2978-3000) | SKIP_HOLD_MS → chapter skip/orientation (1032-1047) | link taps > menu gesture > page zones; toolbar/panel hits via FUI (`toolbarUi->route`, 2950) |
| `KeyboardEntryActivity` (pushed) | `wasReleased(Back)` → `onCancel()` (755-757) | type focused key; hold = repeat (719-753) | dpad moves key focus; Up/Down cycles (616-717) | — | key hits via FUI; cursor-position tap (572) |
| `ConfirmationActivity` (pushed) | `confirmPopup` owns everything; Back/outside tap → cancel + finish (ConfirmationActivity.cpp:61-73) | same | same | — | OptionPopup route |

Touch Back parity: `MappedInputManager::wasBackGesture()` folds the header-back tap (`HeaderBackTapTarget`, set by `AtelierChrome::drawStatusBar`/BaseTheme headers) and the left-edge swipe into `Button::Back` for **both** `wasPressed` and `wasReleased` (MappedInputManager.cpp:335-358, 387-412). A touch back therefore lands on the same handler as the physical Back release in every audited screen. `wasScreenTapped` explicitly excludes the header-back rect (MappedInputManager.cpp:200-208), so a back tap cannot also hit a row/key.

## B. Bottom nav / stack depth — verified mechanism

- Tab tap handling: `UiListActivity::loop` → `AtelierChrome::handleBottomNavTap` (UiListActivity.cpp:181-188; AtelierChrome.cpp:969-988). Tab 0 → `onGoHome(CONTINUE)` directly; tabs 1-3 set `sPendingBottomNavTab` then `onGoHome` → `ActivityManager::goHome` → `replaceActivity(HomeActivity)` (ActivityManager.cpp:361-376).
- **Replace, not push:** pending `Replace` destroys the current activity **and clears the whole stack** (ActivityManager.cpp:208-217). `goToLibrary/goToFileBrowser/goToSettings` are also `replaceActivity` (297-310). `HomeActivity::onEnter` consumes the pending tab via `takePendingBottomNavTab` (HomeActivity.cpp:242-256) and enters the target — confirmed at runtime: log shows `Exit Library → Entering Home → Exit Home → Entering FileBrowser` for every tab switch (`_ux01/nav-cycle.log`). **Stack cannot grow** through nav loops (Home→Library→Files→Settings→Home always re-creates Home). SIMULATOR_BEHAVIOR_VERIFIED.
- **Pending tab staleness:** `sPendingBottomNavTab` is written and `goHome` queued inside `currentActivity->loop()`; pending actions are processed in the same `ActivityManager::loop()` pass (ActivityManager.cpp:147-235) — no other activity loop can interleave. The only consumer is `HomeActivity::onEnter`, which runs unconditionally on entry. A nav tap cannot coexist with another pending action in one frame because the handler returns immediately (UiListActivity.cpp:185-189). No stale-pending path found. SOURCE_VERIFIED.
- **Overlay leak:** in-activity overlays (`OptionPopup`, `UiSliderDialog`, reader toolbar/panels) are reached via `handleCustomInput`, which runs **before** the bottom-nav check; an active popup consumes every touch/button frame (OptionPopup.h:76-147). Pushed overlays (`ConfirmationActivity`, `KeyboardEntryActivity`, `SettingsActivity`, `FrontlightPanelActivity`) own the loop themselves — the parent's nav check never runs. SIMULATOR_BEHAVIOR_VERIFIED: with the Display→"Sleep screen" enum popup open, `TAP:60,768` (Home nav zone) produced no `Entering activity`/`Popped` lines and the sheet stayed up (`_ux01/popup-nav.log` line ~57-68, `ux01-popup-navtap.bmp`).
- On the Home screen the nav is drawn through the FUI table instead (`drawHomeBottomNav` → `drawBottomNav(..., &screen, SELECT, tabActions)`, CoverGridHomeUi.cpp:62-66); the active tab's value is `-1` so tapping Home-on-Home is inert (AtelierChrome.cpp:955-958).

## C. Hit targets (X4 Pro 480x800, measured from source constants)

| Control | Hit rect | Drawn | Verdict |
|---|---|---|---|
| Bottom-nav tab | 4 × (120 × 64) at y=736-800 (AtelierChrome.cpp:949-958) | band y=740, h≈48 | PASS ≥44 |
| Atelier header "← BACK" | x≈rightX−30 → header right edge (~rightW+30 ≈ 60-70 px) × 46 (AtelierChrome.cpp:679) | arrow + "BACK" text | PASS ≥44 |
| Library tab bar slots | `slotW=416/count` × `barH=38` (AtelierChrome.cpp:744-746, 785) | 38 px band | **FAIL: height 38 <44** (UX-F01) |
| Keyboard keys (Atelier) | `keyH=52`, key width = `CONTENT_W` 416 / 10 cols − 4 px gap ≈ 38 px (KeyboardEntryActivity.cpp:1040-1046) | same | **FAIL: width ≈38 <44** on 10-key rows (UX-F02) |
| Option sheet 2-button row | capsule hits h=44 (AtelierChrome.cpp:1226-1244) | 44 | PASS |
| Option sheet rows | `rowH` 54 (≤6 options) / 46 (>6) (AtelierChrome.cpp:1248, 1293-1296) | same | PASS |
| UiSliderDialog (Atelier) | −/+ 44×44, confirm h=44, slider band 44 (UiSliderDialog.h:60-93) | same | PASS |
| Catalog trailing (search) icon | 44 × 40 at (right−44, 46) (CatalogScreens.cpp:28) | 16×16 icon | **FAIL: height 40 <44** (UX-F03) |
| Catalog cancel button | 164×44 (CatalogScreens.cpp:177) | same | PASS |
| Reader toolbar tool row | 3 slots, `slotW=416/3` × 52 (ReaderToolbarUi.cpp:355-359) | pills 40 h | PASS |
| Reader prev/next chapter capsules | 52×48 hits (ReaderToolbarUi.cpp:389-395) | **36×36** | hit OK, affordance <44 (UX-F04) |
| Reader scrub track | `trackW` × 44 (ReaderToolbarUi.cpp:399-401) | 12 px track + 5 px knob | PASS |
| Reader toolbar/panel dismiss | full 480×sheetY (ReaderToolbarUi.cpp:367, 417) | — | PASS |
| Editorial list rows | bodyH ≥56 (AtelierChrome.cpp:846) | same | PASS |
| Sleep "entering sleep" popup | **non-interactive banner** (y 132-184, SleepActivity.cpp:652-684) | — | n/a: no control to hit |

## Findings

| ID | Area | Sev | Trigger | Expected vs actual | Evidence | Label | Fix direction |
|---|---|---|---|---|---|---|---|
| UX-F01 | C | Med | Tap a Library tab near the band's top/bottom edge | All primary nav targets ≥44 px; slot hits are 38 px tall | AtelierChrome.cpp:744-746 (`barY`,`barH=38`), hit at 785 | SOURCE_VERIFIED | Extend each tab hit rect to ≥44 px tall (widen vertically, e.g. ±3 px into header rule/list gap) without changing the 38 px drawn band |
| UX-F02 | C | Med | Type on Atelier keyboard (e.g. Wi-Fi password, search) | Keys ≥44 px; 10-key rows are ≈38 px wide (52 px tall) | KeyboardEntryActivity.cpp:1040-1046 (`keyH=52`, `CONTENT_W` 416 / 10 cols, `gap=4`) | SOURCE_VERIFIED | Reduce to 9-key rows, shrink side gaps, or widen kbRect past `CONTENT_W` for Atelier; note non-Atelier boards share the same ~38 px width (out of scope) |
| UX-F03 | C | Low | Tap catalog search icon near its top/bottom edge | ≥44 px; hit is 44×40 | CatalogScreens.cpp:28 | SOURCE_VERIFIED | Raise hit height to 44 (y 46 → 44 px); drawn icon may stay 16 px |
| UX-F04 | C | Low | Read toolbar prev/next chapter buttons | Visual affordance should read ≥44 px; capsules drawn 36×36 though hits are 52×48 | ReaderToolbarUi.cpp:386-395 (`btnSize=36`, hits +16/+12) | SOURCE_VERIFIED | Enlarge drawn capsule to ~44, or keep and document as intentional small affordance with generous hit |
| UX-F05 | A | Med | In `FileBrowserActivity` PickFirmware mode, hold Back ≥1 s then release | A long-Back should act (cancel or up-dir); the release is consumed and nothing happens | Books-only guard at FileBrowserActivity.cpp:539-540; release consumed at 569-602 (`wasReleased(Back)` → `return true` even when `getHeldTime() ≥ GO_HOME_MS`) | SOURCE_VERIFIED | In `handleButtons`, fall through to short-Back handling (or an explicit no-op log) when mode ≠ Books and held ≥ GO_HOME_MS |
| UX-F06 | B | Low | Tap bottom nav while an `OptionPopup`/overlay is open | Design decision needed: nav tap is swallowed by the popup (tap on sheet chrome keeps it, outside dismisses); it does not switch tab | UiListActivity.cpp:179-189 order; OptionPopup.h:76-116; runtime run `_ux01/popup-nav.log` (no activity change after `TAP:60,768` with popup open) | SIMULATOR_BEHAVIOR_VERIFIED | Keep current "overlay owns input" behavior, or explicitly route nav-zone taps to `handleBottomNavTap` before the popup — PO/design call |

No High-severity findings: no double-handling between host and child (handleCustomInput-first ordering), no same-frame press+release double action (`wasBackGesture` synthesizes both edges but every screen consumes only one per state), no stack growth, no stale pending tab.

## Checked, no issue

- One action per input/edge in all audited activities (UiListActivity.cpp:143-232; per-activity overrides listed above).
- Back parity button vs touch: `wasBackGesture` folds header tap + left-edge swipe into `Button::Back` (MappedInputManager.cpp:335-412); Library's drawn back affordance calls the same `handleBackAction` (LibraryListActivity.cpp:795-797).
- Focus consistency touch↔buttons: single `nav.selected` field — tap writes `activeNav().selected = event.value` (UiListActivity.cpp:117-126), buttons move the same field; Library holds a second guard (`lockNextConfirmRelease`/`lockNextBackRelease`, 127-154) against held-release activation.
- Long-press release suppression is global (`wasLongPressed` → `suppressNextRelease` → `consumeSuppressedRelease` first thing in `ActivityManager::loop`, MappedInputManager.cpp:413-432, ActivityManager.cpp:106) — no long+short double fire.
- Bottom nav hits 120×64; header back ~60×46; option-sheet rows/buttons 44-54; slider controls 44; reader tool row 52, dismiss band and scrub hits ≥44; editorial rows ≥56.
- Swipe-delete reveal: Back/Confirm release or outside tap closes the reveal before any row action (UiListActivity.cpp:66-116).
- Reader page surface ignores the back **gesture** by design (left-edge swipe is a page-back zone there); physical Back still works and no back affordance is drawn on the reader surface (ReaderUtils.h:256-272).
- `HeaderBackTapTarget` is cleared on push/replace/exit (ActivityManager.cpp:221-222, 247-250) — a stale rect cannot eat taps on the next screen.
- Sleep "entering sleep" popup is informational only — no interactive control, nothing to hit (SleepActivity.cpp:652-684).

## NOT TESTED (needs hardware or unreachable flow)

- Physical buttons on device: real press/release edges, remap (`ButtonRemapActivity`), page buttons, Home key long-press, haptics.
- Real touch transform/ghosting, glove/moisture behavior, GT911 coordinate mapping on the panel.
- `FileBrowserActivity` PickFirmware mode live behavior (needs a `.bin` on SD and the SD-firmware-update flow) — UX-F05 is source-verified only.
- Popup outside-tap dismissal end state (script fired the tap; screenshot within the capture window was unchanged — source mechanism verified at OptionPopup.h:97-103, visual confirm pending).
- WifiSelection touch paths end-to-end (needs a scan result list), swipe-to-delete on a populated library/file list, `UiSliderDialog` live use (IntervalSelection/percent flows), `EpubReaderActivity` footnote/end-of-book overlay input on X4 Pro.
- Non-X4-Pro geometries (`env:simulator` 480×800 guard bypassed paths).

## Questions for the PO/design

1. Should a bottom-nav tap while a sheet/popup is open dismiss it, do nothing, or switch tab? Today it is swallowed by the overlay (UX-F06). Current behavior is safe; it just may feel inert.
2. Tab-bar 38 px band: acceptable to grow the hit rect a few px beyond the drawn band (into the 2 px rule/list gap), or should the band itself grow?
3. PickFirmware: what should long-press Back do — same as short (up-dir/cancel) or nothing?

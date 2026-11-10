# STEP 09 — Native integration and bounded delivery

**Reconciled 2026-11-09 against 7d63f480e779df702cc861437b331fccc960bb29.**
This replaces the stale implementation assumptions in the version at that commit; Git preserves the earlier plan. [Execution contract](../../EXECUTION_CONTRACT.md) governs operation. **No new firmware work is authorized by this document.** Existing code is evidence of implementation only; no acceptance or authorization is inferred from its presence.

**Consolidation reconciliation — 2026-11-10.** The canonical checkout is now `01878fe1`, which descends from `372ccca1`; the Library-return implementation and the pinned simulator are present, so do not restart either. This plan remains the native delivery path. The [consolidation campaign](../../superpowers/plans/2026-11-10-project-consolidation.md) governs workspace and branch consolidation only and authorizes no new native work.

## Current architecture: reuse, do not rebuild

| Outcome | Existing owner and current source evidence | Limit / required action |
|---|---|---|
| Root destinations and explicit Home | [ActivityManager](../../../src/activities/ActivityManager.cpp), goHome/replaceActivity/stack cleanup | Reuse root replacement for explicit Home and unrelated origins. No second navigation manager or persisted origin store. |
| Home / Resume / recent shelf | [HomeActivity](../../../src/activities/home/HomeActivity.cpp), [CoverGridHomeUi](../../../src/components/CoverGridHomeUi.cpp), RecentBooksStore, BookProgress | Existing Home work and dirty candidates are not accepted V2. Preserve stored identity and reliable progress; inspect candidate diffs before reuse. |
| Library → Reader → parent | [LibraryListActivity.cpp](../../../src/activities/library/LibraryListActivity.cpp):232–287, ReaderActivity::create, Activity::startActivityForResult | Commit 372ccca1 already creates Reader before closing CLIX, pushes with a one-shot result callback, and reopens/reconciles on return under RenderLock. Do not implement this again. |
| Reader Back | [ReaderUtils.h](../../../src/activities/reader/ReaderUtils.h):257–284, [ReaderActivity](../../../src/activities/reader/ReaderActivity.cpp):159–166 | Current return flag substitutes **every short logical Back**, even with backShortToFileBrowser=true. Recovered STEP 05 instead substitutes the Home-bound branch. Source mismatch against preserved mapping: correct only in an authorized package (G2). |
| Same-book sync / loan relaunch | [EpubReaderActivity](../../../src/activities/reader/EpubReaderActivity.cpp):1314–1369,1519–1554; [KOReaderSyncActivity](../../../src/activities/reader/KOReaderSyncActivity.cpp):104 | replaceActivity/goToReader still clear the retained parent. Source-level gap against NAV-001; runtime reproduction not run. Preserve parent via existing manager ownership only after approval. Different-book suggestion and explicit root exits intentionally end the visit. |
| Native rendering and input | [UITheme](../../../src/components/UITheme.h), [UIThemeTokens](../../../src/components/UIThemeTokens.h), BaseTheme, UiAppHost, UiTabListActivity, MappedInputManager, existing fui components | Reuse current owners. Header taps/left-edge gestures remain ignored by Reader's Back handler. No approved native visible contextual target is wired by 372ccca1. Preserve page-turn gestures and modal/input ownership. |
| Transfer and recovery | [UsbDriveActivity](../../../src/activities/network/UsbDriveActivity.cpp), [CrossPointWebServerActivity](../../../src/activities/network/CrossPointWebServerActivity.cpp), [CrossPointWebServer](../../../src/network/CrossPointWebServer.cpp), Settings, boot/crash owners | Preserve capability guards, exclusive SD ownership, failure cleanup and restart-to-Home. V2 Wi-Fi success requires completed transfer and rescan; it does not authorize new backend semantics. |

**NAV-001 target is APPROVED:** Library-origin contextual Back restores the visit, query, mode/direction, focused book and viewport; explicit Home remains independent. Recovered STEP 05 covers missing/reordered books, empty/no-match/degraded index, same-book detours, child-first input and independent Files/root routes. Latest [V2](../DESIGN-SYSTEM-V2-PROPOSAL.md) supplies the approved upper-left Back visual. The later approved upper-left placement supersedes the older off-page-only visual restriction; do not ask the PO to select the placement again. The native binding must preserve disabled page-touch/menu settings, child ownership and page swipes. Do not add an extra Back control.

Current callback performs rebuild/reset/selection work, but exact viewport, grouping, query and nearest-valid fallback have not been demonstrated. A missing-item nonblocking notice is an approved UX requirement, not established by the source-string test. Construction failure currently leaves the Library index open because allocation precedes close; do not retain the old plan's instruction to reopen it in that branch. Reader load failure after push must use normal child exit and callback cleanup. Measure peak retained Library plus Reader/children, including query/group storage.

## Recovered parity authority

[STEP 03](../../superpowers/audits/STEP-03-FEATURE-INVENTORY.md) contains the recovered **F01–F44** records; [STEP 04](../../superpowers/audits/STEP-04-ROUTE-TRACE.md) contains seven routes; [STEP 05](../../superpowers/audits/STEP-05-NAVIGATION-CONTRACT.md) maps those IDs and corrects guide/source discrepancies. These are original recovered IDs, not invented canonical replacements. They were untracked in another checkout, at baseline ad53eb645f27b76c8c4594fafe58479932234bf2. Provenance and hashes are in the [audit](../../superpowers/audits/EXECUTION-RECOVERY.md).

**VERIFIED:** the artifacts and 44 distinct records exist. **UNKNOWN:** complete PO acceptance as the canonical integration ledger, and current-head/device parity. Use the recovered records as a provisional working ledger pending G1; do not create another grouped inventory or claim 44/44. STEP 05's corrections take precedence over STEP 03's historical labels: three Library tabs, no Added tab; local single-item operations versus web bulk/move; root versus Settings plugin catalog; real progress/recovery behavior.

Fresh spot checks establish these predicates; they do not certify the entire ledger:

| Capability | Exact predicate / route evidence at current head |
|---|---|
| USB MSC | FREEINK_CAP_USB_MSC guards ActivityManager::goToUsbDrive and NetworkModeSelectionActivity; env:x4pro enables it. |
| OPDS | ActivityManager::goToBrowser chooses direct browser when server count == 1, otherwise picker; Settings server management remains available. |
| Plugins | SETTINGS.pluginsEnabled controls Home plugin routing; Settings opens the enable popup. A Settings catalog entry must use a child route, not root goToPlugins. |
| Reader touch / Back setting | BoardConfig::hasTouch gates touch settings; !hasTouch gates backShortToFileBrowser's setting row. Hiding that setting does not justify breaking stored/shared-board mappings. |
| Clock | halClock.isAvailable gates the Settings Clock entry; profile declaration is not proof the physical RTC probe succeeded. |
| Fonts / BLE | Vector font support requires CROSSPOINT_VECTOR_FONTS and usable PSRAM; standard x4pro does not extend the optional ble_page_turner mixin. Do not advertise unavailable capabilities. |

REC-01 adds one current-revision disposition table to the recovered STEP 03 file, keeping its original record descriptions. Each F-ID needs current owner/symbol, entry/return, exact build/runtime predicate, changed/unchanged/blocked disposition, regression case, existing test or NOT TESTED, and missing evidence. UNKNOWN routes or predicates block their affected integration until resolved; no feature silently disappears.

## Execution target and REC-02 evidence

The published REC-01 anchor is **17773ef2100e4e0f51ebbff66d86ce58ddb1ec98**. The canonical checkout is `D:\Work\AI\XTEInk X4 Pro\xteink-os` at `01878fe1`; the 2026-11-10 consolidation campaign owns workspace and branch reduction, so do not treat `feat/noveos-native` or an external REC-01 worktree as a second execution workspace. The REC-01 checkpoint remains the immutable comparison ref. Historical Home/font/parser worktrees and the Phase 03-P0 recovery vault remain candidate evidence.

The SDK gitlink and its own checkout match **946e6ac3d7a9d590ba3224428e6183d55ac7917c**, with nested Lucide at **c81680e066f45b640743ca78ae36cdedda3f0318**. Keep simulator **6730fe5fe576c9d924e44096a9e267d8538d184c** and JPEGDEC **86282979224c8a32fd51e091ed5a35b0c699a52b** pinned. X4 Pro uses ESP32-S3, native 800×480 mapped to portrait, GT911, SDMMC and a distinct Home input. Confirm panel and USB/VBUS behavior on the actual unit.

The November 10 REC-02 addendum recorded a build and visible interaction on the existing empty Cover Grid Home, with an uncommitted simulator compatibility/EOL overlay. It recorded firmware failure at JPEGDEC 0002. Keep that historical evidence separate from new results. Use the canonical program directly under WSL/X11 with an isolated CROSSPOINT_SIM_SD. The launcher in the older workspace points at H1.1B, and the PlatformIO launch wrapper stalled scanning the Windows mount.

Report **SOURCE_VERIFIED**, **BUILD_VERIFIED**, **SIMULATOR_RENDER_VERIFIED**, **SIMULATOR_BEHAVIOR_VERIFIED**, **HARDWARE_VERIFIED**, **BLOCKED** or **NOT_TESTED** per case. Record revision, exact diff, profile, command or input scenario, and evidence. The simulator replaces the hardware and omits USB MSC. Host or simulator evidence does not close physical input, storage, heap, E-Ink or recovery gates.

## Gates and shortest safe path

| Gate | Owner / exact closure condition |
|---|---|
| G1 — pre-integration parity | REC-01 resolves the 44 current-source rows; PO identifies/accepts the recovered ledger or explicitly adopts it as replacement and dispositions all remaining pre-integration evidence gaps under STEP 06. This audit does not weaken STEP 06's parity prerequisite or count source equality as runtime parity. If that gate requires measurements before code, obtain them on the existing candidate in REC-02. |
| G2 — implementation provenance and permission | Record scope-specific authorization/acceptance for existing 372ccca1/2dbf8155 if available, and explicit permission for the bounded verification/correction package. The preservation contract supplies the expected logical mapping: default short→Library/long→Files; reversed short→Files/long→Library for a Library parent, while explicit Home stays independent. Upper-left V2 Back is already approved. Missing permission stays UNKNOWN; any contrary approval must be identified before changing that behavior, not invented. Firmware handoff still requires human acceptance of the concrete implementation/ownership. |
| G3 — candidate/environment/resource | Pin chosen clean candidate/diff, SDK and simulator/toolchain; separate dirty Home/font experiments. Obtain explicit authorization for bounded baseline verification, diagnostic changes if needed, and any device/flash action. Measure the existing Library-origin and root-open paths on X4 Pro before REC-03/B. Repeat the resource and SD lifetime checks on the corrected candidate before acceptance. |
| G4 — native fonts/visual acceptance | Reuse M2-WP1 feasibility and existing font-gate experiment; PO selects native face/roles only after current glyph/layout/fallback and linked flash evidence. Explicitly set or accept measured flash reserve and memory budget; do not invent numeric thresholds. V2 visual direction itself stays settled. |
| G5 — product acceptance | All adopted capability rows and affected journeys have applicable source/host/build/simulator/device evidence; no unresolved safety or parity failure; firmware-handoff and explicit human acceptance complete. Publishing/flashing remain separately authorized. |

**Critical path:** REC-02 builds and baseline parity → G1 disposition → REC-04/H first interactive Home → REC-03/A → REC-03/B → REC-04/R and REC-05 → REC-06. G3 baseline device work can proceed on the saved build-only image. G4 final font and visual evidence applies to REC-04 completion. G1 remains a prerequisite for any UI/NAV integration. Obtain the relevant package approval before editing.

### Native coverage by checkpoint

| Checkpoint | V2 template | Shared styling | Preserved with reason |
|---|---|---|---|
| REC-02 | — | — | Existing baseline for comparisons. |
| REC-04/H | Home | — | Destination screens and transfer chooser retain current behavior while the real Home is exercised. |
| REC-03/A–B, REC-04/R | Library and Reader at REC-04/R | Reader controls and feedback where approved | Existing Reader engines and input ownership. |
| REC-05 | Settings and Import | File Browser, OPDS, Plugins, dialogs and feedback where approved | Service-specific views without an approved template. |
| REC-06 | Review affected templates across the four journeys | Review shared states in those journeys | Startup splash, boot identifiers, crash and recovery under BRAND-001 and existing safety contracts. |

Use the STEP 03 ledger for capability outcomes and this table for visual disposition. Keep CrossPoint attribution. The app-controlled startup splash still shows CrossPoint under [BRAND-001](../step07b/STEP-07B.1R-DECISIONS.md). Any later splash change needs separate approval and belongs to BootActivity and its assets. Bootloader, partitions, crash and recovery have separate owners.

### Device entry, measurements and recovery

- Before the first baseline or candidate flash, obtain separate PO authorization for the exact image and unit. Confirm panel/profile, image hash, the application-only write path, a known-good recovery image and a working route to recovery. Preserve device data and use spare SD media. Stop if recovery is unproven.
- For NAV, compare Library-origin and root-open on the same baseline build, book and card before REC-03/B. Record internal heap, PSRAM minima and largest blocks, SD handle lifetime and repeated entry/return/child behavior. Measure the corrected path before accepting it.
- Compare Home baseline and candidate with matched fixtures, settings, orientation and cache state. Use empty, populated cold/warm and missing-book cases, with EPUB and XTC where the paths differ. Measure time to usable Home, destination response, time until covers settle, index/thumbnail work and requested refresh count/type. On X4 Pro add physical response, ghosting, internal/PSRAM minima and largest blocks, and SD activity over repeated Home/Reader transitions. Use existing logs and captures first. Investigate reproducible degradation, repeated thumbnail work, stale pixels or shrinking headroom. Set reserves from evidence and PO disposition.
- Keep one known-good base, scoped diff and artifact hashes per accepted logical change. Stop before writes if refs, pins or dirty paths differ. Recover owned files from recorded before-images or use a separately authorized revert commit. Preserve other WIP. Stop device tests on ownership conflict, boot failure or crash loop and use the approved recovery procedure.
- Use inexpensive checks at internal checkpoints and four independent read-only review axes on each frozen firmware/build-affecting logical diff. Keep the two-round cap. Rerun only checks invalidated by a change. Use a low-cost builder for bounded work and experienced embedded review for lifecycle, storage and recovery. Human acceptance and publication authorization are separate.

## Work-package contracts

These statuses are the recovery snapshot, not invented Beads issue IDs. Changes to status belong in active Beads when available; package specifications remain here.

### REC-00 — Execution recovery

| Field | Contract |
|---|---|
| Objective | One discoverable, source-backed execution authority. |
| Status | DONE — documentation deliverable only; checks in the recovery audit. |
| Dependencies | Supplied recovery request; existing repository evidence. |
| Authority | Current PO recovery scope and EXECUTION_CONTRACT. |
| Scope | AGENTS/routing rule, project map/status, design authority links, STEP 06/09, recovered STEP 03–05, audit. |
| Implementation | Reconcile stale instructions; retain provenance and approval boundaries. |
| Acceptance | One next task; no active baseline conflict; no firmware/SDK/pipeline changes; usable references. |
| Verification | Final diff, links/paths, dependency/field checks, provenance hashes, original WIP preservation. |
| Exclusions | Firmware changes, new UX, commits/push/flash/cleanup. |
| Completion evidence | Recovery audit with actual commands/results and unverified limits. |

### REC-01 — Reconcile the recovered ledger to current source

| Field | Contract |
|---|---|
| Objective | Give the PO a complete, current F01–F44 preservation map without reconstructing history. |
| Status | DONE — 44/44 current-head disposition rows appended to STEP 03 (source-verified; runtime unverified). G1 adoption and G2 authorization remain PO actions. |
| Dependencies | REC-00; recovered STEP 03–05 and baseline/current Git objects exist; pinned SDK readable in original checkout. |
| Authority | STEP 03 IDs + STEP 05 corrections; current V2/STEP 06; G1. |
| Scope | Append current disposition table to STEP-03-FEATURE-INVENTORY.md; link gate results from this plan. Read changed owners/callers, SettingsList, format/service/boot guards and pinned SDK as needed. |
| Implementation | Compare ad53eb64..7d63f480 (26 source/build/test files, including generated fonts). Reuse unchanged owner evidence; trace reachable changed callers. Preserve original IDs/descriptions and mark corrected interpretations. Account for uncommitted candidates separately. |
| Acceptance | Exactly F01–F44, each with owner, route/return, exact predicate, change disposition, observable regression case and evidence layer. No unknown hidden behind PASS; every gap has owner and closure condition; PO adoption request is concrete. |
| Verification | Git delta plus targeted source checks; count unique IDs; verify file/symbol references and baseline-vs-current labels. No firmware build required. |
| Exclusions | Firmware/test/pipeline edits, new features/IDs, parity acceptance, hardware claims, repeated full-history audit. |
| Completion evidence | Updated 44-row table; affected routes; exact unresolved G1 evidence and G2 authorization provenance; source checks and NOT TESTED runtime cases. |

### REC-02 — Build reproducibility and baseline verification

| Field | Contract |
|---|---|
| Objective | Produce reproducible x4pro and simulator_x4_pro artifacts, then establish the baseline evidence needed for G1 and REC-03/B. |
| Status | Build follow-up in progress. G1 disposition and device authorization/evidence remain open. |
| Build scope | Preserve the three simulator compatibility calls and normalized SimLog/0001 bytes. Normalize 0002 and 0003 to LF. Set text eol=lf for the simulator patch and JPEGDEC patch series. Keep all hunk bodies and dependency pins. |
| Build verification | Preserve byte-verified before-images. Check ordered apply, already-applied detection and reverse on pinned LF fixtures. Build x4pro and simulator_x4_pro with the recorded WSL PlatformIO setup. Save hashes, visible direct launch and Home→Library→Home input/transition evidence. Stop on pin drift, semantic patch change or failure after one bounded correction. |
| Baseline verification | Reuse valid evidence. Exercise STEP 03/05 cases needed for G1. Record source-verified Back and sync/loan mismatches separately from measured baseline outcomes. Capture the same-binary Library-origin/root-open memory and SD comparison after device entry approval. |
| Exclusions | Firmware logic, dependency upgrades, general harness, UI redesign, destructive testing or unapproved flash. |
| Exit | Both build artifacts and visible simulator smoke recorded for the build checkpoint. Keep baseline/device verification open until its required evidence is available. |

### REC-03 — Finish Library → Reader → contextual Back

| Field | Contract |
|---|---|
| Objective | One usable native NAV-001 vertical slice with independent Home and preserved reading functions. |
| Status | A is BLOCKED on H, G1/G2 and scoped authorization. B also needs A, G3 baseline device evidence and scoped B authorization. |
| Dependencies | G1/G2 and package authorization for A. A plus G3 baseline resource/SD evidence for B. |
| Authority | V2 contextual Back and STEP 06 NAV-001; STEP 05 lifecycle, with later V2 visual precedence and scoped G2 authorization. |
| Scope | LibraryListActivity, ReaderActivity/ReaderUtils, ActivityManager only if needed, EPUB sync/loan relaunch, existing Reader chrome/overlay/input owners and focused tests. |
| Implementation | A corrects the Home-bound branch in ReaderUtils and replaces the assertion that encodes the defect. B preserves the Library visit and one-shot callback through current-only same-book sync/loan replacement in ActivityManager, with at most one live Reader. Preserve Wi-Fi cleanup and suppress reboot only for the retained detour, including explicit Home teardown. On reconstruction failure pop normally and reconcile Library. Bind the approved upper-left Back after lifecycle verification. |
| Acceptance | Test both logical mappings and other origins. Preserve tab/direction/query, identity, focus and viewport. Recover from removed/reordered books, index/load/construction failures and show the approved notice. Test sync success/cancel/failure/retry, loan, Home during sync, chapterless XTC, disabled page touch/menu, modal/footnote precedence and one action per input. Verify candidate device headroom and SD lifetime before acceptance. Stop for a scope decision if the retained in-process detour is unsafe. |
| Verification | Production-path regression cases; reused library/index/input tests plus smallest real seam tests for changed logic; simulator where supported; relevant builds and four-axis firmware handoff; human X4 Pro memory/input/SD/orientation checks. |
| Exclusions | Home/Settings restyling, broad Reader rewrite, new gestures, snapshot manager, unrelated origin changes. |
| Completion evidence | Case matrix, tests/builds/reviews/device evidence, state/resource ownership and explicit PO behavior/architecture acceptance. No source-only DONE. |

### REC-04 — V2 reading surface: Home, Library and Reader presentation

| Field | Contract |
|---|---|
| Objective | Open/resume/read/return using approved native V2 composition and legible Vietnamese. |
| Status | BLOCKED for H on reproducible builds, G1 disposition and named Home scope. R depends on REC-03. G4 applies to final REC-04 acceptance. |
| Dependencies | H uses existing font IDs and routes. R uses the accepted NAV slice and requires glyph/fallback/flash evidence and PO selection before adopting new fonts. |
| Authority | V2, Hybrid demo, reconciled STEP 06; existing UI ownership and font rules. |
| Scope | Existing Home/CoverGridHomeUi/X4ProHomeLayout, Library/Reader drawing, UITheme/ThemeMetrics/token bridge and font assets only as approved. |
| Implementation | H adapts the five-file Home candidate through HomeActivity, UITheme and the active X4 Pro Cover Grid draw/input path. Keep existing progress, catalog, route callbacks and transfer behavior. Verify and freeze H separately. R finishes Library and Reader presentation and adopts only approved, measured font assets. |
| Acceptance | H visibly renders empty, populated and missing-book Home in the canonical simulator. Touch and physical-equivalent input open/resume real fixture books and reach Library, Files, Settings and the existing transfer chooser with OPDS/plugins on and off. Verify returns, honest progress and matched cold/warm comparisons. R proves approved rows/tabs/Back, long NFC/NFD text, fallback, hit geometry and redraw, then closes device and G4 gates. |
| Verification | Existing layout/Unicode suites, linked font footprint, native captures/interaction, relevant builds/reviews; human four-orientation input/readability/refresh/resource checks and PO visual acceptance. |
| Exclusions | Reopening V2 direction, evaluating endless new font families, new catalog/progress database or renderer. |
| Completion evidence | Native before/after for affected states, font decision/budget, cases and all applicable handoff evidence. |

### REC-05 — Settings and Import through existing services

| Field | Contract |
|---|---|
| Objective | Reach preserved services and safely import books from the approved native surfaces. |
| Status | BLOCKED — G1/G2/G3 and separately scoped firmware authorization; independent of REC-04 cosmetics. |
| Dependencies | Adopted current ledger, known native navigation/input contract; any shared component actually used must be available. |
| Authority | V2 Settings/Import, STEP 06 Journeys C/D, recovered STEP 05 and existing service/storage contracts. |
| Scope | Settings section/list, File Browser, NetworkModeSelection/CrossPointWebServer/UsbDrive activities, existing OPDS/plugin routes and shared UI. Backend changes need explicit scope. |
| Implementation | Bind approved presentation to existing capability-gated owners; preserve all routes and return context; completed Wi-Fi write/close/rescan before success; USB remains exclusive and restarts Home after safe teardown. |
| Acceptance | Empty Home can import; unsupported/offline/auth/abort/duplicate/corrupt cases preserve content; Settings child returns to origin; OPDS 0/1/many and plugins disabled/enabled retain routes; USB never offers unsafe navigation while host owns SD. |
| Verification | Relevant host/service tests; source route/guard mapping; build and firmware handoff; human actual transfer/eject/remount/restart/recovery evidence. Simulator only proves supported UI transitions. |
| Exclusions | New service framework, altered formats, storage ownership shortcuts or direct USB→Library. |
| Completion evidence | Affected F-ID cases, data/handle lifecycle and device outcomes; PO acceptance for any approved behavior changes. |

### REC-06 — Whole-product acceptance and handoff

| Field | Contract |
|---|---|
| Objective | A usable, validated X4 Pro NoveOS candidate with preserved CrossPoint functionality. |
| Status | BLOCKED — accepted REC-03/04/05 and G5 evidence. |
| Dependencies | Adopted ledger; final candidate/diff; human X4 Pro access and authorized non-destructive verification. |
| Authority | Execution contract, all approved product decisions and mandatory firmware-handoff. |
| Scope | Final candidate, all F01–F44 capability/guard cases, four journeys, boot/recovery/storage and release evidence. |
| Implementation | Close remaining verification gaps; localize discovered defects to owning packages. Reuse valid earlier evidence; do not repeat unchanged reviews/builds. |
| Acceptance | Close every applicable F01–F44 route/guard/outcome and the native visual dispositions above. Reading/progress/resume, Vietnamese, input, panel/refresh, resources, storage, network and recovery meet approved checks. Record the accepted boot branding limitation and G5 human acceptance. |
| Verification | Target/shared-board builds as affected; host regressions, supported simulator scenarios and human device cases with exact revision/profile/card. Full recovery/USB checks only at relevant boundaries, never waived. |
| Exclusions | Automatic flash, commit, merge, push, PR or release publication. |
| Completion evidence | Final evidence matrix and human behavior/design/maintenance acceptance; separate release authorization remains required. |

## Existing tests, CI and evidence limits

Reuse test/library_index_file, library_builder, library_format, library_text, utf8_compose, combining_marks, button_press_navigation, home_button, x4pro_home_layout and vietnamese_font_coverage as relevant. Their presence is not a passing run. test/test_library_reader_return.py is registered as library_reader_return_contract; it checks source strings and a Python list model, **not the production ActivityManager lifecycle**. It cannot certify touch, actual viewport restoration, sync retention, SD handles or MCU allocation failure.

CI already has formatting, cppcheck, eight board builds (including x4pro/default), CMake/CTest and an aggregate Test Status job. No current CI run was retrieved or certified by this audit. No simulator or browser job is defined there.

**PROPOSED minimal CI changes, not implemented here:** when REC-03 adds focused production-seam tests, register them in existing CTest; ensure their dependency libraries (for example Expat, required by parser suites) are installed in the existing unit-test job. Add a simulator smoke job only after REC-02 establishes a reproducible command and a concrete missing failure it detects. A documentation link check can reuse the recovery check if drift recurs; no new QA framework. CI cannot grant PO acceptance or pass hardware gates.

## Current handoff

The first authorized builder scope is REC-02 build reproducibility from the canonical checkout (`01878fe1`), not a separate `feat/noveos-native` workspace. G1 disposition and a separately authorized Home scope still gate REC-04/H. G2 gates REC-03/A, and G3 baseline device evidence gates REC-03/B. Device actions, commits and publication require their own authorization.

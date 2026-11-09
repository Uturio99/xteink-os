# STEP 09 — Native integration and bounded delivery

**Reconciled 2026-11-09 against 7d63f480e779df702cc861437b331fccc960bb29.**
This replaces the stale implementation assumptions in the version at that commit; Git preserves the earlier plan. [Execution contract](../../EXECUTION_CONTRACT.md) governs operation. **No new firmware work is authorized by this document.** Existing code is evidence of implementation only; no acceptance or authorization is inferred from its presence.

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

## SDK and simulator evidence

Parent SDK gitlink is **946e6ac3d7a9d590ba3224428e6183d55ac7917c**. The refoundation and font-gate Windows SDK directories were empty during this audit; running Git there returned the parent repository. The initialized, clean SDK under the original xteink-os worktree matched the gitlink and supplied read-only BoardConfig evidence. Before native work, initialize/verify the chosen checkout at the existing pin, never update it.

Pinned BoardConfig's X4 Pro profile declares ESP32-S3, 800×480 panel mapped to portrait, GT911, Left/Right/Power plus capacitive Home, SDMMC and RTC/frontlight. Panel mount and USB/VBUS detection remain explicitly unconfirmed in that source. Device observations still belong to the actual PO unit. Use HalStorage/HalFile locking and HalMemory internal/PSRAM/largest-block accounting; C3's limits do not describe X4 Pro headroom.

| Simulator state | Current evidence |
|---|---|
| Configured | **VERIFIED**: 2dbf8155, platformio.ini env:simulator and env:simulator_x4_pro; simulator pin 6730fe5fe576c9d924e44096a9e267d8538d184c; host-only compatibility script/patch. |
| Compiled | **UNKNOWN for this exact clean revision**: no matching build log was recovered in inspected Windows checkouts. Separate font experiment reports are not clean-head evidence. |
| Launched | **UNKNOWN for this exact clean revision**: launch instructions exist; no reproducible run record recovered. |
| Native behavior tested | **NOT TESTED by this audit**. No activity lifecycle acceptance follows from configuration. |
| Physical X4 Pro verified | **NOT TESTED**. Simulator shims cannot prove panel, physical input, MCU memory, real USB/SD/Wi-Fi or boot recovery. |

Use [existing Linux/WSL setup and launch instructions](../../contributing/getting-started.md#x4-pro-desktop-simulator-linuxwsl2) when an authorized package needs it. Do not create another simulator or require a new simulator project before NAV work. Source/host checks remain useful if simulator runtime is unavailable; mandatory device acceptance remains open.

## Gates and shortest safe path

| Gate | Owner / exact closure condition |
|---|---|
| G1 — pre-integration parity | REC-01 resolves the 44 current-source rows; PO identifies/accepts the recovered ledger or explicitly adopts it as replacement and dispositions all remaining pre-integration evidence gaps under STEP 06. This audit does not weaken STEP 06's parity prerequisite or count source equality as runtime parity. If that gate requires measurements before code, obtain them on the existing candidate in REC-02. |
| G2 — implementation provenance and permission | Record scope-specific authorization/acceptance for existing 372ccca1/2dbf8155 if available, and explicit permission for the bounded verification/correction package. The preservation contract supplies the expected logical mapping: default short→Library/long→Files; reversed short→Files/long→Library for a Library parent, while explicit Home stays independent. Upper-left V2 Back is already approved. Missing permission stays UNKNOWN; any contrary approval must be identified before changing that behavior, not invented. Firmware handoff still requires human acceptance of the concrete implementation/ownership. |
| G3 — candidate/environment/resource | Pin chosen clean candidate/diff, SDK and simulator/toolchain; separate dirty Home/font experiments. Obtain explicit authorization for bounded baseline verification, diagnostic changes if needed, and any device/flash action. Human X4 Pro evidence must establish parent+Reader/child headroom and SD lifetime before retained-parent changes are accepted. No MCU memory values inferred from simulator. |
| G4 — native fonts/visual acceptance | Reuse M2-WP1 feasibility and existing font-gate experiment; PO selects native face/roles only after current glyph/layout/fallback and linked flash evidence. Explicitly set or accept measured flash reserve and memory budget; do not invent numeric thresholds. V2 visual direction itself stays settled. |
| G5 — product acceptance | All adopted capability rows and affected journeys have applicable source/host/build/simulator/device evidence; no unresolved safety or parity failure; firmware-handoff and explicit human acceptance complete. Publishing/flashing remain separately authorized. |

**Critical path:** REC-01 → G2 and bounded REC-02 authorization → REC-02 → G1 final disposition → REC-03 → REC-04 → REC-06, with REC-05 joining before REC-06. G1/G2 records and existing font evidence collection can proceed independently of one another. REC-05 does not require new Home cosmetics; it needs the common gates and its own approved route scope. Simulator diagnosis can run alongside ledger work when separately assigned; it is not a substitute for device gates. Defer additional browser prototypes, other-device redesign, speculative caches and CI expansion. Shared-board regressions and recovery cannot be deferred.

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

### REC-02 — Validate the existing native candidate

| Field | Contract |
|---|---|
| Objective | Establish whether the existing retained-parent implementation is a safe base to finish. |
| Status | BLOCKED — G3 verification/device authorization and reproducible candidate required. |
| Dependencies | REC-01 evidence for affected F03/F05/F20/F28/F30/F31/F43/F44; the preservation input matrix above; G3 setup and authorization. |
| Authority | NAV-001, STEP 05 lifecycle, existing resource/input rules; no new UX. |
| Scope | Existing Library/Reader/ActivityManager paths and named tests; initialized pinned SDK; simulator_x4_pro; human X4 Pro observations. Diagnostic modifications only if explicitly included in its authorization. |
| Implementation | Run existing tests; build/launch the pinned candidate as authorized; execute normal/filtered/reordered/missing-index/missing-book/child/sync/Home cases. Compare replacement baseline and retained-parent memory on the same card/book. |
| Acceptance | Reproducible results for each case, known mismatches localized, no false success. Internal/PSRAM free/minimum/largest blocks and SD lifetime recorded through load/children/return/repeat; passing resource budget reviewed before native continuation. A failed behavioral case is evidence for REC-03, not acceptance of that behavior. |
| Verification | Existing host suites + actual native transitions where supported; human device evidence for resources/input/storage. If no production lifecycle test seam exists, report NOT TESTED rather than create a generic framework. |
| Exclusions | Visual redesign, general test harness, dependency updates, destructive testing or unapproved flash. |
| Completion evidence | Candidate/SDK hashes, commands/logs, separate build/launch/scenario/device results and bounded defect list. Unknown required resource evidence leaves REC-02 incomplete. |

### REC-03 — Finish Library → Reader → contextual Back

| Field | Contract |
|---|---|
| Objective | One usable native NAV-001 vertical slice with independent Home and preserved reading functions. |
| Status | BLOCKED — G1/G2/G3, REC-02 and explicit bounded firmware authorization. |
| Dependencies | REC-01 and REC-02; approved native input binding; retained-parent resource evidence. |
| Authority | V2 contextual Back and STEP 06 NAV-001; STEP 05 lifecycle, with later V2 visual precedence and scoped G2 authorization. |
| Scope | LibraryListActivity, ReaderActivity/ReaderUtils, ActivityManager only if needed, EPUB sync/loan relaunch, existing Reader chrome/overlay/input owners and focused tests. |
| Implementation | Correct only demonstrated gaps in existing 372ccca1. Wire the approved visible contextual action for EPUB and XTC; preserve parent through same-book detours, state and failure recovery. No duplicate navigation state or new UI framework. |
| Acceptance | All tabs/directions and active query retain book/viewport; reordered/removed/empty/failed-index cases recover with approved notice; chapterless XTC and disabled page-touch settings still have approved exit; child/footnote Back is consumed first; both logical mappings match the preservation matrix; one gesture produces one action; explicit Home/Files ends visit; load failure leaves usable Library. |
| Verification | Production-path regression cases; reused library/index/input tests plus smallest real seam tests for changed logic; simulator where supported; relevant builds and four-axis firmware handoff; human X4 Pro memory/input/SD/orientation checks. |
| Exclusions | Home/Settings restyling, broad Reader rewrite, new gestures, snapshot manager, unrelated origin changes. |
| Completion evidence | Case matrix, tests/builds/reviews/device evidence, state/resource ownership and explicit PO behavior/architecture acceptance. No source-only DONE. |

### REC-04 — V2 reading surface: Home, Library and Reader presentation

| Field | Contract |
|---|---|
| Objective | Open/resume/read/return using approved native V2 composition and legible Vietnamese. |
| Status | BLOCKED — REC-03, G4 and named firmware/visual implementation authorization. |
| Dependencies | Accepted NAV slice; current native font/fallback/flash evidence and PO selection. |
| Authority | V2, Hybrid demo, reconciled STEP 06; existing UI ownership and font rules. |
| Scope | Existing Home/CoverGridHomeUi/X4ProHomeLayout, Library/Reader drawing, UITheme/ThemeMetrics/token bridge and font assets only as approved. |
| Implementation | Reuse selected evidence-backed portions of dirty Home/font candidates, one coherent reading journey. Make only necessary owner/token changes; keep progress/catalog/reader engines. |
| Acceptance | Populated/empty/missing-book Home; honest progress; approved rows/tabs/Back and navigation; long NFC/NFD titles/Reader text without lost marks or clipping; glyph fallback works; safe hit geometry and redraw. |
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
| Acceptance | Every applicable capability retains a reachable route and correct result; unavailable profile capabilities are documented correctly. Reading/progress/resume, Vietnamese, input, panel/refresh, resources, storage, network and recovery meet approved checks. No safety/parity blocker or missing mandatory human acceptance. |
| Verification | Target/shared-board builds as affected; host regressions, supported simulator scenarios and human device cases with exact revision/profile/card. Full recovery/USB checks only at relevant boundaries, never waived. |
| Exclusions | Automatic flash, commit, merge, push, PR or release publication. |
| Completion evidence | Final evidence matrix and human behavior/design/maintenance acceptance; separate release authorization remains required. |

## Existing tests, CI and evidence limits

Reuse test/library_index_file, library_builder, library_format, library_text, utf8_compose, combining_marks, button_press_navigation, home_button, x4pro_home_layout and vietnamese_font_coverage as relevant. Their presence is not a passing run. test/test_library_reader_return.py is registered as library_reader_return_contract; it checks source strings and a Python list model, **not the production ActivityManager lifecycle**. It cannot certify touch, actual viewport restoration, sync retention, SD handles or MCU allocation failure.

CI already has formatting, cppcheck, eight board builds (including x4pro/default), CMake/CTest and an aggregate Test Status job. No current CI run was retrieved or certified by this audit. No simulator or browser job is defined there.

**PROPOSED minimal CI changes, not implemented here:** when REC-03 adds focused production-seam tests, register them in existing CTest; ensure their dependency libraries (for example Expat, required by parser suites) are installed in the existing unit-test job. Add a simulator smoke job only after REC-02 establishes a reproducible command and a concrete missing failure it detects. A documentation link check can reuse the recovery check if drift recurs; no new QA framework. CI cannot grant PO acceptance or pass hardware gates.

## Next-agent handoff — REC-01 only

Copy-ready instruction — **executed 2026-11-09; retained for provenance** (completion recorded in the STEP 03 REC-01 section and PROJECT_STATUS):

> Execute REC-01 only. Use the recovery worktree with base 7d63f480e779df702cc861437b331fccc960bb29 plus its uncommitted documentation changes; verify the current diff first. Read AGENTS.md, docs/EXECUTION_CONTRACT.md and REC-01 here. Reuse recovered STEP 03 IDs, STEP 05 corrections and the current NAV findings above. Compare ad53eb645f27b76c8c4594fafe58479932234bf2..7d63f480e779df702cc861437b331fccc960bb29; inspect only affected owners/callers and exact guards. The original Windows checkout G:/AI-Setup/XTEInk-X4-Pro/xteink-os/freeink-sdk was verified clean at 946e6ac3d7a9d590ba3224428e6183d55ac7917c; recheck identity before using its source, never assume an empty SDK directory is initialized. Append the current 44-row disposition table in docs/superpowers/audits/STEP-03-FEATURE-INVENTORY.md, preserving historical text/IDs and marking unresolved evidence. Supply a concrete G1 adoption/disposition packet and the G2 mismatch; do not reopen settled V2 choices. Use Beads if an active workspace exists; otherwise report tracking unavailable without inventing an issue. Run link/ID/source-reference/diff checks. No firmware/test/pipeline edits, builds, commits, push, flash or cleanup. Report exact checks, gaps and the next gate; stop.

No firmware builder instruction is ready. The smallest subsequent PO action is to adopt/disposition the REC-01 ledger and record G2 scope/authorization, then authorize the bounded REC-02 evidence work on a named candidate. Do not ask for blanket native integration approval.

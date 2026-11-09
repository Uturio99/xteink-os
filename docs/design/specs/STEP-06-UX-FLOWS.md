# XTEInk OS — STEP 06 UX Flows & Controls

> **Status:** Behavioral contract reconciled with later approved V2 decisions, 2026-11-09; native implementation and acceptance remain separate.
> **Provenance:** Initially reconstructed without STEP 03–05. Those source artifacts were recovered from another worktree during the [recovery audit](../../superpowers/audits/EXECUTION-RECOVERY.md). Their baseline claims are not current native/device acceptance.
> **Scope:** UX flows and controls only. No firmware, SDK, prototype, build, or dependency changes.
> **Target:** Xteink X4 Pro, logical portrait viewport 480×800, monochrome E-Ink, Vietnamese Unicode.

## 1. Evidence boundary

The following evidence is verified locally:

- [BASELINE](../BASELINE.md) indexes current NoveOS V2 / Hybrid Editorial Index authority and historical H1.1A/M1 references; it prohibits unapproved promotion of experiments.
- [V2](../DESIGN-SYSTEM-V2-PROPOSAL.md) supplies current visual/interaction decisions. UNIFIED_UI_COLLECTION.md preserves the historical 15-frame review; its obsolete typography, tabs and gesture proposals do not override V2.
- `docs/design/specs/M2-WP2-HOME-PLAN.md` records the existing Home implementation, the existing navigation conflict, and the requirement to preserve the existing activity/`fui::` stack.
- `docs/superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md` and `docs/superpowers/audits/M1-UI-COLLECTION-QA.md` record the prior prototype reconciliation and browser QA claims. They are evidence of the M1 prototype work, not proof of physical-device acceptance.
- `test/button_press_navigation/ButtonPressNavigationTest.cpp` verifies press-vs-hold behavior for the shared physical-button navigator.
- `c9d12b52` preserves historical DESIGN-002 assets and a local handoff, but it is local-only and is not treated as a remote approval record.

Recovered [STEP 03](../../superpowers/audits/STEP-03-FEATURE-INVENTORY.md), [STEP 04](../../superpowers/audits/STEP-04-ROUTE-TRACE.md) and [STEP 05](../../superpowers/audits/STEP-05-NAVIGATION-CONTRACT.md) provide baseline source evidence and original F01–F44 identifiers. Historical reconstructed detail is superseded where later V2 approval is explicit. [Execution contract](../../EXECUTION_CONTRACT.md) defines precedence; [STEP 09](STEP-09-NATIVE-INTEGRATION-PLAN.md) records current implementation differences and PO gates.

## 2. Product and interaction invariants

These rules preserve the approved product outcomes and incorporate the later explicit V2 decisions. Source-derived operational proposals and native authorization remain separately identified in STEP 09.

These are fixed for STEP 06:

1. The XTEInk UI is a layer inside existing CrossPoint. Existing reader, library, storage, transfer, OPDS, extensions/plugins, format, settings, recovery, localization, and device capability behavior remains available.
2. The logical coordinate system is 480×800 portrait, rendered as 1-bit monochrome E-Ink. Layouts must remain legible without color, gradients, blur, shadows, or information conveyed only by tone.
3. Vietnamese text must render correctly for both NFC and NFD input, including combining marks, without destructive normalization. Normalization may be used non-destructively for comparison, search, or layout measurement; source text semantics and displayable grapheme content must remain intact. Text supports diacritics without clipping, wraps rather than truncates important titles, and does not use negative tracking that damages diacritic clearance.
4. Interactive targets use a 44×44 px minimum baseline; 48×48 px or larger targets and 56 px primary actions are design preferences where layout permits. Adjacent touch regions have at least 8 px separation.
5. E-Ink updates are explicit and bounded: state changes may redraw only the affected region when safe, while modal opening, screen transitions, and user-requested cleanup refreshes must leave a stable, readable frame. No animation is required for correctness.
6. NAV-001 is fixed: Reader contextual Back returns to the Library with the prior viewport and focused book restored. Explicit Home is a separate command and never substitutes for contextual Back.
7. Navigation semantics are independent of visual presentation: Back, Home, Library tabs, and service destinations retain their defined meanings regardless of whether the UI uses a dock, header links, menus, or another approved presentation. STEP 06 does not lock a persistent dock.

## 3. Screen-state model

### Home

- **Populated:** current/resumable book, recent shelf, Library, Files/import, and Settings entry points.
- **Empty:** clear absence-of-books message plus import routes. Wi-Fi and USB are capability-gated and must not be shown as available when the device cannot provide them.
- **Unstarted/curated:** available catalog content without an active reading position; opening a book starts it without inventing progress.
- **Error:** preserve the Home shell and show a concise, localized recovery action when metadata, cover, storage scan, or an entry-point capability fails.

### Library

- **Approved tab mapping:** Recent, Title, Author, with per-mode direction and a direction indicator on the active tab; activate the same tab again to reverse it. No redundant Sort dropdown. Authority: V2 approved decisions.
- **Catalog:** title/author/progress rows, selection/focus, search, and sorting.
- **Search:** query, clear, diacritic-insensitive matching, zero-result state, and return to the prior catalog viewport.
- **Empty:** distinguish an empty library from a query with zero matches; offer rescan/import without losing the surrounding Library context.
- **Book selected:** opening a book records the Library viewport, selected book identity, tab, and relevant query/sort context needed by NAV-001.

### Reader

- **Reading:** page/chapter content, page turn zones, physical page buttons, and no required persistent navigation chrome.
- **Context controls:** a temporary HUD/controls surface may expose TOC, typography, refresh, and contextual Back.
- **TOC/typography modal:** modal controls trap focus until applied or cancelled; applying settings returns to the same book/chapter/page unless the user explicitly navigates elsewhere.
- **Reader error:** show a localized error and a safe return action; never silently route to Home when contextual Back can return to the originating Library state.

### Import / Files

- **File browser:** storage root, folders, supported files, parent-folder navigation, and a stable selection.
- **Import source:** local SD/file browser, Wi-Fi transfer, or USB MSC only when the capability is available and enabled.
- **USB MSC lifecycle:** preserve the native exclusive-ownership, eject/disconnect, failure/timeout teardown and restart-to-Home paths. Restore firmware SD ownership and rescan through that lifecycle; no direct Library transition.
- **Unsupported/corrupt file:** explain the failure, preserve the browser location, and offer Back/retry. Do not remove or rewrite the source file.
- **Successful import:** Wi-Fi may report success and offer Open Library only after transfer completion and successful rescan. USB requires safe eject/disconnect, firmware SD ownership restoration and rescan through its native restart-to-Home lifecycle; never offer direct USB→Library. Local Files preserves its existing origin/path behavior.

### Settings and services

- Settings retains network, OPDS, extensions/plugins, display/refresh, input, storage, and recovery access.
- Network/OPDS/plugin screens have explicit loading, success, authentication/permission, offline, empty, and failure states.
- A failed optional service must not make unrelated reading or local-library functions unavailable.

## 4. Journeys and control contracts

### Journey A — Home → Resume → Reader → Home

1. Home primary Resume opens the stored active book at its stored location.
2. Reader page controls and physical page buttons change only the reading position.
3. Explicit Home returns to Home independently of contextual Back.
4. Home reflects the current book and progress after return.
5. If the stored book is missing or unreadable, show a recoverable error and keep Home usable.

### Journey B — Library → Book → Reader → Contextual Back

1. Entering Library records the active tab, viewport anchor, selected/focused book, and search/sort context.
2. Opening a book enters Reader without discarding that Library context.
3. Reader contextual Back returns to the same Library tab and viewport, with the originating book focused.
4. Explicit Home remains independent and does not count as contextual Back.
5. If the originating Library item disappeared during reading, return to the nearest valid viewport and show a non-blocking localized notice.

### Journey C — Empty Home → Import → Wi-Fi completion / USB restart to Home

1. Empty Home explains that no books are present and offers local file browsing plus capability-gated Wi-Fi/USB routes.
2. Wi-Fi transfer preserves the existing CrossPoint transfer service and shows unavailable/offline/error states without pretending transfer succeeded.
3. USB MSC is shown only when the board/profile and runtime capability permit it.
4. Wi-Fi must complete write/close and successful rescan before success/Open Library. USB must be safely ejected/disconnected, release exclusive host ownership and follow native SD restoration/rescan and restart to Home; no direct Library transition. Preserve native local Files return/path semantics.
5. Unsupported, duplicate, corrupt, or interrupted imports remain recoverable and do not delete source content.

### Journey D — Settings → Network/OPDS/Extensions → Back

1. Settings opens the selected service without losing the Settings location.
2. Network and OPDS expose loading, authentication, empty, offline, and server-error states.
3. Extensions/plugins preserve the existing opt-in and capability rules; no unavailable action is presented as active.
4. Back returns to the prior Settings screen with its selection/focus restored.
5. Home remains an explicit independent escape path where the existing CrossPoint activity model provides it.

## 5. Input and focus rules

### Touch

- Touch activates the nearest visible target only; decorative regions do not become implicit commands.
- Reader page zones remain separate from the central HUD zone. A modal or HUD consumes input until dismissed.
- Focus/active state uses high-contrast inversion or a clear border and survives partial refreshes.
- Long text, Vietnamese diacritics, and translated labels may wrap; their hit target must remain the full visible row/control. NFC and NFD strings must render equivalently without deleting or flattening combining marks.

### Physical buttons

- Short press selects or steps once; holding begins repeated paging only after the existing navigator threshold.
- Release stops repeat behavior and must not generate an additional page or selection step.
- Direction changes start a fresh repeat interval.
- Screen-specific actions must use the existing mapped-input/activity ownership model; STEP 06 does not introduce new global button mappings.

### Back and Home

- Back is contextual to the current activity/modal and follows the journey contracts above.
- Home is an explicit destination command, separate from Back.
- Modal Back/cancel closes the modal before leaving its parent screen.
- At a root screen, Back is a no-op or uses the existing CrossPoint root behavior; it must not terminate reading or erase state.

## 6. Error, empty, and recovery requirements

Every screen state must specify:

- what the user can see;
- what caused the state in plain localized language;
- one safe retry or recovery action;
- a Back path that preserves the last valid context;
- whether a full or partial E-Ink refresh is needed.

Required distinctions include: empty Home vs empty Library, zero search results vs no books, offline vs authentication failure, unsupported vs corrupt file, unavailable capability vs failed operation, and missing active book vs invalid reading position.

## 7. STEP 07 acceptance criteria

STEP 07 browser artifacts and later V2 migration exist. These criteria apply to the candidate being verified; they do not erase earlier work or imply whole-design/native/hardware acceptance.

### Contract and navigation

- A: Resume opens the correct book/location and explicit Home returns to Home.
- B: Reader contextual Back restores Library tab, viewport, focused book, and search/sort context.
- C: Empty Home import works through each available source; unavailable Wi-Fi/USB paths are hidden or clearly disabled. Wi-Fi offers Library only after transfer/rescan; USB safely releases SD and restarts to Home.
- D: Settings service flows preserve focus and return correctly through network, OPDS, and extensions/plugins.
- Existing CrossPoint format, reader, storage, recovery, and localization capabilities remain reachable.

### Device and typography

- 480×800 logical portrait layout remains readable in 1-bit output.
- Vietnamese titles and labels show complete diacritics for both NFC and NFD input without destructive normalization, clipping, or truncation.
- Touch targets meet the 44 px baseline minimum; larger targets and 56 px primary actions are recorded as design preferences, not baseline contract failures.
- Physical button short/hold/release behavior matches the existing navigator contract.

### E-Ink behavior

- Every transition leaves a stable readable frame.
- Partial refreshes do not leave stale focus, modal, or progress pixels.
- Full-refresh controls work where required and do not create an unbounded refresh loop.
- Human hardware testing records panel-specific ghosting, latency, touch, button, storage, Wi-Fi, USB MSC, and recovery observations.

### Browser prototype gate

- Browser prototype acceptance verifies the four journeys, Back/Home semantics, focus restoration, empty/error states, NFC/NFD display samples, capability-gated routes, and baseline touch-target geometry. It does not prove simulator or hardware behavior.
- Any browser-prototype failure blocks the browser gate and is triaged before that gate is accepted.

### Later simulator and hardware gate

- Simulator verification covers only supported native rendering/activity/input paths. Host storage/display shims do not verify real SD/USB ownership, transfer durability, physical input, MCU resources or E-Ink refresh/ghosting; those remain device checks.
- Human hardware verification separately covers panel rendering, ghosting, latency, touch, physical buttons, USB MSC disconnect, SD ownership restoration, Wi-Fi, storage, and recovery.
- Browser, simulator, and hardware results must remain separately identified; passing one gate does not imply passing another.

### Deferred 44-feature parity gate

- Before any firmware integration is authorized, verify parity for all 44 preserved CrossPoint capabilities/features against the existing baseline, including reader, library, formats, storage, transfer, OPDS, extensions/plugins, settings, localization, and recovery behavior.
- This 44-feature parity check is deferred from STEP 06 and is not satisfied by prototype/browser evidence.
- Any parity gap blocks firmware integration and requires Product Owner disposition.

## 8. Settled decisions and remaining gates

Library mapping/direction, navigation presentation, English system UI, contextual upper-left Back, Wi-Fi completion and USB restart-to-Home are settled by V2 and STEP 07B decisions. Do not ask again. NAV-001 and independent Home remain fixed. Native font selection needs measured evidence; native Back binding must preserve the existing logical mapping and approved V2 placement; ledger disposition, implementation acceptance and integration authorization are the bounded gates in STEP 09. They do not reopen the approved visual direction.

## 9. Explicit non-goals

- No firmware or SDK implementation.
- No changes to the HTML prototype, build files, translations, dependencies, or partition configuration.
- No new feature, format, service, network connector, or navigation model.
- No claim that STEP 03–05 were previously approved or that STEP 07D runtime QA passed.
- This document correction does not execute or reauthorize STEP 07.

# XTEInk OS — STEP 06 UX Flows & Controls

> **Status:** Reconstructed working contract for Product Owner review
> **Provenance:** Reconstructed from verified local repository evidence; no unpublished Company PC STEP 03–05 files were available.
> **Scope:** UX flows and controls only. No firmware, SDK, prototype, build, or dependency changes.
> **Target:** Xteink X4 Pro, logical portrait viewport 480×800, monochrome E-Ink, Vietnamese Unicode.

## 1. Evidence boundary

The following evidence is verified locally:

- `docs/design/BASELINE.md` identifies H1.1A as the approved prototyping baseline, keeps DESIGN-002 as historical reference, and prohibits unapproved promotion of experimental designs.
- `docs/design/specs/UNIFIED_UI_COLLECTION.md` defines the 15 representative frames, the 480×800 logical canvas, Vietnamese typography rules, touch-target rules, and prototype journeys.
- `docs/design/specs/M2-WP2-HOME-PLAN.md` records the existing Home implementation, the existing navigation conflict, and the requirement to preserve the existing activity/`fui::` stack.
- `docs/superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md` and `docs/superpowers/audits/M1-UI-COLLECTION-QA.md` record the prior prototype reconciliation and browser QA claims. They are evidence of the M1 prototype work, not proof of physical-device acceptance.
- `test/button_press_navigation/ButtonPressNavigationTest.cpp` verifies press-vs-hold behavior for the shared physical-button navigator.
- `c9d12b52` preserves historical DESIGN-002 assets and a local handoff, but it is local-only and is not treated as a remote approval record.

STEP 03 (inventory), STEP 04 (route trace), and STEP 05 (navigation contract) were not found as formal local artifacts. The contract below therefore labels reconstructed rules as **R-CONSTRUCTED**. The user-supplied product contract is treated as the current authority for NAV-001 and the required capabilities.

## 2. Product and interaction invariants

The rules in this section combine the user-supplied approved product contract with **R-CONSTRUCTED** operational detail where the missing STEP 03–05 documents would normally have supplied it.

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

- **Unapproved tab mapping:** preserve three Library tabs. Exact tab names, ordering, and mapping are pending source verification and Product Owner design review; no mapping is approved by this document.
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
- **USB MSC lifecycle:** before leaving USB MSC, require a safe disconnect/eject acknowledgement or equivalent completion signal, restore SD ownership to the firmware, rescan storage, and only then transition to Library.
- **Unsupported/corrupt file:** explain the failure, preserve the browser location, and offer Back/retry. Do not remove or rewrite the source file.
- **Successful import:** rescan/register the book, preserve the import result, and return to Library with the new book discoverable.

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

### Journey C — Empty Home → Import → Wi-Fi/USB → Library

1. Empty Home explains that no books are present and offers local file browsing plus capability-gated Wi-Fi/USB routes.
2. Wi-Fi transfer preserves the existing CrossPoint transfer service and shows unavailable/offline/error states without pretending transfer succeeded.
3. USB MSC is shown only when the board/profile and runtime capability permit it.
4. After a successful copy, USB MSC must be safely disconnected, SD ownership restored to firmware, and storage rescanned before registering supported formats and returning to Library. Wi-Fi/local imports must complete their own write/close operation and rescan before the same transition.
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

STEP 07 remains unstarted and unaccepted. These are acceptance criteria for a future verification phase only:

### Contract and navigation

- A: Resume opens the correct book/location and explicit Home returns to Home.
- B: Reader contextual Back restores Library tab, viewport, focused book, and search/sort context.
- C: Empty Home import works through each available source; unavailable Wi-Fi/USB paths are hidden or clearly disabled; successful import reaches Library.
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

- Simulator verification separately covers activity transitions, storage ownership/rescan behavior, partial/full refresh state, and input edge cases.
- Human hardware verification separately covers panel rendering, ghosting, latency, touch, physical buttons, USB MSC disconnect, SD ownership restoration, Wi-Fi, storage, and recovery.
- Browser, simulator, and hardware results must remain separately identified; passing one gate does not imply passing another.

### Deferred 44-feature parity gate

- Before any firmware integration is authorized, verify parity for all 44 preserved CrossPoint capabilities/features against the existing baseline, including reader, library, formats, storage, transfer, OPDS, extensions/plugins, settings, localization, and recovery behavior.
- This 44-feature parity check is deferred from STEP 06 and is not satisfied by prototype/browser evidence.
- Any parity gap blocks firmware integration and requires Product Owner disposition.

## 8. Open decisions for Product Owner review

1. **Library tab mapping:** verify the historical source and approve the exact names, ordering, and mapping of the three Library tabs during Product Owner design review.
2. **Navigation presentation:** choose the visual presentation of the approved navigation semantics, including whether to use a dock, header links, menus, or another treatment. No persistent dock is locked by STEP 06; this does not reopen NAV-001.
3. **Import completion policy:** confirm whether successful Wi-Fi/USB imports should automatically open Library or show a completion confirmation first. The contract requires reaching Library but does not choose the intermediate acknowledgement.

No other product decision is reopened here. In particular, NAV-001 is fixed as written above.

## 9. Explicit non-goals

- No firmware or SDK implementation.
- No changes to the HTML prototype, build files, translations, dependencies, or partition configuration.
- No new feature, format, service, network connector, or navigation model.
- No claim that STEP 03–05 were previously approved or that STEP 07D runtime QA passed.
- No STEP 07 execution.

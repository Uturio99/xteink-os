# STEP 07B.1R — UX decisions

- **BRAND-001 — NoveOS:** Approved product name for user-facing prototype branding. CrossPoint remains the underlying foundation and attribution. Repository, SDK, identifiers, and boot assets retain their existing names.
- **INPUT-001 — parity:** Keep touch interactions and preserve existing CrossPoint physical-button behavior. Reader configuration remains reachable through the existing Reader menu contract: physical Confirm release opens it; touch devices also use the existing center-third tap/menu gesture; home-key boards use the configured Home-button ReaderMenu action. No firmware event or new mapping is defined here. The current STEP 07B browser prototype is illustrative and does not simulate those physical inputs.
- **CLOCK-001 — native clock:** Reuse the native Reader clock in product implementation; the HTML time is static visual placeholder only.
- **HW-NAV-001 — deferred:** Physical navigation hardware treatment is deferred. This decision does not change current physical-button behavior or authorize firmware work.
- Keep explicit Home separate from contextual Back. Keep Library tab mapping unresolved; preserve NAV-001 contextual Back and CrossPoint feature parity. Visual approval remains separate from runtime, integration, and hardware QA.

## STEP 07B.1R2 — approved evolution

This PO-approved interaction styling and default language evolves the approved Hybrid Editorial Index v1.0 without replacing its visual DNA:

- Primary and secondary action buttons use full pills (`border-radius: 9999px`); other controls, rows, and tabs retain their existing shapes. The monochrome palette, typography, layout, and Field Index / Quiet Editorial structures stay intact.
- Place contextual Back at the upper-left. Remove the redundant Reader Home control; keep Home distinct through the existing CrossPoint Home route, independently handled from Reader Back: configured Home action on Home-key boards, or the existing bottom-edge up-swipe on boards without a Home key. Source: `MappedInputManager::wasHomeGesture()` and `ActivityManager` dispatch. Browser preview cannot simulate that physical route.
- NoveOS system UI defaults to English, with no system-wide localization framework. Book titles, metadata, and Reader text remain source content and support Vietnamese NFC/NFD without translation or mark stripping.
- Reader status places Back at upper-left, battery and percentage on the right, and time at far-right; footer shows position and reading percentage. Firmware reuses its native Reader clock.
- Visual approval remains separate from runtime and hardware verification. NAV-001, unresolved Library mapping, CrossPoint parity, INPUT-001, CLOCK-001, and HW-NAV-001 remain in force.

## STEP 07B browser prototype — scope and evidence boundary

- Journeys C and D are browser-only simulations. Wi-Fi, USB MSC, SD ownership restoration, storage rescan, network, OPDS, and extension actions do not reach device services or storage.
- The Reader sample uses mock page position 128 of 320 to derive 40%; this is not actual EPUB pagination. If no reliable total is available, show Position X without / Y; show a percentage only when the format provides a reliable progress value.
- A successful simulated import adds only a clearly labeled sample row to the browser Library. It does not claim a file was transferred to the device.
- Browser QA is separate from simulator, firmware, and physical-hardware verification.

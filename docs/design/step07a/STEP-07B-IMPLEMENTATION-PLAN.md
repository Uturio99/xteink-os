# STEP 07B — Interactive Prototype Plan

> Status: unaccepted planning WIP. STEP 07B has not been approved or accepted. The comparison page includes experimental interactions for preservation and review only; they are not STEP 07B acceptance or QA evidence.

**Goal:** Preserve the A/B/C visual explorations and, only after PO approval, use a direction still to be selected for a browser-only prototype of STEP 06 journeys and controls. The PO is reconsidering A · Quiet Editorial versus B · Field Index; final visual direction is open.

**Approach:** Any future STEP 07B work would extend the self-contained HTML/CSS/JS comparison artifact while retaining A, B, and C. No direction is currently selected for implementation. Preserve STEP 06 semantics and label unresolved decisions rather than inventing answers.

**Tech:** Vanilla HTML, CSS, and JavaScript; no frameworks, dependencies, firmware, or SDK changes.

**Source contract:** [`STEP-06-UX-FLOWS.md`](../specs/STEP-06-UX-FLOWS.md).

## Scope and implementation sequence

1. **Prototype foundation (proposed; not authorized):** Keep all three STEP 07A explorations accessible. After explicit PO approval of STEP 07B and selection of a working direction, implement interactive states in the existing preview, maintaining the 480×800 logical artboard and no horizontal overflow.
2. **Home and Reader:** Explore real book covers on Home using existing, public-domain, or licensed assets; record an asset gap if none are verified. Avoid time-dependent greetings. Implement Resume, Reader controls, independent explicit Home, and minimal persistent Reader chrome. Contextual Back from a Library-opened book must restore the originating Library tab, viewport, and focused book (NAV-001).
3. **Library:** Retain exactly three tabs as neutral placeholders. Do not assign names, ordering, or semantics until historical source verification and PO design review. Opening a book and returning must preserve focus and viewport.
4. **Import and Settings journeys:** Demonstrate empty Home and capability-gated Wi-Fi/USB routes. Model safe USB disconnect/eject, SD ownership restoration, and rescan before Library transition. Preserve the simple Settings hierarchy and routes for Network, OPDS, and Extensions, with contextual Back.
5. **Browser QA:** Verify Journeys A–D, Back/Home separation, focus restoration, empty/error/capability states, ≥44px touch targets, keyboard/focus behavior, Vietnamese NFC and NFD rendering without destructive normalization, and 480×800 clipping/overflow. Browser acceptance does not establish simulator or hardware acceptance.

## Constraints and gates

- Carry forward every STEP 06 contract, including CrossPoint capability preservation, touch-target baseline, E-Ink-readable stable states, and Vietnamese NFC/NFD rendering requirements.
- Keep visual navigation presentation open; do not add or assume a persistent dock.
- Treat USB/storage lifecycle behavior as a prototype state demonstration, not device-level implementation.
- STEP 07B ends at browser prototype review. Simulator/hardware verification is separate, and the deferred 44-feature CrossPoint parity gate remains required before any firmware integration.
- This plan is not approval to start STEP 07B. Obtain explicit PO approval before implementation; obtain PO review of the resulting interactive prototype before treating pixel details as approved or beginning any later integration phase.

## Unresolved decisions retained

- Exact Library tab names, order, and mapping: unresolved pending source verification and PO design review.
- Import completion acknowledgement versus direct Library transition: unresolved per STEP 06.
- Final visual direction (A versus B), pixel-level composition, cover treatment, and Reader chrome: open for PO review; no direction is selected.

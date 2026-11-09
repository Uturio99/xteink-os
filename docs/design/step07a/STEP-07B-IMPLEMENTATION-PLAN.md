# STEP 07B — Interactive Prototype Plan

> HISTORICAL browser checkpoint. Earlier PASS claims and unresolved-tab wording below apply only to that revision. [V2](../DESIGN-SYSTEM-V2-PROPOSAL.md) now fixes Recent/Title/Author, per-mode direction and import completion. Use [STEP 09](../specs/STEP-09-NATIVE-INTEGRATION-PLAN.md) for current delivery, not this completed prototype plan.

> Status: browser prototype implemented and verified for Product Owner review. This does not establish simulator, firmware, or hardware acceptance.

**Visual baseline:** The Product Owner approved the exact visual DNA in [`DESIGN-SYSTEM.md`](../DESIGN-SYSTEM.md), grounded in the immutable [`Hybrid Editorial Index reference`](xteink-hybrid-demo.html). Use its Field Index Home/Library structure and Quiet Editorial typography as the implemented browser baseline. This replaces the earlier open A-versus-B direction decision. Preserve the existing A/B/C explorations unchanged.

**Approach:** The PO authorized browser-only work. The self-contained [browser prototype](../step07b/step-07b.1.html) retains the A/B/C explorations, uses the approved Hybrid Editorial Index baseline, and labels unresolved decisions without inventing answers.

**Tech:** Vanilla HTML, CSS, and JavaScript; no frameworks, dependencies, firmware, or SDK changes.

**Source contract:** [`STEP-06-UX-FLOWS.md`](../specs/STEP-06-UX-FLOWS.md).

## Scope and implementation sequence

1. **Prototype foundation — PASS:** The browser prototype uses the approved baseline at a fixed 480×800 logical size; the A/B/C explorations and immutable demo remain unchanged.
2. **Home and Reader — PASS:** Resume and contextual Back work; the mock Reader footer derives Page 128 / 320 and 40%. No real EPUB pagination is claimed.
3. **Library — PASS:** Search, sort, and neutral tabs work. NAV-001 restores query, sort, tab, focused book, and viewport, including a tested 23px scroll position.
4. **Import and Settings — PASS, simulated:** Empty Home offers local, Wi-Fi, and capability-gated USB routes. Failure recovery and the safe disconnect → SD ownership restoration → rescan sequence are browser states only. Network, OPDS, and Extensions return to Settings with focus restored.
5. **Browser QA — PASS:** Journeys A–D, Vietnamese NFC/NFD display, and 480×800 canvas bounds were checked in the browser. No console errors were recorded. Simulator and hardware behavior were not tested.

## Constraints and gates

- Carry forward every STEP 06 contract, including CrossPoint capability preservation, touch-target baseline, E-Ink-readable stable states, Vietnamese NFC/NFD rendering, and NAV-001.
- Match the approved visual baseline in DESIGN-SYSTEM.md. Do not add visual treatments absent from the reference. Keep the three Library tabs neutral; their mapping remains unresolved.
- Treat USB/storage lifecycle behavior as prototype state demonstration, not device-level implementation.
- STEP 07B ends at browser prototype review. Simulator/hardware verification is separate, and the deferred 44-feature CrossPoint parity gate remains required before any firmware integration.
- Browser-only STEP 07B is implemented for PO visual/UX review. It authorizes no firmware, SDK, simulator, or hardware changes; those remain separate gates.

## Unresolved decisions retained

- Exact Library tab names, order, and mapping: unresolved pending source verification and PO design review.
- Import completion acknowledgement versus direct Library transition: unresolved per STEP 06.
- Browser interaction acceptance remains pending PO review. Runtime interaction details and device behavior remain outside this browser checkpoint.

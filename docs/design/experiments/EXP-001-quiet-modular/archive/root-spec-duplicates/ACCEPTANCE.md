# DESIGN ACCEPTANCE GATES — XOS-DL v1.0 (PROPOSED)

**Important:** This is a candidate rule set. Product Owner approval is required before treating it as Concept B source of truth.

## Gate 0 — Evidence and boundaries (hard stop)

- [ ] Existing Concept A, Golden Home, H1.1A firmware and earlier EXP-001 files remain unchanged.
- [ ] Work confined to the Concept B design-system folder unless explicitly authorized.
- [ ] References are real URLs from `REFERENCES.md` with source attribution, not invented.
- [ ] Prototype does not claim actual e-paper refresh rate, power saving, native font fidelity or usable features that aren't implemented.

## Gate 1 — Composition (P0)

- [ ] Dominant primary reading object visible immediately in **Home populated**.
- [ ] Only one primary filled action; no permanent smartphone-like bottom dock.
- [ ] Content groups are optically aligned to the 32px anchor; whitespace has semantic structure.
- [ ] The information hierarchy works after hiding outlines, badges and secondary gray treatments.
- [ ] Long Vietnamese hero title + author + progress never clip or overlap; if needed, recents shrink, NOT primary title.
- [ ] Empty Home reorganizes functionally around Add Books rather than showing a blank book placeholder.

## Gate 2 — Usability (P0)

- [ ] One tap from Home to Resume, Library, Import and Settings in browser prototype or clearly mapped route.
- [ ] Every visible control in the final interactive design has an actual handler or is explicitly labeled as a design-only placeholder.
- [ ] Minimum clickable hitbox 48px in both axes; high-frequency primary ≥56px where practical.
- [ ] Touch and hardware-button focus order documented; Back/recovery accessible.
- [ ] Library row supports long metadata; the active search/sort state is visible without color.

## Gate 3 — Native feasibility (P1, not passable by browser alone)

- [ ] All dynamic data points map to real CrossPoint model or explicitly marked not yet verified.
- [ ] Serif and sans native font rasterization and Vietnamese NFC/NFD validated through simulator/font test.
- [ ] Missing cover, missing SD, unknown progress and error states documented.
- [ ] No animation/gradient/blur or hardware behavior simulated as real.
- [ ] Actual ghosting, touch, waveform and RAM/power results labeled UNVERIFIED until device tests.

## Gate 4 — Visual acceptance (PO only)

- [ ] 480×800 PNG capture of Home populated and empty.
- [ ] Library, Reader samples and one COMPONENTS specimen capture.
- [ ] Two reference IDs cited per screen at most, with concrete borrow/reject rationale.
- [ ] Side-by-side comparison to old Concept B identifies exactly what changed.
- [ ] Product Owner explicitly approves the art direction. No agent can self-mark this gate.

## Status vocabulary (mandatory)

`SPECIFIED` = documented; `BROWSER_VERIFIED` = rendered/interaction tested in browser; `SIMULATOR_VERIFIED` = CrossPoint native simulator; `HARDWARE_VERIFIED` = real X4 Pro. `PROPOSED` ≠ `APPROVED`.

## No unbounded iteration

After one pass: deliver screenshots, the token/constraint deviation list (max 10 items), any failed gates and STOP. Do not launch redesign variants, generate additional Golden references, modify firmware or edit the production design baseline without explicit instruction.

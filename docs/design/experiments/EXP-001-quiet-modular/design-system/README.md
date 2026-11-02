# XOS-DL v1.0 — Quiet Editorial Utility

**STATUS: PROPOSED / EXPERIMENT ONLY / NOT APPROVED FOR FIRMWARE**  
**Author / design authority:** ChatGPT — design-system specification for Product Owner review  
**Target:** XTEINK X4 Pro, **480 × 800 logical pixels, portrait**  
**Placement:** `docs/design/experiments/EXP-001-quiet-modular/design-system/`  
**Updated:** 2026-11-02

## Purpose

A concrete visual and interaction contract for **Concept B**. It has one goal: retain CrossPoint functionality while making reading, navigation and organization feel composed, quiet and intentional. The design takes principles from Apple HIG and visual observations of DuRoBo Krono, **without claiming affiliation or copying their UI**.

This package is *the author-written Concept B experiment specification*, not a request for an agent to invent a design system. `specimen.html` is the visual target; the documents and token file are the accompanying rules. All native feasibility claims remain hypotheses until independently tested.

## Read order / authority

1. **`README.md`** — status, scope, agent contract.
2. **`FOUNDATIONS.md`** — geometry, typography, color, tokens.
3. **`COMPOSITION.md`** — mandatory screen grammar and layouts.
4. **`COMPONENTS.md`** — variants, behavior, interactive targets.
5. **`INTERACTION.md`** — flows, hardware keys, e-paper behavior.
6. **`REFERENCES.md`** — exact, verified external UI references and what to borrow/avoid.
7. **`ACCEPTANCE.md`** — tests and release gates.
8. **`tokens.json`** — machine-readable proposed token values.
9. **`AGENT_CONTRACT.md`** — exact bounded read-only/implementation workflow for harnesses.
10. **`specimen.html`** — one self-contained, **offline** visual study: Home populated, Home empty, Library and Reader. **It is not firmware.**

### Source of truth precedence within EXP-001

This proposed package > prior EXP-001 `art-direction.md` or `design-notes.md` **only after the Product Owner approves** this design-system package. Until approved, earlier experiment artifacts remain unchanged and this is a competing candidate spec. In case of internal discrepancy, stop and report it; do not pick whichever rule is easiest to code. Native code, Concept A, Golden Home and any earlier approved product design remain untouched.

## Non-negotiables

- **Function-led, composition-driven:** functionality first; composition is the primary visual craft; typography supports it.
- **Content over chrome:** a dominant reading object, one primary action, contextual secondary actions.
- **No permanent four-tab dock** in this Concept B direction; library/import/settings accessible within **one tap from Home**.
- White and black must carry the complete information structure without gray, animation, glass, shadows or gradients.
- Touch and hardware-button paths must be mapped. Do not confuse Apple's **44 points** with X4 Pro **44 physical pixels**.
- Vietnamese long titles, diacritics, empty/error/loading states are design inputs, not edge cases.
- Never fabricate storage health, battery %, IP address, pages-left estimates, book progress or unsupported CrossPoint features.
- Never claim lower power, faster e-paper refresh or memory savings from HTML/CSS screenshots.

## Instruction to all harnesses

**Antigravity:** Read the files in the order above. Do not redesign these foundations. First reproduce `specimen.html` geometry and capture 480 × 800 images with a difference log; stop for approval. After approval, adapt to Library/Reader/Files/Settings one screen at a time. Any required change to a locked token must be proposed as a deviation with reason and screenshot. No auto-approval, new concepts, subagents, firmware edits or broad searches.

**DeepSeek:** This is a design candidate, not an engineering task. No C++ implementation, code modifications or flash until explicit Product Owner approval and feasibility review. When requested to implement, reconcile components against CrossPoint APIs and report differences rather than silently downgrading typography.

**Codex:** Technical feasibility reviewer only on request; verify font metrics, Vietnamese shaping/rasterization, touch and refresh implementation in the native simulator. Do not treat browser preview as device validation.

## Definition of done for DS acceptance

Product Owner has reviewed the 480×800 specimen; screenshot evidence is attached; all rules in `ACCEPTANCE.md` pass or have tracked deviations; and explicit approval is recorded. **No approval is implied by file creation or by an agent reporting PASS.**

## Local visual QA evidence

`previews/home.png`, `previews/empty.png`, `previews/library.png`, `previews/reader.png`, `previews/specimen.png` were rendered in Chromium at **480×800** from `specimen.html`; `*-binary-check.png` are simple threshold previews, **not actual device captures**. Screen body bounding boxes for Home populated/empty/Library/Reader remained within 800px in this check. The scrolling Tokens sheet intentionally extends beyond the viewport. Browser checks confirmed Home `Đọc tiếp` → Reader and Reader → HUD toggle. Import/Settings actions are explicitly stubbed as design-only.

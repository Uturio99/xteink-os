# XTEInk OS — Project Status & Milestone Roadmap

> **Program Tracking Document:** Obra Superpowers Framework  
> **Repository:** `xteink-os`  
> **Current Active Milestone:** **M2 — Governance Reconciliation / WP2.4 Verification**
> **Last Updated:** 2026-11-04

---

## Milestone Progress Overview

| Milestone | Title | Status | Primary Focus | Dependencies |
| :---: | :--- | :---: | :--- | :--- |
| **M0** | **Safe Project Consolidation** | **COMPLETE (100%)** | Non-destructive repository audit, baseline protection, archival of duplicates, single sources of truth. | None |
| **M1** | **Baseline Reconciliation & Selection** | **COMPLETE (DESIGN ACCEPTED)** | Product Owner alignment on production design baseline (Concept A / H1.1A vs Concept B / XOS-DL v1.0). Design frozen for native implementation. | M0 |
| **M2** | **Screen Architecture & Full Specs** | **ACTIVE — GOVERNANCE APPROVED** | Approved bounded decisions; WP2.1–WP2.3 checkpoints confirmed; WP2.4 is authorized for verification and firmware-handoff gating. | M1 |
| **M3** | **Native C++ Home & Nav Architecture** | **QUEUED** | Implementing approved Home layout and navigation hierarchy in embedded C++ on X4 Pro (ESP32-S3 / 8 MB PSRAM) while maintaining ESP32-C3 compatibility. | M2 |
| **M4** | **Font Subsystem & Typography** | **QUEUED** | Bitmap and vector font pipeline, Vietnamese glyph coverage, font metrics cache. | M3 |
| **M5** | **Touch Driver, Hit Targets & Events** | **QUEUED** | Touch controller integration (GT911 on X4 Pro subject to SDK board-profile verification, FT6336 on Paper Mono / legacy), ≥44px hit bounds, debouncing, and input event routing. | M3 |
| **M6** | **Waveform Modes & Refresh Pipeline** | **QUEUED** | 1-bit fast paging, 4-bit high quality rendering, ghosting mitigation, flashing reduction. | M3, M5 |
| **M7** | **End-to-End Release & Handoff** | **QUEUED** | Release packaging and final handoff after earlier separately authorized hardware validation. | M1–M6 |

---

## Detailed Milestone Status

### M0: Safe Project Consolidation
- **Objective:** Establish a clean, well-documented, non-destructive foundation across all branches, documentation, and design assets without losing historical work.
- **Key Deliverables:**
  - [x] Full baseline inventory (`docs/superpowers/audits/M0-INVENTORY.md`)
  - [x] Cryptographic move manifest (`docs/superpowers/audits/M0-MOVE-MANIFEST.md`)
  - [x] Bounded execution plan (`docs/superpowers/plans/M0-project-consolidation.md`)
  - [x] Project navigation guide (`docs/PROJECT_MAP.md`)
  - [x] Milestone status dashboard (`docs/PROJECT_STATUS.md`)
  - [x] Design baseline specification (`docs/design/BASELINE.md`)
  - [x] Non-destructive archival of redundant root duplicates into `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/`
  - [x] Final verification and Product Owner handoff report (docs/superpowers/audits/M0-CLOSEOUT.md).
- **Current Blocker:** None. M0 complete and ready for Product Owner review and transition to M1.

---

### M1: Baseline Reconciliation & Target Selection
- **Objective:** Reconcile differences between the approved production baseline (Concept A / H1.1A Native Home) and the experimental explorations (Concept B / EXP-001 / XOS-DL v1.0) to select the definitive architectural direction.
- **Status:** **COMPLETE & DESIGN ACCEPTED (Design Freeze Checkpoint)**
- **Design Acceptance Record:**
  - Complete 15-frame UI Collection and interactive HTML prototype approved by Product Design Review.
  - Authentic TrueType reference fonts bundled with SIL OFL 1.1 license for reproducible typography across host environments.
  - Automated Playwright QA verification suite confirmed 100% PASS across dimensions, frame integrity, overflow, 99 evaluated touch targets, state persistence, search reset, sorting, Reader ↔ HUD typography synchronization, and all 5 user journeys.
- **Key Deliverables:**
  - [x] Baseline Reconciliation Matrix ([docs/superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md](superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md))
  - [x] Unified UI Collection Specification ([docs/design/specs/UNIFIED_UI_COLLECTION.md](design/specs/UNIFIED_UI_COLLECTION.md))
  - [x] Complete 15-Frame Navigable HTML Prototype ([docs/design/prototype/unified-ui-prototype.html](design/prototype/unified-ui-prototype.html))
  - [x] 15-Frame Visual Inspection Gallery ([docs/design/prototype/gallery.html](design/prototype/gallery.html))
  - [x] 15 Pixel-Exact 480×800 Captures ([docs/design/prototype/captures/](design/prototype/captures/))
  - [x] Comprehensive QA & Verification Report ([docs/superpowers/audits/M1-UI-COLLECTION-QA.md](superpowers/audits/M1-UI-COLLECTION-QA.md))
  - [x] Product Owner Review Package ([docs/superpowers/audits/M1-UI-COLLECTION-REVIEW.md](superpowers/audits/M1-UI-COLLECTION-REVIEW.md))
- **Documented Implementation Limitations (Deferred to Native Implementation):**
  - **Reader dynamic reflow/pagination:** Text formatting is styled via CSS in the reference prototype, but true book dynamic reflow and pagination calculations across custom font sizes (14–22px) must be implemented by the native C++ EPUB layout engine in firmware.
  - **Search demo query initialization:** The search frame initializes with a deterministic demo query (`chiến tranh`) for automated capture reproducibility; native firmware will initialize with empty query and live SQLite/catalog index.
  - **Hardware e-paper behavior:** 1-bit thresholding and high-contrast styling validated in browser reference, but physical refresh waveforms, ghosting clearing, and panel partial updates require on-device validation on XTEINK X4 Pro hardware.
  - **DESIGN-002 historical provenance:** Components derived from historical DESIGN-002 explorations remain categorized as unverified design heritage, preserved for visual guidance rather than proven firmware implementation.
- **Current Blocker:** None. Ready for M2 (Screen Architecture & Full Specs).

---

### M2: Screen Architecture & Full Specifications
- **Objective:** Establish pixel-exact, interaction-complete specifications for all core screens.
- **Deliverables:**
  - Definitive state matrix for Home (populated, empty, first-run).
  - Library browsing (grid, list, sorting, metadata filtering).
  - Reader and HUD overlay (paging, progress scrubber, font size adjustments).
  - File manager / storage browser (SD card directory tree, USB transfer state).
  - Settings (refresh modes, power management, Wi-Fi configuration).

#### M2 governance checkpoint

- PD-1 through PD-7 are approved using the documented defaults; PD-6
  investigation is deferred.
- D8 and D10 are confirmed unchanged. The accepted WP1 D9 direction is
  preserved: NotoSans fallback, SD-loaded Reader TTF, and minimal bundled UI
  Serif evaluation. Exact subset and weight remain open.
- D11 analysis is delegated to engineering. A measured flash-reserve
  recommendation and later PO approval are required before bundled Serif
  integration.
- WP2.1–WP2.3 have task-specific implementation and checkpoint approvals.
  WP2.4 has separate implementation and test-only authorization; its current
  six-file patch remains uncommitted pending verification and firmware-handoff
  requirements.
- The delivery roadmap is five stages: (1) governance and design freeze,
  (2) bounded implementation, (3) verification and firmware handoff,
  (4) early hardware validation, and (5) release packaging. Existing WP
  identifiers remain task-traceability labels.
- Hardware validation is an early, separately authorized activity; it is not
  postponed wholesale until release.

---

### M3: Native C++ Home & Navigation Architecture
- **Objective:** Transition approved screen specifications into production C++ firmware within `src/activities/` and `src/components/`.
- **Target Subsystems:**
  - `src/components/CoverGridHomeUi.cpp` / `X4ProHomeLayout.h`
  - Activity navigation stack and transition lifecycle.
  - Memory budget compliance: zero dynamic heap growth in hot paths, stack usage < 4 KB per task.

---

### M4: Font Subsystem & Typographic Hierarchy
- **Objective:** Optimize font rendering for e-paper legibility and resource limits.
- **Deliverables:**
  - Font rasterization pipeline supporting Vietnamese diacritics without clipping.
  - Runtime font glyph caching strictly bounded to allocated RAM pool.
  - Fallback font chains for Latin and extended Unicode ranges.

---

### M5: Touch Driver, Hit Targets & Event Bus
- **Objective:** Ensure responsive, ergonomic touch input on physical hardware.
- **Deliverables:**
  - GT911 I2C touch controller tuning on X4 Pro (subject to SDK board-profile verification; FT6336 on Paper Mono / legacy).
  - Minimum touch target bounds (≥44×44 CSS equivalent; 56px for primary reading actions).
  - Edge swipe rejection and reading gesture zones.

---

### M6: Waveform Modes & Refresh Pipeline
- **Objective:** Minimize e-paper visual fatigue and ghosting while maximizing paging speed.
- **Deliverables:**
  - Rapid 1-bit waveform mode for page turns (<300ms latency).
  - 4-bit grayscale waveform mode for cover art and home dashboard rendering.
  - Configurable full-refresh counter (e.g., full clean refresh every 5–10 pages).

---

### M7: End-to-End Release & Handoff
- **Objective:** Complete physical validation on Xteink X4 Pro hardware, finalize documentation, and package release binaries.
- **Deliverables:**
  - PlatformIO release build verification (`pio run -e x4pro`).
  - Serial console diagnostic logs verifying heap stability over 2+ hours continuous reading.
  - Execution of `.agents/skills/firmware-handoff/SKILL.md` checklist with human sign-off.

# XTEInk OS — Project Status & Milestone Roadmap

> **Program Tracking Document:** Obra Superpowers Framework  
> **Repository:** `xteink-os`  
> **Current Active Milestone:** **M0 — Safe Project Consolidation**  
> **Last Updated:** 2026-11-02  

---

## Milestone Progress Overview

| Milestone | Title | Status | Primary Focus | Dependencies |
| :---: | :--- | :---: | :--- | :--- |
| **M0** | **Safe Project Consolidation** | **COMPLETE (100%)** | Non-destructive repository audit, baseline protection, archival of duplicates, single sources of truth. | None |
| **M1** | **Baseline Reconciliation & Selection** | **PENDING REVIEW** | Product Owner alignment on production design baseline (Concept A / H1.1A vs Concept B / XOS-DL v1.0). | M0 |
| **M2** | **Screen Architecture & Full Specs** | **QUEUED** | Comprehensive interaction and visual specs for all 5 screens (Home, Library, Reader, Files, Settings). | M1 |
| **M3** | **Native C++ Home & Nav Architecture** | **QUEUED** | Implementing approved Home layout and navigation hierarchy in embedded C++ on X4 Pro (ESP32-S3 / 8 MB PSRAM) while maintaining ESP32-C3 compatibility. | M2 |
| **M4** | **Font Subsystem & Typography** | **QUEUED** | Bitmap and vector font pipeline, Vietnamese glyph coverage, font metrics cache. | M3 |
| **M5** | **Touch Driver, Hit Targets & Events** | **QUEUED** | Touch controller integration (GT911 on X4 Pro subject to SDK board-profile verification, FT6336 on Paper Mono / legacy), ≥44px hit bounds, debouncing, and input event routing. | M3 |
| **M6** | **Waveform Modes & Refresh Pipeline** | **QUEUED** | 1-bit fast paging, 4-bit high quality rendering, ghosting mitigation, flashing reduction. | M3, M5 |
| **M7** | **End-to-End Release & Handoff** | **QUEUED** | Hardware validation on physical X4 Pro, firmware-handoff verification, release build packaging. | M1–M6 |

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
- **Scope & Key Decisions:**
  - Complete UI Collection and UX validation (reviewing Concept A, Concept B, and DESIGN-002 explorations) before broad native C++ firmware implementation.
  - Compare H1.1A Native Home performance/memory footprint on X4 Pro (ESP32-S3) and generic ESP32-C3 vs XOS-DL token-driven architecture.
  - Review 5-screen scope: Concept A covers Home; Concept B explored Home, Library, Reader, Files, Settings.
  - Formal Product Owner sign-off on whether to maintain H1.1A, adopt XOS-DL v1.0, or pursue a phased hybrid.
- **Status:** Ready to commence immediately upon approval of M0.

---

### M2: Screen Architecture & Full Specifications
- **Objective:** Establish pixel-exact, interaction-complete specifications for all core screens.
- **Deliverables:**
  - Definitive state matrix for Home (populated, empty, first-run).
  - Library browsing (grid, list, sorting, metadata filtering).
  - Reader and HUD overlay (paging, progress scrubber, font size adjustments).
  - File manager / storage browser (SD card directory tree, USB transfer state).
  - Settings (refresh modes, power management, Wi-Fi configuration).

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

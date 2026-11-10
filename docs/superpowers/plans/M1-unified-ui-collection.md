# Unified UI/UX Collection Implementation Plan (M1)

> **Historical — superseded (2026-11-09).** Preserved for provenance only. Do not execute this plan. Current authority is the [execution contract](../../EXECUTION_CONTRACT.md) and the [2026-11-10 consolidation plan](2026-11-10-project-consolidation.md).

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Assemble, reconcile, and validate a unified 15-frame UI/UX Collection and single navigable HTML prototype across all 5 core XTEInk OS operational areas (Home, Library, Reader, Files, Settings), synthesizing Concept A, DESIGN-002, Concept B, and XOS-DL into an e-paper feasible product baseline for Product Owner review.

**Architecture:** Non-destructive visual synthesis and interaction prototyping. Reuses existing visual assets, tokens (`docs/design/experiments/EXP-001-quiet-modular/design-system/tokens.json`), and design explorations (`xteink-os-design-002-review.zip`, `docs/design/explorations/design-002/`, `EXP-001`) without modifying embedded C++ firmware in `src/`. Establishes a cohesive visual gallery, cross-screen navigation flows, and empirical e-paper feasibility bounds (refresh rates, hit targets, memory budgets).

**Tech Stack:** HTML5, CSS3, Vanilla JS, Obra Superpowers framework (`writing-plans`, `verification-before-completion`), Python/Playwright for automated 480×800 rendering checks.

---

## Baseline & Boundaries

### Non-Destructive Synthesis Principles
1. **Zero Firmware Modifications:** No code changes in `src/`, `lib/`, `freeink-sdk/`, `test/`, or `platformio.ini`.
2. **Reconciliation, Not Invention:** Do not design Home in isolation or create a redundant design system from scratch. Harmonize existing proven patterns:
   - **Concept A / H1.1A:** Editorial layout structure and cover grid hierarchy.
   - **DESIGN-002:** Curated shelf, editorial list typography, and quiet reader immersive controls.
   - **Concept B:** 5-screen functional completeness, task continuity, and ingestion canvas empty state.
   - **XOS-DL v1.0:** 32px baseline rhythm, 48px/56px touch target contracts, token architecture, and 1-bit high-contrast rules.
3. **E-Paper Feasibility Bounds:** Strict compliance with X4 Pro physical characteristics (ESP32-S3, 8 MB PSRAM, 800×480 landscape panel mapped 480×800 portrait, GT911 capacitive touch) and generic CrossPoint compatibility (ESP32-C3, 380 KB SRAM).
4. **Single Decision Gate:** Execution culminates in one comprehensive Product Owner review gate for the unified collection.

---

## 15 Representative Frames & State Matrix

The unified collection encompasses 5 functional areas across 15 essential states:

| Area | Frame ID | Screen State / Role | Description & Reused Asset Origin |
| :--- | :---: | :--- | :--- |
| **Home** | `FR-01` | **Home — Populated (Primary)** | Reading hero with cover, progress milestone, single primary CTA, 2-item recent shelf. Reconciles Concept A, Golden Home, and XOS-DL. |
| **Home** | `FR-02` | **Home — Empty / Ingestion** | First-run onboarding canvas with 1-tap SD scan, Web Server import, and storage health. Reuses Concept B ingestion stage. |
| **Home** | `FR-03` | **Home — Unstarted / Curated** | Curated recommendations / unstarted shelf layout with title-first hierarchy. Reuses DESIGN-002 golden-home-unstarted. |
| **Library** | `FR-04` | **Library — Curated Shelf** | Editorial shelf grouping with featured reading collections and cover art. Reuses DESIGN-002 `library-curated-shelf`. |
| **Library** | `FR-05` | **Library — Editorial List** | High-density 1-column list with long metadata support (Vietnamese title wrap) and progress gauges. Reuses DESIGN-002 `library-editorial-list`. |
| **Library** | `FR-06` | **Library — Filtered Search** | Live catalog search with active query, 1-bit status badges, and dynamic pagination. Reuses Concept B search flows. |
| **Library** | `FR-07` | **Library — Empty State** | Empty collection state with SD rescan prompt and sideload guidance. Reuses Concept B empty library. |
| **Reader** | `FR-08` | **Reader — Pure Immersive** | Minimal distraction-free reading canvas; running header/footer; 0 UI chrome. Reuses DESIGN-002 `reader-immersive`. |
| **Reader** | `FR-09` | **Reader — Interactive HUD** | Center-tap HUD with chapter scrubber, progress tag (`42/280`), and settings triggers. Reuses Concept B `reader-hud`. |
| **Reader** | `FR-10` | **Reader — Table of Contents Drawer** | Modal TOC with active chapter marker, page targets, and scroll bounds. Reuses Concept B TOC modal. |
| **Reader** | `FR-11` | **Reader — Quick Settings / Font** | In-reader typography tuning modal (font scale, line height, margins) within touch bounds. Reuses DESIGN-002 `reader-controls`. |
| **Files** | `FR-12` | **Files — Root / SD Browser** | Directory tree navigation with breadcrumbs (`/sdcard/books/`), folder rows, and capacity bar. Reuses Concept B files screen. |
| **Files** | `FR-13` | **Files — Direct Sideload Launch** | EPUB file selection state with file size, metadata preview, and direct reader launch CTA. Reuses Concept B sideload flow. |
| **Settings**| `FR-14` | **Settings — Editorial List** | Grouped 1-bit settings rows (Display, Waveforms, Controls, Power) with trailing states. Reuses DESIGN-002 `settings-editorial`. |
| **Settings**| `FR-15` | **Settings — Compact Modal** | Interactive refresh interval and sunlight fading fix configuration modal. Reuses DESIGN-002 `settings-compact`. |

---

## Planned File Structure

- **Audits & Specifications:**
  - `docs/superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md`: Cross-concept reconciliation matrix mapping Concept A, DESIGN-002, Concept B, and XOS-DL into the unified spec.
  - `docs/design/specs/UNIFIED_UI_COLLECTION.md`: Comprehensive visual and interaction contract for the 15 frames.
- **Interactive Prototype & Visual Gallery:**
  - `docs/design/prototype/unified-ui-prototype.html`: Single self-contained navigable prototype supporting all 5 screens, 15 states, keyboard/touch shortcuts, and simulated e-paper flashing.
  - `docs/design/prototype/gallery.html`: Side-by-side 15-frame contact sheet and inspection gallery for Product Owner review.
- **Visual Evidence & Captures:**
  - `docs/design/prototype/captures/`: 15 pixel-exact 480×800 rendered captures verifying composition and layout integrity.
- **QA & Verification:**
  - `docs/superpowers/audits/M1-UI-COLLECTION-QA.md`: Automated and manual QA record covering touch bounds, typography wrapping, and contrast.

---

## Task Decomposition

### Task 1: Reconcile Historical Design Baselines & Author Specification

**Files:**
- Create: `docs/superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md`
- Create: `docs/design/specs/UNIFIED_UI_COLLECTION.md`

- [ ] **Step 1: Extract and audit DESIGN-002 reference files**
  Inspect `xteink-os-design-002-review.zip` and commit `c9d12b52` (`docs/design/explorations/design-002/`).
  Document exact layout geometry, typography sizes, and component boundaries.
- [ ] **Step 2: Reconcile Concept A, DESIGN-002, Concept B, and XOS-DL**
  Map conflicting patterns (e.g. 4-tab bottom dock vs. editorial utility links; cover grid vs. curated shelf).
  Synthesize the definitive rule set honoring the approved hierarchy (reading continuity hero, 1-bit high contrast, ≥44px touch targets).
- [ ] **Step 3: Author UNIFIED_UI_COLLECTION.md**
  Document the complete visual grammar, layout grid (480×800 portrait), typography scale, touch zones, and state definitions for all 15 frames.

---

### Task 2: Build the Navigable Unified HTML Prototype & Visual Gallery

**Files:**
- Create: `docs/design/prototype/unified-ui-prototype.html`
- Create: `docs/design/prototype/gallery.html`

- [ ] **Step 1: Construct the unified CSS design tokens and core component library**
  Embed XOS-DL tokens (`tokens.json`), 1-bit monochrome palette, typography scale (32px, 24px, 18px, 15px, 13px), and button styles.
- [ ] **Step 2: Implement the 5 functional screen engines**
  - **Home:** Populated (`FR-01`), Empty (`FR-02`), Unstarted (`FR-03`).
  - **Library:** Curated Shelf (`FR-04`), Editorial List (`FR-05`), Search (`FR-06`), Empty (`FR-07`).
  - **Reader:** Immersive (`FR-08`), HUD (`FR-09`), TOC Drawer (`FR-10`), Font Settings (`FR-11`).
  - **Files:** SD Directory (`FR-12`), Sideload Preview (`FR-13`).
  - **Settings:** Editorial Categorized (`FR-14`), Modal Settings (`FR-15`).
- [ ] **Step 3: Connect interactive navigation flows**
  - Home &rarr; Continue Reading &rarr; Reader (`FR-01` &rarr; `FR-08`).
  - Reader &rarr; Center Tap &rarr; HUD (`FR-08` &rarr; `FR-09`).
  - Reader HUD &rarr; TOC &rarr; Chapter Jump (`FR-09` &rarr; `FR-10` &rarr; `FR-08`).
  - Library &rarr; Search "Dune" &rarr; Filter &rarr; Tap book &rarr; Reader (`FR-05` &rarr; `FR-06` &rarr; `FR-08`).
  - Files &rarr; Navigate folder &rarr; Select EPUB &rarr; Launch Reader (`FR-12` &rarr; `FR-13` &rarr; `FR-08`).
- [ ] **Step 4: Author gallery.html contact sheet**
  Create side-by-side inspection view displaying all 15 frames simultaneously with metadata and design notes.

---

### Task 3: Render Pixel-Exact Captures & Validate E-Paper Feasibility

**Files:**
- Create: `docs/design/prototype/captures/*.png` (15 frames)
- Create: `docs/superpowers/audits/M1-UI-COLLECTION-QA.md`

- [ ] **Step 1: Capture 15 pixel-exact 480×800 screenshots**
  Use headless automation or screenshot script to capture `FR-01` through `FR-15`.
  Verify output files match exactly 480×800 dimensions.
- [ ] **Step 2: Audit touch hit target compliance**
  Verify all tappable areas (buttons, rows, tabs, sliders) are ≥44×44 CSS px (≥56px for primary CTA).
- [ ] **Step 3: Audit Vietnamese typography & diacritic wrapping**
  Verify multi-line Vietnamese text (*Nỗi Buồn Chiến Tranh* by *Bảo Ninh*) renders without ascender/descender clipping or truncation.
- [ ] **Step 4: E-paper constraint & memory budget audit**
  - Verify pure 1-bit monochrome compatibility (no color, blur, or complex gradients).
  - Check DOM weight and memory footprint to ensure feasibility on ESP32-S3 (8 MB PSRAM) and ESP32-C3 (380 KB SRAM).
- [ ] **Step 5: Write M1-UI-COLLECTION-QA.md report**
  Catalog test results, verified criteria, and identified nuances.

---

### Task 4: Product Owner Review Gate & Closeout

**Files:**
- Create: `docs/superpowers/audits/M1-UI-COLLECTION-REVIEW.md`
- Update: `docs/PROJECT_STATUS.md`

- [ ] **Step 1: Run Git syntax and whitespace verification**
  Execute `git diff --check` and verify exit code 0.
- [ ] **Step 2: Verify repository link integrity**
  Ensure all paths use portable relative links with zero machine-specific paths.
- [ ] **Step 3: Compile Product Owner Review Package**
  Summarize the 15 frames, navigation flows, and e-paper verification in `docs/superpowers/audits/M1-UI-COLLECTION-REVIEW.md`.
- [ ] **Step 4: Present Review Gate and STOP**
  Halt execution for Product Owner sign-off before proceeding to native C++ firmware implementation (M3).

---

## Rollback & Safety Procedures

1. All prototype and specification work is strictly confined to `docs/design/prototype/`, `docs/design/specs/`, and `docs/superpowers/`.
2. Firmware source in `src/`, `lib/`, `freeink-sdk/`, and `platformio.ini` is untouched.
3. If reverted, discarding the `chore/m1-ui-collection` branch safely restores the working tree without affecting any prior commits or backups.

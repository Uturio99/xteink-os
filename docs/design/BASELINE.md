# XTEInk OS — Canonical Design Baseline Specification

> **Authority Document:** Single Source of Truth for Design Baselines & Experiments  
> **Repository:** `xteink-os`  
> **Status:** Locked Baseline vs Experimental Exploration  
> **Date:** 2026-11-02 (Milestone M0)  

---

## 1. Executive Summary

This document establishes the official status of all design specifications, HTML prototypes, and user interface implementations across the `xteink-os` project.

AI agents, contributors, and maintainers must distinguish between:
1. **Production-Approved Baseline:** Officially accepted designs that guide production firmware.
2. **Experimental Concepts:** Research and design explorations that are **not** approved for production merging.
3. **Documented-Only Assets:** Historical references noted in audits but not present in the current repository tree.

---

## 2. Status Matrix

| Artifact Group | Primary Location / Ref | Official Status | Merged to Production? | PO Approval Status |
| :--- | :--- | :---: | :---: | :---: |
| **Concept A / Home v1.1 / H1.1A** | Branch `checkpoint/h1.1a-native-home` (`877461d3`) & `codex/xteink-os-checkpoint-2026-10-31` (`0861aeaf`) | **APPROVED PRODUCTION BASELINE** | **YES** (Implemented in `src/components/`) | **APPROVED** |
| **Concept B / EXP-001** | `docs/design/experiments/EXP-001-quiet-modular/prototype.html` | **EXPERIMENTAL** | **NO** | **UNAPPROVED / IN EVALUATION** |
| **XOS-DL v1.0 Design System** | `docs/design/experiments/EXP-001-quiet-modular/design-system/` | **PROPOSED EXPERIMENTAL SPEC** | **NO** | **UNAPPROVED / IN EVALUATION** |
| **AD-01 Calm Editorial Home** | `docs/design/experiments/EXP-001-quiet-modular/home-direction.html` | **EXPERIMENTAL STUDY** | **NO** | **UNAPPROVED / IN EVALUATION** |
| **Home Pilot (XOS-DL)** | `docs/design/experiments/EXP-001-quiet-modular/home-xosdl-pilot.html` | **EXPERIMENTAL PILOT** | **NO** | **UNAPPROVED / IN EVALUATION** |
| **DESIGN-002 Assets** | Historical external host path (`D:\...`) | **DOCUMENTED ONLY / MISSING EVIDENCE** | **NO** | **AWAITING EXTERNAL RECOVERY** |

---

## 3. Approved Production Baseline: H1.1A Native Home

### Definition & Origin
- **Git Checkpoints:**
  - Design & Specification: Commit `0861aeaf` on branch `codex/xteink-os-checkpoint-2026-10-31`. Contains `home-v1.1.html`, `DESIGN_SYSTEM_v1.0.md`, `HOME_SPEC_v1.0.md`, and `IMPLEMENTATION_HANDOFF.md`.
  - Firmware Implementation: Commit `877461d3` on branch `checkpoint/h1.1a-native-home`. Contains native C++ implementation within `src/components/X4ProHomeLayout.h`, `src/components/CoverGridHomeUi.cpp`, and associated FreeInk drivers.

### Baseline Principles
- Optimized directly for the ESP32-C3 hardware constraints (380 KB RAM, no external PSRAM).
- Direct cover grid layout with minimal memory overhead and zero dynamic heap reallocations during home navigation.
- Verified on hardware display controllers.

---

## 4. Experimental Explorations: Concept B & XOS-DL v1.0

### Definition & Scope
- **Directory:** [`docs/design/experiments/EXP-001-quiet-modular/`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/design/experiments/EXP-001-quiet-modular/)
- **Experiment Status:** **EXPERIMENTAL / NOT APPROVED**
- **Core Assets:**
  - `prototype.html`: 5-screen interactive prototype exploring Home, Library, Reader, Files, and Settings.
  - `design-system/`: The XOS-DL v1.0 authored design specification (Foundations, Components, Composition, Interaction, Tokens, Specimen).
  - `home-direction.html` & `home-xosdl-pilot.html`: High-fidelity standalone Home art direction and pilot implementations.
  - `screenshots/`: 14 verified 480×800 pixel-exact visual captures.

### Strict Boundary Rules
- **No Production Code Leaks:** No token structures, styling, or layouts from EXP-001 may be introduced into `src/` without formal Milestone M1 Product Owner approval.
- **Hypothesis Status:** Claims regarding task efficiency, cognitive load, or e-paper refresh superiority remain hypotheses pending empirical hardware validation.

---

## 5. Documented-Only Assets: DESIGN-002

### Context & Status
Prior audit notes and design logs referenced a suite of editorial designs:
- `golden-home.html`
- `library-editorial-list.html`
- `reader-immersive.html`
- `reader-controls.html`
- `settings-editorial.html`
- `COMPOSITION_REVIEW.md`
- `P0_REFERENCE_FONT_LOCK.md`

### Audit Findings
- **Filesystem Verification:** These files do not exist anywhere in the tracked repository or untracked local tree. They were authored on an external host path (`D:\...`) during prior design phases.
- **Policy:** Classified as **DOCUMENTED ONLY / REVIEW**.
- **Rule for AI Agents:** Do not fabricate, hallucinate, or simulate these files. If these historical references are required for Milestone M1 reconciliation, they must be retrieved from external archives by the Product Owner.

---

## 6. Guidance for AI Agents & Developers

1. When instructed to implement or fix firmware, **target the Approved Baseline** (`src/components/CoverGridHomeUi.cpp`).
2. When instructed to explore design or refine Concept B, **work strictly inside** `docs/design/experiments/EXP-001-quiet-modular/`.
3. Never merge, promote, or replace baseline files without an explicit instruction referencing a Product Owner approval record.

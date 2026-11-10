# M0 — Baseline & Repository Inventory

> **Historical — superseded (2026-11-09).** This records the 2026-11-02 M0 baseline
> (`G:\AI-Setup\...`, `chore/xos-project-consolidation`). It is provenance only. The
> canonical workspace is `D:\Work\AI\XTEInk X4 Pro\xteink-os`; current state is in the
> [CONS-01 inventory](CONS-01-INVENTORY.md), and authority is the
> [execution contract](../../EXECUTION_CONTRACT.md).

> **Document ID:** M0-AUDIT-001  
> **Milestone:** M0 (Safe Project Consolidation)  
> **Repository:** `xteink-os` (Root: `G:\AI-Setup\XTEInk-X4-Pro\xteink-os`)  
> **Active Branch:** `chore/xos-project-consolidation` (Branched from `checkpoint/h1.1a-native-home` at `877461d3`)  
> **Remote Upstream:** `https://github.com/crosspoint-reader/crosspoint-reader.git`  
> **Remote Origin:** `https://github.com/Uturio99/xteink-os.git`  
> **Date:** 2026-11-02  
> **Auditor:** Technical Program Engineer (Gemini 3.8 Flash)  

---

## 1. Executive Summary & Git Baseline

- **Repository Root:** `G:/AI-Setup/XTEInk-X4-Pro/xteink-os`
- **Current HEAD Commit:** `877461d36006c42d4997f622d97d6a9d8f8a7e72` (`feat(x4pro): implement H1.1A native home layout`)
- **Key Historical Commits / Branches:**
  - `checkpoint/h1.1a-native-home` (`877461d3`): Native C++ implementation of H1.1A Home layout in `src/components/CoverGridHomeUi.cpp`, `src/components/X4ProHomeLayout.h`, and tests.
  - `codex/xteink-os-checkpoint-2026-10-31` (`0861aeaf`): Checkpoint of XTEInk OS design assets containing approved Concept A (`home-v1.1.html`, `DESIGN_SYSTEM_v1.0.md`, `HOME_SPEC_v1.0.md`, `IMPLEMENTATION_HANDOFF.md`, etc.).
  - `develop` (`ad53eb64`): Integration branch upstream base.
- **Submodule Status:**
  - `freeink-sdk` at commit `946e6ac3d7a9d590ba3224428e6183d55ac7917c` (`remotes/origin/feat/boox-picco-131-g946e6ac`). Intact, clean, unmodified.
- **Pre-Consolidation Working Tree State:**
  - Tracked modification: `USER_GUIDE.md` (Theme documentation update for X4 Pro).
  - Untracked files: `docs/design/` (EXP-001 Concept B experimental files).
  - Pre-cleanup non-destructive backup created at: `C:\Users\Welcome\.gemini\antigravity\brain\ac5660be-68ec-484c-9fd8-179e8e1d7c07\scratch\backup\m0-pre-consolidation\`.

---

## 2. Asset Classification Taxonomy

In accordance with M0 Operating Rules:
- **KEEP:** Authoritative, required project specs, active code, core docs, or locked references.
- **ORGANIZE:** Active experimental files safe to relocate into coherent modular subdirectories.
- **ARCHIVE:** Historical checkpoints, duplicate copies, or superseded versions preserved for audit trail.
- **GENERATED:** Headless browser screenshots, temporary test outputs, or build caches.
- **REVIEW:** Uncertain assets or external dependencies requiring Product Owner evaluation.

---

## 3. Comprehensive Inventory Table

### 3.1 Project Design Assets & Experiments (`docs/design/`)

| Asset Path | Type | Size | Status | Classification | Context & Reference Notes |
| :--- | :--- | :---: | :---: | :---: | :--- |
| `docs/design/experiments/README.md` | Markdown | 933 B | Untracked | **KEEP** | Experiments directory index cataloging EXP-001. |
| `docs/design/experiments/EXP-001-quiet-modular/README.md` | Markdown | 5.8 KB | Untracked | **KEEP** | EXP-001 experiment summary (Concept B: Quiet Modular). |
| `docs/design/experiments/EXP-001-quiet-modular/QA.md` | Markdown | 6.7 KB | Untracked | **KEEP** | QA and verification record for Concept B prototype. |
| `docs/design/experiments/EXP-001-quiet-modular/prototype.html` | HTML | 93.3 KB | Untracked | **KEEP** | Full 5-screen interactive browser prototype (Home, Library, Reader, Files, Settings). |
| `docs/design/experiments/EXP-001-quiet-modular/design-notes.md` | Markdown | 10.5 KB | Untracked | **KEEP** | Decision rationale and task efficiency comparison matrix for Concept B. |
| `docs/design/experiments/EXP-001-quiet-modular/report.md` | Markdown | 5.8 KB | Untracked | **KEEP** | Preserved Concept B deliverable report artifact. |
| `docs/design/experiments/EXP-001-quiet-modular/art-direction.md` | Markdown | 2.5 KB | Untracked | **KEEP** | AD-01 Art direction rationale for Calm Editorial Utility. |
| `docs/design/experiments/EXP-001-quiet-modular/home-direction.html` | HTML | 29.3 KB | Untracked | **KEEP** | Standalone 480×800 Home study for AD-01. |
| `docs/design/experiments/EXP-001-quiet-modular/home-xosdl-pilot.html` | HTML | 15.5 KB | Untracked | **KEEP** | Home Pilot implementation executing XOS-DL v1.0 specifications. |
| `docs/design/experiments/EXP-001-quiet-modular/DS-PILOT-QA.md` | Markdown | 6.3 KB | Untracked | **KEEP** | QA compliance audit and visual diff report for Home pilot. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/README.md` | Markdown | 4.8 KB | Untracked | **KEEP** | Authoritative XOS-DL v1.0 specification entrypoint. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/AGENT_CONTRACT.md` | Markdown | 2.2 KB | Untracked | **KEEP** | Agent boundary and workflow rules for XOS-DL v1.0. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/FOUNDATIONS.md` | Markdown | 5.8 KB | Untracked | **KEEP** | Design tokens, spatial scale, typography scale, 1-bit palette. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/COMPOSITION.md` | Markdown | 5.8 KB | Untracked | **KEEP** | Screen archetypes, vertical bands, and composition rules. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/COMPONENTS.md` | Markdown | 3.7 KB | Untracked | **KEEP** | Component specifications, geometry, states, and touch bounds. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/INTERACTION.md` | Markdown | 3.0 KB | Untracked | **KEEP** | Touch ergonomics, e-paper refresh contracts, navigation map. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/REFERENCES.md` | Markdown | 5.1 KB | Untracked | **KEEP** | Verified external UI references (Apple HIG, Apple Books, Krono, Kobo). |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/ACCEPTANCE.md` | Markdown | 3.3 KB | Untracked | **KEEP** | Gate criteria and release requirements for XOS-DL v1.0. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/tokens.json` | JSON | 2.0 KB | Untracked | **KEEP** | Machine-readable proposed token registry v1. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/specimen.html` | HTML | 18.4 KB | Untracked | **KEEP** | Offline visual specimen study for XOS-DL v1.0. |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/previews/*` | PNG | ~500 KB | Untracked | **KEEP** | Specimen preview renders and binary check images. |
| `docs/design/experiments/EXP-001-quiet-modular/screenshots/*` | PNG | ~400 KB | Untracked | **KEEP** | Verified 480×800 screenshots of Concept B, AD-01, and Pilot. |
| `docs/design/experiments/EXP-001-quiet-modular/ACCEPTANCE.md` | Markdown | 3.3 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/ACCEPTANCE.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/AGENT_CONTRACT.md` | Markdown | 2.2 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/AGENT_CONTRACT.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/COMPONENTS.md` | Markdown | 3.7 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/COMPONENTS.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/COMPOSITION.md` | Markdown | 5.8 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/COMPOSITION.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/FOUNDATIONS.md` | Markdown | 5.8 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/FOUNDATIONS.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/INTERACTION.md` | Markdown | 3.0 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/INTERACTION.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/REFERENCES.md` | Markdown | 5.1 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/REFERENCES.md`. |
| `docs/design/experiments/EXP-001-quiet-modular/specimen.html` | HTML | 18.4 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/specimen.html`. |
| `docs/design/experiments/EXP-001-quiet-modular/tokens.json` | JSON | 2.0 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/tokens.json`. |
| `docs/design/experiments/EXP-001-quiet-modular/previews/` | Directory | ~450 KB | Untracked | **ARCHIVE** | Stray root duplicate of `design-system/previews/`. |

---

### 3.2 Historical Checkpoint Assets (`codex/xteink-os-checkpoint-2026-10-31` / commit `0861aeaf`)

| Asset Path (on branch) | Type | Status | Classification | Context & Reference Notes |
| :--- | :--- | :---: | :---: | :--- |
| `docs/design/home-v1.1.html` | HTML | Committed on `0861aeaf` | **KEEP** | **Authoritative Approved Reference for Concept A** (Home v1.1). |
| `docs/design/DESIGN_SYSTEM_v1.0.md` | Markdown | Committed on `0861aeaf` | **KEEP** | Approved Design System specification for Concept A / H1.1A. |
| `docs/design/HOME_SPEC_v1.0.md` | Markdown | Committed on `0861aeaf` | **KEEP** | Approved master layout coordinate map for Concept A / H1.1A. |
| `docs/design/IMPLEMENTATION_HANDOFF.md` | Markdown | Committed on `0861aeaf` | **KEEP** | Engineering handoff specification to Codex for native implementation. |
| `docs/design/VISUAL_DIFFERENCE_REPORT_v1.1.md` | Markdown | Committed on `0861aeaf` | **KEEP** | Audit report validating v1.0 to v1.1 visual fidelity corrections. |
| `docs/design/home-v1.0.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical predecessor prototype (v1.0). |
| `docs/design/home-v0.5.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical predecessor prototype (v0.5). |
| `docs/design/home-v0.4.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical predecessor prototype (v0.4). |
| `docs/design/home-editorial-focus.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical exploration (Editorial Focus). |
| `docs/design/home-editorial-utility.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical exploration (Editorial Utility). |
| `docs/design/home-primary.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical exploration (Primary v0.3). |
| `docs/design/home-alternative.html` | HTML | Committed on `0861aeaf` | **ARCHIVE** | Historical exploration (Alternative v0.3). |
| `docs/design/DESIGN_SYSTEM_v0.2..v0.5.md` | Markdown | Committed on `0861aeaf` | **ARCHIVE** | Historical specifications v0.2 through v0.5. |
| `docs/design/CHANGELOG_v0.4..v0.5.md` | Markdown | Committed on `0861aeaf` | **ARCHIVE** | Historical changelogs. |
| `docs/design/xteink-os-design-v1.1.zip` | Archive | Committed on `0861aeaf` | **ARCHIVE** | Standalone deliverable zip archive. |
| `xteink-os-design-v0.2..v1.1.zip` (root) | Archives | Committed on `0861aeaf` | **ARCHIVE** | Historical root zip archives. |

---

### 3.3 Documented-Only External Assets (DESIGN-002)

| Asset Name | Status | Classification | Description & Action Required |
| :--- | :---: | :---: | :--- |
| `golden-home.html` | `DOCUMENTED_ONLY` | **REVIEW** | Recorded in prior DESIGN-002 audit (`docs/design/explorations/design-002/`). Awaits recovery from external archive. |
| `library-editorial-list.html` | `DOCUMENTED_ONLY` | **REVIEW** | Documented editorial library composition. Awaits asset recovery. |
| `reader-immersive.html` | `DOCUMENTED_ONLY` | **REVIEW** | Documented immersive reading view. Awaits asset recovery. |
| `reader-controls.html` | `DOCUMENTED_ONLY` | **REVIEW** | Documented in-book reader HUD. Awaits asset recovery. |
| `settings-editorial.html` | `DOCUMENTED_ONLY` | **REVIEW** | Documented editorial settings view. Awaits asset recovery. |
| `COMPOSITION_REVIEW.md` | `DOCUMENTED_ONLY` | **REVIEW** | Documented review notes for DESIGN-002. Awaits asset recovery. |
| `P0_REFERENCE_FONT_LOCK.md` | `DOCUMENTED_ONLY` | **REVIEW** | Documented font lock record for DESIGN-002. Awaits asset recovery. |

---

### 3.4 Runtime Source & Firmware Files (Checked Out on `checkpoint/h1.1a-native-home`)

| File Path | Status | Classification | Purpose |
| :--- | :---: | :---: | :--- |
| `src/activities/home/HomeActivity.cpp` | Tracked | **KEEP** | Native Home activity controller. |
| `src/components/CoverGridHomeUi.cpp` | Tracked | **KEEP** | Native H1.1A editorial layout renderer. |
| `src/components/CoverGridHomeUi.h` | Tracked | **KEEP** | Header interface for H1.1A rendering. |
| `src/components/X4ProHomeLayout.h` | Tracked | **KEEP** | Layout integer geometry constants for H1.1A. |
| `src/components/UITheme.cpp` | Tracked | **KEEP** | Theme configuration and X4 Pro layout binding. |
| `test/x4pro_home_layout/X4ProHomeLayoutTest.cpp` | Tracked | **KEEP** | Layout boundary regression tests. |
| `USER_GUIDE.md` | Tracked (Modified) | **KEEP** | User guide with X4 Pro native theme notice. |

---

## 4. Duplicate & Misplaced Assets Findings

1. **Accidental Spec Duplication in EXP-001 Root:**
   10 files and 1 directory (`ACCEPTANCE.md`, `AGENT_CONTRACT.md`, `COMPONENTS.md`, `COMPOSITION.md`, `FOUNDATIONS.md`, `INTERACTION.md`, `REFERENCES.md`, `specimen.html`, `tokens.json`, `previews/`) exist both at `docs/design/experiments/EXP-001-quiet-modular/` AND inside `docs/design/experiments/EXP-001-quiet-modular/design-system/`.
   - *Resolution:* Authoritative version stays in `design-system/`. The duplicate root copies will be safely archived into `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/`.
2. **Scattered Scraping / Automation Scripts:**
   Headless screenshot generation scripts created during testing live in the conversation scratch directory (`C:\Users\Welcome\.gemini\antigravity\brain\ac5660be-68ec-484c-9fd8-179e8e1d7c07\scratch\`), which is already external to the git repository and properly isolated.

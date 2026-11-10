# M0 Safe Project Consolidation Implementation Plan

> **Historical — superseded (2026-11-09).** Preserved for provenance only. Do not execute this plan. Current authority is the [execution contract](../../EXECUTION_CONTRACT.md) and the [2026-11-10 consolidation plan](2026-11-10-project-consolidation.md).

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Safely audit, consolidate, and document the XTEInk OS project structure without losing historical work, altering production firmware, or making premature design decisions.

**Architecture:** Non-destructive preservation and reorganization framework. The repository establishes an isolated consolidation branch, archives duplicate artifacts into a structured subfolder with cryptographic SHA256 verification, and creates three authoritative entrypoints (`PROJECT_MAP.md`, `PROJECT_STATUS.md`, `BASELINE.md`) to clearly separate approved production baselines from experimental concepts.

**Tech Stack:** Git, PowerShell/Bash, Markdown, Obra Superpowers framework (`writing-plans`, `verification-before-completion`).

---

## Baseline & Boundaries

### Non-Destructive Principles
1. **Preservation Outranks Tidiness:** Zero files are deleted or destroyed.
2. **Firmware Integrity:** No changes to `src/`, `lib/`, `freeink-sdk/`, `test/`, or `platformio.ini`.
3. **No Unapproved Baseline Promotion:** Concept B / EXP-001 / XOS-DL v1.0 remains strictly experimental until Product Owner sign-off.
4. **Git Safety:** No `git reset --hard`, no `git clean`, no force push, no rebasing, and no direct commits without explicit user approval.

### Separation of Concerns
- **Approved Baseline:** H1.1A Native Home (`checkpoint/h1.1a-native-home` at commit `877461d3` + `codex/xteink-os-checkpoint-2026-10-31` at commit `0861aeaf`).
- **Design Exploration:** Concept B / EXP-001 / XOS-DL v1.0 (`docs/design/experiments/EXP-001-quiet-modular/`). Experimental; not approved for production merge.
- **Documented-Only Assets:** DESIGN-002 references (`golden-home.html`, `library-editorial-list.html`, `reader-immersive.html`, `reader-controls.html`, `settings-editorial.html`, `COMPOSITION_REVIEW.md`, `P0_REFERENCE_FONT_LOCK.md`). Documented in prior audits as located on external development host storage; awaiting recovery.

---

## Planned File Structure

- **Audits & Inventory:**
  - `docs/superpowers/audits/M0-INVENTORY.md`: Comprehensive repository inventory and classification.
  - `docs/superpowers/audits/M0-MOVE-MANIFEST.md`: Cryptographically verified move manifest with SHA256 checksums.
- **Plans:**
  - `docs/superpowers/plans/M0-project-consolidation.md`: This execution plan.
- **Entrypoints & Project Documentation:**
  - `docs/PROJECT_MAP.md`: AI agent orientation guide and repository navigation tree.
  - `docs/PROJECT_STATUS.md`: Tracking dashboard across milestones M0 through M7.
  - `docs/design/BASELINE.md`: Canonical design truth separating approved baseline from experiments.
- **Archival:**
  - `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/`: Non-destructive destination for redundant root copies.

---

## Task Decomposition

### Task 1: Environment Inspection and Baseline Inventory

**Files:**
- Create: `docs/superpowers/audits/M0-INVENTORY.md`

- [x] **Step 1: Inspect repository state and refs**
  Run: `git branch --show-current; git status --short; git rev-parse HEAD`
  Expected: Working branch identified, uncommitted changes cataloged, HEAD recorded.

- [x] **Step 2: Inventory design and root assets**
  Run: `Get-ChildItem -Path "docs/design" -Recurse`
  Expected: Complete catalog of `docs/design/`, `EXP-001-quiet-modular/`, `design-system/`, and previews.

- [x] **Step 3: Draft and write M0-INVENTORY.md**
  Classify every item as `KEEP`, `ORGANIZE`, `ARCHIVE`, `GENERATED`, or `REVIEW`.
  Verify all files from commits `0861aeaf` and `877461d3` are mapped.

---

### Task 2: Safety Backup and Move Manifest Preparation

**Files:**
- Create: `docs/superpowers/audits/M0-MOVE-MANIFEST.md`
- Backup: Copy modified/untracked assets to external brain directory

- [x] **Step 1: Create local external safety backup**
  Execute non-destructive copy of uncommitted working tree assets (`USER_GUIDE.md`, `docs/design/`) to brain scratch backup directory.
  Run: `Copy-Item -Path "docs/design" -Destination "$env:USERPROFILE\.gemini\antigravity\brain\ac5660be-68ec-484c-9fd8-179e8e1d7c07\scratch\backup\m0-pre-consolidation\docs\design" -Recurse`
  Expected: External backup verified on disk.

- [x] **Step 2: Create isolated consolidation branch**
  Run: `git checkout -b chore/xos-project-consolidation`
  Expected: Switched to new branch `chore/xos-project-consolidation`.

- [x] **Step 3: Document move manifest**
  Record source, target, action type, and rationale for all candidate files in `docs/superpowers/audits/M0-MOVE-MANIFEST.md`.

---

### Task 3: Cryptographic Verification and Non-Destructive Archival

**Files:**
- Modify: `docs/design/experiments/EXP-001-quiet-modular/` (relocate root spec duplicates)
- Create: `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/`
- Update: `docs/superpowers/audits/M0-MOVE-MANIFEST.md`

- [x] **Step 1: Compute SHA256 checksums of root spec duplicates vs design-system canonical copies**
  Run: `Get-FileHash` on root files vs `design-system/` files.
  Expected: 100% hash parity confirmed across all 9 markdown/spec files and 10 preview images.

- [x] **Step 2: Relocate root duplicate files to archive folder**
  Execute: `Move-Item` for `ACCEPTANCE.md`, `AGENT_CONTRACT.md`, `COMPONENTS.md`, `COMPOSITION.md`, `FOUNDATIONS.md`, `INTERACTION.md`, `REFERENCES.md`, `specimen.html`, `tokens.json`, and `previews/` into `archive/root-spec-duplicates/`.
  Expected: Files moved without deletion; authoritative `design-system/` untouched.

- [x] **Step 3: Record exact SHA256 hashes in M0-MOVE-MANIFEST.md**
  Update `docs/superpowers/audits/M0-MOVE-MANIFEST.md` with explicit hash table.

---

### Task 4: Establish Canonical Project Entrypoints

**Files:**
- Create: `docs/PROJECT_MAP.md`
- Create: `docs/PROJECT_STATUS.md`
- Create: `docs/design/BASELINE.md`

- [x] **Step 1: Author `docs/PROJECT_MAP.md`**
  Detail repository architecture, directory layout, reading order for AI agents (firmware vs design vs experiments), and maintenance rules.
  Verify path links match actual filesystem paths.

- [x] **Step 2: Author `docs/PROJECT_STATUS.md`**
  Track progression from M0 to M7:
  - M0: Safe Project Consolidation (In Progress)
  - M1: Baseline Reconciliation & Target Selection (Pending PO review)
  - M2: Complete Screen Specifications (Pending)
  - M3: Native C++ Home & Navigation Architecture (Pending)
  - M4: Font Subsystem & Typographic Hierarchy (Pending)
  - M5: Touch Driver, Hit Targets & Event Bus (Pending)
  - M6: Waveform Integration & Display Pipeline (Pending)
  - M7: End-to-End Release & Handoff (Pending)

- [x] **Step 3: Author `docs/design/BASELINE.md`**
  State unequivocally:
  - Approved Baseline: H1.1A native home as implemented in `src/components/CoverGridHomeUi.cpp` and `codex/xteink-os-checkpoint-2026-10-31`.
  - Experimental Exploration: Concept B / EXP-001 / XOS-DL v1.0. Must NOT be merged into production without formal sign-off.
  - Documented-Only: DESIGN-002 assets pending external recovery.

---

### Task 5: Verification and Final Safety Audit

**Files:**
- Inspect: Full repository working tree

- [x] **Step 1: Verify file existence and link integrity**
  Run: Check that all canonical files, moved archive files, and newly created entrypoints exist.
  Expected: All paths resolve correctly.

- [x] **Step 2: Run Git whitespace and syntax check**
  Run: `git diff --check`
  Expected: Clean exit code 0; zero whitespace errors or merge conflict markers.

- [x] **Step 3: Verify working tree status**
  Run: `git status --short`
  Expected: Only expected new files in `docs/` and modified `USER_GUIDE.md` present. No firmware files modified, deleted, or staged.

- [x] **Step 4: Confirm zero deletions**
  Run: `git diff --diff-filter=D --stat`
  Expected: Empty output; zero deleted files.

---

## Rollback & Safety Procedures

If at any point the Product Owner or engineer requests reversion:
1. All moved files can be restored from `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/` back to root via `Move-Item`.
2. The entire consolidation branch can be abandoned safely:
   `git checkout develop`
   `git branch -D chore/xos-project-consolidation`
3. The pre-consolidation external backup remains untouched at:
   `%USERPROFILE%\.gemini\antigravity\brain\ac5660be-68ec-484c-9fd8-179e8e1d7c07\scratch\backup\m0-pre-consolidation\`

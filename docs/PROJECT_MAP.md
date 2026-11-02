# XTEInk OS — Project Map & AI Agent Navigation Guide

> **Target Audience:** Human maintainers and AI programming agents (Gemini, Antigravity, Claude, Codex, DeepSeek).  
> **Repository:** `xteink-os` (CrossPoint Reader firmware & design ecosystem).  
> **Hardware Baseline:** Xteink X4 Pro / ESP32-C3 (380 KB usable SRAM, no external PSRAM, 480×800 1-bit/4-bit e-paper display).  
> **Last Updated:** 2026-11-02 (Milestone M0)

---

## 1. Executive Summary & Core Rules

CrossPoint Reader / XTEInk OS is open-source e-reader firmware providing a fast, lightweight reading experience. Memory discipline on the ESP32-C3 is paramount: every heap allocation, framebuffer operation, and task allocation must be justified and bounded.

### Cardinal Operating Rules for AI Agents:
1. **Preservation Outranks Tidiness:** Never delete, clean, or discard user files or uncommitted work.
2. **Consult Baseline First:** Before touching UI code, read [`docs/design/BASELINE.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/design/BASELINE.md) to distinguish production-approved designs from unapproved experiments.
3. **No Unapproved Firmware Merges:** Experiments under `docs/design/experiments/` (e.g., Concept B / XOS-DL v1.0) must **not** be incorporated into `src/` without explicit Product Owner approval.
4. **Mandatory Firmware Handoff:** Any logical change to C++ firmware, build configuration, or partition tables requires following `.agents/skills/firmware-handoff/SKILL.md`.

---

## 2. Repository Directory Map

```text
xteink-os/
├── .agents/                        # Repository-local agent rules, skills, and checklists
│   ├── rules/                      # Subsystem architecture and coding constraints
│   │   ├── architecture-hal.md     # HAL contracts, drivers, FreeInk SDK boundaries
│   │   ├── coding-standards.md     # Embedded C++ standards and naming rules
│   │   ├── hardware-resources.md   # ESP32-C3 memory limits (380 KB RAM, no PSRAM)
│   │   ├── testing-debugging.md    # Unit test and verification guidelines
│   │   └── ui-activities.md        # UI activity lifecycle and input routing
│   └── skills/                     # Local engineering review skills
├── .github/                        # GitHub Actions CI workflows and issue templates
├── archive/                        # Historical repository archives and deprecated prototypes
├── docs/                           # Central documentation repository
│   ├── design/                     # Design baselines, specifications, and experiments
│   │   ├── BASELINE.md             # CANONICAL TRUTH: Approved vs experimental designs
│   │   └── experiments/            # Isolated design explorations (NOT production code)
│   │       ├── README.md           # Experiments registry and evaluation criteria
│   │       └── EXP-001-quiet-modular/ # Concept B / XOS-DL v1.0 exploration
│   │           ├── README.md       # Experiment status and interaction scope
│   │           ├── QA.md           # Verified functionality and known defects
│   │           ├── prototype.html  # 5-screen interactive prototype
│   │           ├── design-system/  # Authoritative XOS-DL v1.0 design system
│   │           ├── screenshots/    # Rendered 480×800 reference captures
│   │           └── archive/        # Preserved historical duplicates
│   ├── superpowers/                # Obra Superpowers engineering management
│   │   ├── audits/                 # Inventories, move manifests, risk analyses
│   │   └── plans/                  # Structured milestone implementation plans
│   ├── PROJECT_MAP.md              # THIS FILE: Architecture map and reading guide
│   └── PROJECT_STATUS.md           # Milestone roadmap (M0 through M7)
├── freeink-sdk/                    # Pinned FreeInk SDK submodule (HAL, drivers, UI base)
├── lib/                            # Firmware support libraries and rendering utilities
├── src/                            # Production native C++ firmware source
│   ├── activities/                 # User-facing activities (Reader, Library, Settings)
│   ├── components/                 # UI components (X4ProHomeLayout, CoverGridHomeUi)
│   ├── hal/                        # Device hardware abstraction layer implementations
│   ├── models/                     # Data structures (Book, Metadata, Settings)
│   ├── services/                   # Background services (Storage, Battery, Power)
│   └── main.cpp                    # Application entrypoint and FreeRTOS setup
├── test/                           # Native host unit tests and mocks
├── platformio.ini                  # PlatformIO target environments and build flags
├── AGENTS.md                       # High-level firmware instructions and agent contracts
├── README.md                       # Project introduction and user setup instructions
└── USER_GUIDE.md                   # End-user feature and configuration guide
```

---

## 3. Reading Order by Task Domain

### A. Firmware & Embedded C++ Development
When implementing features, fixing crashes, or modifying native components:
1. [`AGENTS.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/AGENTS.md) — Fundamental rules and ESP32-C3 baseline requirements.
2. [`.agents/rules/hardware-resources.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/.agents/rules/hardware-resources.md) — Heap allocation discipline, buffer sizing, and memory budgets.
3. [`.agents/rules/architecture-hal.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/.agents/rules/architecture-hal.md) — FreeInk SDK contract and driver boundaries.
4. [`src/components/`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/src/components/) & [`src/activities/`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/src/activities/) — Existing native implementations.
5. [`platformio.ini`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/platformio.ini) — Build environments and compile-time flags.

### B. UI Architecture & Design Work
When evaluating layouts, adding screens, or auditing visual polish:
1. [`docs/design/BASELINE.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/design/BASELINE.md) — **MANDATORY FIRST READ**. Verifies what is approved vs what is an experiment.
2. Approved Baseline: H1.1A Native Home (`checkpoint/h1.1a-native-home` & `codex/xteink-os-checkpoint-2026-10-31`).
3. Experimental Concepts: [`docs/design/experiments/EXP-001-quiet-modular/`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/design/experiments/EXP-001-quiet-modular/) — Concept B and XOS-DL v1.0. Read `README.md` and `design-system/AGENT_CONTRACT.md`.
4. [`.agents/rules/ui-activities.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/.agents/rules/ui-activities.md) — Activity lifecycle, touch routing, and partial refresh contracts.

### C. Program Management & Milestone Progress
When tracking deliverables or planning next phases:
1. [`docs/PROJECT_STATUS.md`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/PROJECT_STATUS.md) — Progress tracker for Milestones M0 through M7.
2. [`docs/superpowers/plans/`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/superpowers/plans/) — Current active and archived execution plans.
3. [`docs/superpowers/audits/`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/superpowers/audits/) — Move manifests and inventory logs.

---

## 4. Safety & Contribution Guidelines

- **Submodules:** Never update `freeink-sdk` without verifying compatibility with the pinned commit.
- **Git Hygiene:** Work in dedicated topic branches (`feat/*`, `fix/*`, `chore/*`). Integration work targets `develop`.
- **Review Protocol:** All changes affecting firmware builds must undergo the mandatory checks in `firmware-handoff`. PR descriptions must be written by humans; agents provide technical bullet points only.

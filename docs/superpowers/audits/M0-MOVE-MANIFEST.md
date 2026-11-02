# M0 — Move & Reorganization Manifest

> **Document ID:** M0-MANIFEST-001  
> **Milestone:** M0 (Safe Project Consolidation)  
> **Repository:** `xteink-os`  
> **Safety Policy:** Non-destructive reorganization. Zero files deleted. Historical assets preserved.  
> **Date:** 2026-11-02  

---

## 1. Executed Moves & Archival Actions

The following table records all exact source-to-destination operations executed during M0 consolidation, including SHA256 verification hashes proving 100% data integrity and zero data loss.

| Item ID | Source Path | Destination Path | SHA256 Digest | Action Type | Rationale & Safety Check |
| :---: | :--- | :--- | :--- | :---: | :--- |
| **M-01** | `docs/design/experiments/EXP-001-quiet-modular/ACCEPTANCE.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/ACCEPTANCE.md` | `e20f176f209ac6e837a43abf8c0dd6c1aa1c9967d471c67c889f0748755e059e` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/ACCEPTANCE.md`. |
| **M-02** | `docs/design/experiments/EXP-001-quiet-modular/AGENT_CONTRACT.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/AGENT_CONTRACT.md` | `44ea2550641f5f939de8379dd39a6b75b98cc4e538ddd5ca4b874d34ae6cf122` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/AGENT_CONTRACT.md`. |
| **M-03** | `docs/design/experiments/EXP-001-quiet-modular/COMPONENTS.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/COMPONENTS.md` | `68ede8ae86056716746686576da64a83950f7d503d67e111d53543339c2f260b` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/COMPONENTS.md`. |
| **M-04** | `docs/design/experiments/EXP-001-quiet-modular/COMPOSITION.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/COMPOSITION.md` | `93399f452c69dcf208d6f99b5cc07a15a5df794075a5d55af71305ababc3ba11` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/COMPOSITION.md`. |
| **M-05** | `docs/design/experiments/EXP-001-quiet-modular/FOUNDATIONS.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/FOUNDATIONS.md` | `2d3f840bd8b60c6f04a8b1130fefbcc41333492a3bd8879452c3f6234bbc13bd` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/FOUNDATIONS.md`. |
| **M-06** | `docs/design/experiments/EXP-001-quiet-modular/INTERACTION.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/INTERACTION.md` | `dc2752b1861ac2d7767547f96a1a94da9e426fb0b987a526135ce54e11c06fe5` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/INTERACTION.md`. |
| **M-07** | `docs/design/experiments/EXP-001-quiet-modular/REFERENCES.md` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/REFERENCES.md` | `a7145426faf52461e3a94fcbf1bc140f9d9ba5defaec682ec80b74a7c25c4578` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/REFERENCES.md`. |
| **M-08** | `docs/design/experiments/EXP-001-quiet-modular/specimen.html` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/specimen.html` | `ef482deba4f60f193b496f7316a5d920722154d89ed618e1e5730a2629737e51` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/specimen.html`. |
| **M-09** | `docs/design/experiments/EXP-001-quiet-modular/tokens.json` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/tokens.json` | `ef71b9f2476961f26378fe537eaecea48b6329eb55f241c56e845c2884afb050` | Non-destructive Move | Relocate duplicate copy; authoritative version maintained in `design-system/tokens.json`. |
| **M-10** | `docs/design/experiments/EXP-001-quiet-modular/previews/` | `docs/design/experiments/EXP-001-quiet-modular/archive/root-spec-duplicates/previews/` | *(Directory of 10 PNGs)* | Non-destructive Move | Relocate duplicate directory; authoritative previews maintained in `design-system/previews/`. All 10 internal PNG hashes matched. |

---

## 2. Retained Assets (No Relocation / Untouched)

The following core files remain at their established canonical locations:

| Canonical Path | Purpose | Retained Status |
| :--- | :--- | :---: |
| `docs/design/experiments/README.md` | Experiments index | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/README.md` | Experiment Concept B README | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/QA.md` | Concept B QA Record | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/prototype.html` | Concept B 5-Screen Interactive Prototype | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/design-notes.md` | Concept B Rationale & Efficiency Matrix | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/report.md` | Concept B Deliverable Report | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/art-direction.md` | AD-01 Calm Editorial Utility Art Direction | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/home-direction.html` | AD-01 Standalone Home Study | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/home-xosdl-pilot.html` | XOS-DL v1.0 Home Pilot Prototype | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/DS-PILOT-QA.md` | Home Pilot QA Report | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/screenshots/*` | Verified 480×800 screenshots (14 PNG files) | **UNTOUCHED** |
| `docs/design/experiments/EXP-001-quiet-modular/design-system/*` | Authoritative XOS-DL v1.0 Design System folder | **UNTOUCHED** |
| `src/*`, `lib/*`, `freeink-sdk/*`, `test/*`, `platformio.ini` | Production firmware, HAL, tests, build config | **UNTOUCHED** |

---

## 3. Deletion Audit

- **Total Files Deleted:** `0`
- **Total Files Destroyed or Discarded:** `0`
- **Total Tracked Files Altered:** `0` (Excluding existing `USER_GUIDE.md` local modification).

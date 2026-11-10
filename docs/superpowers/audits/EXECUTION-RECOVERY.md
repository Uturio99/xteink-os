# Execution recovery audit — 2026-11-09

**Scope:** investigation and documentation/process correction only, requested by the PO. No firmware, SDK pin, production/build pipeline, visual prototype or approved UX was changed. This is an evidence register, not another plan; [EXECUTION_CONTRACT](../../EXECUTION_CONTRACT.md) and [STEP 09](../../design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md) own execution.

## Verified repository snapshot

- Assessed source: G:/AI-Setup/XTEInk-X4-Pro/xteink-os-refoundation, branch feat/xteink-os-refoundation, HEAD **7d63f480e779df702cc861437b331fccc960bb29**.
- origin = https://github.com/Uturio99/xteink-os.git; upstream = https://github.com/crosspoint-reader/crosspoint-reader.git. Both metadata fetches succeeded after the restricted origin attempt failed. origin/feat/xteink-os-refoundation matches 7d63f480; origin/develop is ad53eb64. Refreshed upstream/develop is **7a67cddb3a7819619d9f8adb49f75ba747e91c5b**; it is not the selected integration target. Upstream fetch warned it could not access that commit's submodule locally; no SDK update was attempted.
- Changes are isolated in the app-created worktree **C:/Users/Welcome/.codex/worktrees/noveos-execution-recovery/xteink-os-refoundation**, detached at 7d63f480. No commit or branch publication was created. Its uncommitted documentation diff is the deliverable; it is not yet present on origin.
- Parent SDK gitlink: **946e6ac3d7a9d590ba3224428e6183d55ac7917c**, unchanged. SDK directory in the assessed refoundation checkout is empty. Git invoked there returns the parent HEAD; that is not SDK initialization. Original xteink-os/freeink-sdk has its own verified top-level path, matching HEAD and clean status. Its pinned source was used read-only.
- Native config: env:x4pro selects esp32-s3-devkitc1-n16r8, dio_opi, FREEINK_DEVICE_X4PRO, FREEINK_CAP_USB_MSC and BOARD_HAS_PSRAM. Pinned BoardConfig.h:1855–1983 identifies touch/input/SD/RTC/light and explicitly unconfirmed panel mount / USB detection facts. Physical PO device remains unverified.
- Local engineering skill inventory in the assessed branch contains **10** SKILL.md files; the original checkout additionally has four untracked local skills. Do not assume “14 installed” applies to every worktree. Recovery used using-superpowers, ponytail, beads, the context/design portion of brainstorming, using-git-worktrees and verification-before-completion. Firmware-handoff and local rules were read as audit constraints, not executed as a firmware handoff. No review subagents or unavailable design skills were invoked.

| Existing worktree | HEAD / branch | Preserved initial state |
|---|---|---|
| xteink-os | 19f46d45 / feat/m2-wp2-home | Modified AGENTS; untracked STEP 03–05, refoundation plan/design/prototype and four local skills. |
| xteink-os-refoundation | 7d63f480 / feat/xteink-os-refoundation | Modified english.yaml, HomeActivity.cpp, CoverGridHomeUi.cpp, X4ProHomeLayout.h and X4ProHomeLayoutTest.cpp; untracked shape-consistency/, v2-final-review/ and test/simulator_home/. |
| xteink-os-font-gate | 7d63f480 / detached | Dirty Home/layout/translation/main and font/layout tests; untracked fonts, binaries/maps/logs/specimen overlay and typography bake-off. Experimental, not accepted production. |
| xteink-os-wp24-fixes | f6c9151e / feat/wp24-fixes | Preserved checkout; no new acceptance or reuse claim. |
| xteink-os-wp24-home-lean | 8a36e75e / feat/wp24-home-lean | Preserved checkout; no new acceptance or reuse claim. |

Beads: bd prime returned no context; bd where reported no active workspace in both refoundation and the original checkout. No task IDs, database, claims or completion records were invented. STEP 09's required status table is this audit snapshot; use active Beads for operational tracking when configured.

## Recovery and provenance

Formal STEP 03–05 **were unavailable in the assessed branch/history but present as untracked files in the original checkout**. Reachable Git history alone could not find them. The recovered record has F01–F44; there is no reason to invent another “44” list.

Source directory: G:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/superpowers/audits/. Original files remain untouched. Copies retain original content with a recovery banner; STEP 05's missing product-plan link is redirected to the execution contract. The old proposed refoundation plan was inspected in the original checkout but not copied as a competing active master plan.

| Recovered file | Original SHA-256 before banner/link correction |
|---|---|
| [STEP 03](STEP-03-FEATURE-INVENTORY.md) | 25d916beb6d8a7d41af661863241c855ac0a7ab9258eea13ec04497d4a36972d |
| [STEP 04](STEP-04-ROUTE-TRACE.md) | 545c18a660b80d85c8155bfa03c910e0e75928248f9380f5d617f9860c8e4cb9 |
| [STEP 05](STEP-05-NAVIGATION-CONTRACT.md) | 80beb4996dd3bb50c4ea92c6f00a895ef6087ec15b93ee653169ee3680b3757f |

**VERIFIED:** recoverability, original IDs, baseline ad53eb64 and recorded SDK pin. **APPROVED:** NAV-001 outcome and independent Home are recorded in STEP 04/05 and later STEP 06/V2; V2's named decisions are recorded in DESIGN-SYSTEM-V2-PROPOSAL.md (56804ed5 and subsequent source). **UNKNOWN:** blanket PO adoption of the recovered 44-record inventory as canonical, exact-current-head parity, native/hardware acceptance and authorization provenance for later implementation commits. Recovery does not convert author-written “complete” into PO approval.

## Discrepancy and root-cause register

The table establishes concrete mechanisms and repeated contradictory document claims, not incident frequencies or agent intent. Historical failure rates and token savings are UNKNOWN.

| Root cause / classification | Supporting evidence at original 7d63f480 or recovered source | How the workflow permits error | Smallest preventive control / location | Cost |
|---|---|---|---|---|
| Competing design entry points — VERIFIED | BASELINE called H1.1A “APPROVED PRODUCTION BASELINE”; PROJECT_MAP directed UI work there; V2 Authority records Hybrid/V2 approval. | Agents can follow different active-looking entry points. | Current precedence in execution contract; BASELINE index; historical banners on obsolete M1/M2/v1 guidance. | One short startup link; no repeated design research. |
| “Missing” evidence outside Git — VERIFIED | STEP 06/09 say STEP 03–05/44 ledger missing; original checkout has untracked originals. | Branch/history search excludes untracked worktree artifacts; handoff omits their location. | Recover original files once with hashes; REC-01 uses the same IDs. Future handoff includes untracked deliverables and exact checkout. | Three recovered files, no duplicate feature census. |
| Status detached from implementation — VERIFIED | PROJECT_STATUS still names M0 active/M2 queued; 372ccca1 adds Library return, 2dbf8155 adds simulator; STEP 09 later still describes both absent. | Status and plan repeat facts without revision/scope. | PROJECT_STATUS owns dated state; STEP 09 updates actual owner gaps rather than replaying completed implementation. | One state snapshot and delta update per affected package. |
| SDK identity inferred from directory/Git — VERIFIED | STEP 09 claims SDK present/matching; actual refoundation SDK is empty and rev-parse falls back to parent. | Command success is mistaken for the intended repository identity. | Verify SDK top-level + parent gitlink + actual HEAD before API claims; startup protocol. | One identity check when SDK matters. |
| Overstated verification reach — VERIFIED boundary, not measured historical error rate | library_reader_return_contract uses source strings and a Python list model; older browser PASS reports and build milestones have narrower scopes than native acceptance. | A single PASS/DONE label can hide unexecuted lifecycle/device cases. | Evidence layers + task-specific DoD; STEP 09 production-path cases and explicit simulator state table. | Attach scope/revision to existing results; tests only for real gaps. |
| Integration-boundary requirements omitted from partial NAV work — VERIFIED source discrepancies | STEP 05 names sync/loan relaunch, reversed Back mapping and touch-only exit. Current EpubReaderActivity/KOReaderSyncActivity replace the stack; ReaderUtils returns on all short Back and ignores touch Back gestures. | A narrow happy-path patch/test does not trace indirect callers or input modes. | REC-03 acceptance includes detours, mapping, modal/failure/Home/Files cases; read affected callers first. | Bounded scenario matrix, no new navigation framework. |
| Approval/readiness conflation and circular resource gate — VERIFIED document structure | STEP 09 says no firmware authorized while implementation exists; asks for parent+Reader memory before authorizing the proposed change. | Missing authorization provenance can be inferred from commits; no scoped route to measure candidate. | G1/G2 explicit acceptance/provenance; REC-02 separately authorized evidence on existing candidate before REC-03. Existing parity/safety gates remain intact. | Small gate record, no blanket approval. |
| Redundant or obsolete process — VERIFIED documents, unmeasured token cost | Legacy M0–M7 queue and M2 instructions coexist with later STEP plans; root starts on develop; original refoundation plan adds model/phase instructions. | Agents reload entire history or build another prototype instead of closing known native gaps. | One package, one writer, relevant sections only; preserve firmware four-review rule, lighter documentation checks; route tasks through current contract. | Short handoff; no extra agents in recovery. |
| Test prerequisites/gates not sufficient for behavior — VERIFIED configuration only | CI runs CMake/CTest and eight firmware targets, but no simulator/browser job; source NAV assertion is already registered. Parser suites require Expat while CI's explicit apt list only names cmake/ninja/zlib. | Build success cannot cover unsupported runtime cases; missing libraries may be incidentally supplied by runner image. No CI failure is asserted. | Reuse CTest; propose explicit missing dependency installation and smallest production-seam NAV tests, only in an authorized pipeline/code task. | Targeted CI proposal; no speculative framework. |
| Tracker assumed available — VERIFIED | bd prime empty; bd where fails in both relevant checkouts. | Instructions to select/claim an issue have no active store. | Report unavailability in contract; do not silently initialize competing stores. Package specs/handoff remain usable; activate shared Beads through its normal setup when maintained. | Explicit limitation, no invented task history. |

## Important remaining decisions and limits

- G1: accept/disposition the recovered ledger after current-head REC-01 reconciliation. The old pre-integration parity requirement is not silently relaxed to source-only proof.
- G2: record scoped implementation authorization/acceptance and correct the real short/long mismatch under the existing preservation contract; native binding must implement the already-approved contextual Back. Do not reopen V2 direction, tab names/direction, 12 px dialogs, English UI, Wi-Fi completion or USB restart-to-Home.
- G3: choose candidate, authorize bounded verification and any device/flash/diagnostic scope, establish real retained-parent memory/SD evidence. No source audit can supply these measurements.
- G4: reuse existing font feasibility/experiment evidence, then select native fonts and measured budgets. The font-gate bake-off README/visual-review.json explicitly says NOT TESTED; its report of earlier 13/13 coverage is historical, not this audit's result.
- Same-book relaunch and logical Back discrepancies are source-confirmed; exact failure manifestations, viewport restoration, missing-item notice and device safety remain runtime checks. No native defect was repaired here.
- A clean-head simulator build/launch record was not recovered in inspected Windows outputs. The font-gate README names separate WSL checkouts; those outputs were not inspected. Therefore build/launch history remains UNKNOWN, not “never ran.”

## Validation report

The final check results below are recorded only after execution; documentation-only verification does not certify firmware.

### Changed files

- New operating contract: docs/EXECUTION_CONTRACT.md.
- Existing entry points: AGENTS.md, .agents/rules/git-workflow.md, docs/contributing/development-workflow.md, docs/PROJECT_MAP.md, docs/PROJECT_STATUS.md, docs/design/BASELINE.md.
- Active behavior/delivery: docs/design/specs/STEP-06-UX-FLOWS.md and STEP-09-NATIVE-INTEGRATION-PLAN.md.
- Small historical/precedence notices: docs/design/DESIGN-SYSTEM.md, docs/design/specs/M2-ADDENDUM.md, M2-WP2-HOME-PLAN.md, UNIFIED_UI_COLLECTION.md, docs/design/step07a/STEP-07B-IMPLEMENTATION-PLAN.md, docs/design/step07b/STEP-07B.1R-DECISIONS.md.
- Recovered STEP-03-FEATURE-INVENTORY.md, STEP-04-ROUTE-TRACE.md, STEP-05-NAVIGATION-CONTRACT.md and this audit under docs/superpowers/audits/.

### Executed checks

| Check | Result and scope |
|---|---|
| Git worktree/status/log/remote/tree inspection; origin and upstream fetch | PASS for inventory/metadata. Origin restricted fetch initially failed; authorized retry succeeded. No integration/pin change. |
| git submodule status in restricted Windows shell | FAILED: Git MSYS permission error. Fallback git ls-tree plus verified SDK top-level/HEAD/status established the pin and empty-directory distinction; not a firmware failure. |
| bd prime; bd where | Tracking unavailable: prime empty; where failed with no active workspace in both original/refoundation. Not a passing tracker setup. |
| python test/test_library_reader_return.py in assessed refoundation source | PASS, exit 0. Source assertions and Python model only; native lifecycle NOT TESTED. No rerun after documentation edits. |
| Local Markdown link/path and heading-anchor scan of changed files | PASS after correction of a conditional uninitialized-SDK link. All local linked paths and Markdown heading anchors resolve; external URLs/historical textual Git pointers were not fetched by this check. |
| Recovered inventory IDs and provenance | PASS: exactly 44 unique F01–F44; all three original files unchanged; recovery banners and one repaired STEP 05 link are the only copy changes. |
| Package schema/dependencies and active authority review | PASS: REC-00 through REC-06 each has ID plus all 10 required fields; no package dependency cycle; REC-01 is documentation-only READY; native work blocked by named gates. Later V2 overrides old visual wording without reopening it. |
| git diff --check; Markdown whitespace scan including new files | PASS. Git reports expected LF/CRLF conversion warnings only; no whitespace errors. |
| git status --short --untracked-files=all and final diff review | PASS: only the 19 named Markdown paths; firmware, tests, pipelines and gitlink untouched. New files reviewed as well as tracked diff. |
| Original WIP protection | PASS: five initially dirty refoundation source/test files retain their SHA-256 fingerprints; the three source audit originals retain recorded hashes. Other worktrees were not edited. |

### Not tested / not authorized

No firmware builds, full host suite, rendered browser QA, simulator compilation/launch, physical X4 Pro checks or CI runs were executed. No production/build-pipeline edits, dependency/SDK changes, commit, push, merge, rebase, reset, delete, flash or release. Mandatory firmware handoff was not run because this change is documentation-only. Native integration remains BLOCKED; the next READY task is the bounded documentation-only REC-01 in STEP 09.

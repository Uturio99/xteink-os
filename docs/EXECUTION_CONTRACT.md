# NoveOS execution contract

**Current operating authority — 2026-11-09.** This recovery changes documentation and process only. It authorizes no firmware, SDK, build-pipeline, flash, commit, merge, push or publication action. Existing explicit PO authorizations remain valid only for their named scope; a commit or a continuation request is not acceptance.

## Product and authority

Deliver NoveOS on XTEInk X4 Pro over the existing CrossPoint/FreeInk architecture, preserving CrossPoint capabilities, recovery and data integrity. Use the approved NoveOS V2 / Hybrid Editorial Index direction, 480 × 800 logical portrait baseline, English system UI and intact Vietnamese NFC/NFD content. Preserve reading performance, E-Ink readability, physical input and supported-board compatibility. Browser dimensions and fonts are references; use native oriented/viewable geometry and measured font metrics.

Resolve conflicts in this order, within higher-priority session instructions:

1. Hardware, storage, boot/recovery and firmware safety constraints in [AGENTS](../AGENTS.md), the relevant [rules](../.agents/rules/) and pinned SDK. No document silently waives them.
2. Explicit PO decisions with their provenance and scope. Visual approval, implementation authorization and behavior/maintenance acceptance are separate.
3. Current product/UX: [V2 Authority and Templates](design/DESIGN-SYSTEM-V2-PROPOSAL.md), its immutable [Hybrid demo](design/step07a/xteink-hybrid-demo.html), then [STEP 07B decisions](design/step07b/STEP-07B.1R-DECISIONS.md) and reconciled [STEP 06](design/specs/STEP-06-UX-FLOWS.md). V2's later approved decisions resolve older Library/import/shape wording. PROPOSED token/native-font choices remain proposals.
4. Architecture and integration: current source, relevant rules and [STEP 09](design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md). Source proves implementation, not permission or correct behavior. Recovered [STEP 05](superpowers/audits/STEP-05-NAVIGATION-CONTRACT.md) supplies baseline ownership/route evidence, with the corrections in STEP 09.
5. The assigned work-package contract and its recorded authorization, dependencies and acceptance cases. It cannot change levels 1–4.
6. Historical H1.1A, M1/M2 plans, earlier experiments and screenshots: provenance/reference only. [BASELINE](design/BASELINE.md) is the design index. They do not choose today's product design or integration branch.

If two sources at the same level materially conflict, record the exact conflict and stop only the dependent work. Never infer approval from file dates, commit subjects, screenshots or model confidence.

## State and readiness

[PROJECT_STATUS](PROJECT_STATUS.md) owns the dated evidence snapshot; [recovery audit](superpowers/audits/EXECUTION-RECOVERY.md) owns Git facts, discrepancies, checks and provenance. Do not reproduce those inventories in handoffs.

The F01–F44 source inventory and STEP 04/05 have been recovered from another worktree, not newly invented. They describe baseline ad53eb64; their current-head reconciliation and PO adoption are still required. NAV-001 code and the pinned simulator configuration already exist at 7d63f480, but neither is accepted NoveOS parity. Full native implementation is BLOCKED by the STEP 09 gates. The first READY package is **REC-01**, a bounded documentation-only delta reconciliation. Do not reimplement the existing Library push/result callback or restart browser design.

Evidence labels: **VERIFIED** = named current source/check with its scope; **APPROVED** = identifiable PO decision, not a test; **PROPOSED** = awaiting approval; **UNKNOWN** = insufficient evidence; **BLOCKED** = named unmet dependency with a closure condition. Check outcomes are PASS / FAIL / NOT TESTED, always with source/host/build/simulator/device layer and revision. Package states are DONE / READY / BLOCKED / IN PROGRESS. DONE requires its acceptance evidence; READY requires actual prerequisites and permission for that scope.

## One bounded task

1. Open the assigned package in [STEP 09](design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md). Check its status and exact authorization. With active Beads, use `bd prime`, `bd ready`, `bd show <id>`, then `bd update <id> --claim`. Match the stable package ID in the issue; never invent a Beads ID. The audit found no active Beads workspace. Report that limitation; do not create a second tracker or claim a task was registered. The roadmap statuses are this audit's requested snapshot, not a parallel live task database.
2. Record checkout, HEAD, remotes, dirty paths and SDK gitlink/actual checkout. Verify the SDK's own top-level directory: Git in an empty submodule directory can return the parent HEAD. Fetch when branch currency matters. Preserve unrelated work. Select the authorized base from evidence; never default to develop or the newest branch. Repository setup is not repeated unless missing/broken.
3. Read this contract, the package's relevant authority sections and affected source/callers. Reuse the existing owner's API, tests and evidence. Load only skills whose procedures the task needs. No repository-wide/history replay without a concrete missing fact.
4. Confirm dependencies and observable acceptance before editing. For firmware, resolve the named PO gates and follow [firmware-handoff](../.agents/skills/firmware-handoff/SKILL.md). Ordinary documentation corrections need no new approval. Implement the smallest complete change; no unrelated features, wrappers, state stores, dependencies or cleanup.
5. Verify the failure modes below; inspect the complete final diff including new files. Keep green checks valid until a change/failure invalidates them. On an ordinary defect, reproduce and correct it inside the package. After two failed fixes based on the same hypothesis, stop patching, collect discriminating evidence and reassess the root cause; escalate only if scope/architecture must change.
6. Handoff: package/Beads ID if available, exact base and candidate diff, scope, authority links, acceptance results, commands/logs, evidence layer, blockers, remaining risks and the next status. Close a Beads issue only on actual completion. Stop at scope; no automatic commit, push, PR, merge or flash.

## Risk-proportional definition of done

Every check names the failure it detects and leaves its command, revision and observed result. An unavailable mandatory check leaves the package incomplete.

| Task type | Failure to detect | Required evidence / completion boundary |
|---|---|---|
| Documentation/process | Conflicting authority, dead links, false state/approval, scope leakage | Self-review against source/decisions; local link/path checks; package-field and dependency consistency; diff/whitespace check; preserved dirty work. No firmware build or four-review ceremony. |
| Browser prototype | Wrong composition/geometry/routes/focus/states/text | Affected 480×800 views and hit regions; interaction walkthroughs, empty/error/NFC/NFD samples and focused existing browser checks. Static assertions do not prove rendered behavior. PO visual acceptance applies to the changed design only. |
| Native UI/font | Geometry/clipping/fallback/input regressions, shared-board breakage | Relevant layout/Unicode tests, actual native rendering where simulator works, relevant target builds, actual hit rectangles and physical readability/refresh/input evidence. Font footprint/fallback must be measured before adoption. Firmware handoff applies. |
| Navigation/lifecycle | Lost parent/identity/viewport, wrong input owner, stale handles, leaks | Production-path open/child/return/failure/Home cases; both Back mappings; same-book relaunch; missing/reordered item; CLIX failure and construction failure. Existing source-string assertions are supplemental. Measure device internal/PSRAM peaks/largest blocks and repeated return/exit. Firmware handoff applies. |
| Storage/USB/Wi-Fi/firmware | Data loss, conflicting SD owners, false success, broken recovery | Affected host/error-path tests + target build; human device evidence for write/close/rescan, offline/abort, exclusive MSC/eject/remount/restart and protected boot/recovery. Preserve source files on failure and existing recovery paths. No destructive test or flash without explicit scope. Firmware handoff applies. |
| Hardware-dependent | Ghosting, wrong touch transform, failed allocation/boot/physical controls | Human observations tied to exact board/panel/build/card, relevant orientations and remappings, repeat cycles, internal heap/PSRAM/largest blocks, and pass/fail cases. Simulator/compile success cannot close this row. |

The main agent owns verification. For firmware/build-affecting changes retain **all four independent read-only review axes**, at most two rounds, final relevant builds and explicit human behavior/architecture/maintenance acceptance from firmware-handoff. Do not apply that gate to this documentation-only task. Hardware checks remain human-owned and are required before any human-opened PR.

## Escalation and lean roles

Stop dependent implementation when an approved decision must change, an essential prerequisite is unknown, safety is uncertain, a check disproves the foundational contract, or a material ownership/architecture change is needed. State the smallest decision or evidence that unblocks it. Do not reopen architecture for routine compilation or test failures.

Use one builder with write ownership. An orchestrator settles scope/dependencies and technical escalations; it cannot grant PO approval. Prefer a low-cost capable builder (for example DeepSeek Flash **if actually available**) for a complete, routine contract; do not make model availability a dependency or claim a model was used when it was not. Use expensive reasoning for unresolved architecture. A reviewer checks the pinned diff and evidence, returning actionable defects; no duplicate exploration or tests. Independent agents are used only for genuinely independent work or required firmware reviews. This recovery needs self-review only.

Defer new prototypes, new frameworks, performance caches and CI expansion unless a named acceptance failure requires them. CI proposals and the next-agent instruction are in STEP 09; do not create another master plan.

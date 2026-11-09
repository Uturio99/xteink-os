# NoveOS project map

Start with [AGENTS](../AGENTS.md) and the [execution contract](EXECUTION_CONTRACT.md), then the assigned [STEP 09 package](design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md). Read only its linked authority and affected source. This map is navigation, not a second plan.

| Need | Owning reference |
|---|---|
| Current state, acceptance versus implementation | [PROJECT_STATUS](PROJECT_STATUS.md) |
| Git snapshot, recovered artifacts, discrepancies and checks | [Recovery audit](superpowers/audits/EXECUTION-RECOVERY.md) |
| Current visual authority / historical baselines | [BASELINE](design/BASELINE.md), [V2](design/DESIGN-SYSTEM-V2-PROPOSAL.md), [Hybrid demo](design/step07a/xteink-hybrid-demo.html) |
| User flows, Back/Home, import completion | [STEP 06](design/specs/STEP-06-UX-FLOWS.md), [STEP 07B decisions](design/step07b/STEP-07B.1R-DECISIONS.md) |
| Recovered 44 source records and route corrections | [STEP 03](superpowers/audits/STEP-03-FEATURE-INVENTORY.md), [STEP 04](superpowers/audits/STEP-04-ROUTE-TRACE.md), [STEP 05](superpowers/audits/STEP-05-NAVIGATION-CONTRACT.md) |
| Native owners, blockers, packages and next-agent handoff | [STEP 09](design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md) |
| Native activities and shared UI | [src/activities](../src/activities/), [src/components](../src/components/), [UI/input rules](../.agents/rules/ui-activities.md) |
| Hardware/storage and SDK | [lib/hal](../lib/hal/), [freeink-sdk](../freeink-sdk/), [HAL rules](../.agents/rules/architecture-hal.md), [resource rules](../.agents/rules/hardware-resources.md) |
| Book/catalog/font/data owners | [lib](../lib/), [src](../src/); identify the existing symbol before adding anything |
| Host tests and CI | [test](../test/), [CI](../.github/workflows/ci.yml), [testing rules](../.agents/rules/testing-debugging.md) |
| Target/simulator setup | [platformio.ini](../platformio.ini), [getting started](contributing/getting-started.md) |
| Firmware handoff | [Mandatory skill](../.agents/skills/firmware-handoff/SKILL.md), unchanged four-review and human-acceptance requirements |
| Historical planning and experiments | [Prior plans](superpowers/plans/), [M1 specification](design/specs/UNIFIED_UI_COLLECTION.md), [experiments](design/experiments/); not current execution authority |

Target: X4 Pro, 480 × 800 logical portrait. Actual board capabilities come from the pinned SDK and selected environment, not guessed from the product name. Preserve generic CrossPoint compatibility and C3 resource constraints where shared code is affected.

Current assessed source is branch feat/xteink-os-refoundation at 7d63f480 (this recovery worktree is detached at the same commit); this is an audit baseline, **not an integration destination approval**. Never automatically target develop. Preserve all dirty/experimental worktrees; no merge, commit, push or flash follows from a documentation update.

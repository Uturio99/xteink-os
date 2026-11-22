## Orchestration and delegation

PO decision 2026-11-22. Applies to every NoveOS work package. It does not weaken [AGENTS](../../AGENTS.md), the [execution contract](../../docs/EXECUTION_CONTRACT.md) or [firmware-handoff](../skills/firmware-handoff/SKILL.md).

### Roles

- **Orchestrator (frontier model).** Thinks, brainstorms, settles scope, gates and design, and writes detailed briefs. Talks to the PO. Does not code, build or search, except small checks that unblock a decision. Gives a short approve/return verdict on every diff before it lands, based on the reported logs and artifacts. Does not re-run the checks.
- **Builder (fast model, e.g. Devin Fusion sidekick).** Does the hands-on work: reading, searching, coding, building, testing and reporting. Only one builder has write ownership at a time.
- **Reviewer (read-only subagent with the repository `review-*` skills).** Reviews the pinned diff and evidence. May not write files, build, test, commit, publish or flash.

### Review escalation

1. A reviewer subagent reviews.
2. If it is stuck, the orchestrator writes focused review guidance and the reviewer tries again.
3. The orchestrator reviews only as a last resort: step 2 failed and a serious defect remains.

### Brief template

Every builder brief contains:

1. Package ID, goal (1–2 lines), PO decision or authority link.
2. Exact base (branch/SHA), files in scope, files explicitly out of scope.
3. Step-by-step instructions with paths, symbols and line references, plus the hard edge cases.
4. Constraints: IRAM is full, no heap allocation in render loops, no Bayer dither in Atelier chrome, `FREEINK_DEVICE_X4PRO` plus runtime guards, i18n only through YAML, no commit/push/flash/SDK change, preserve dirty files.
5. Verification commands (narrowest set) and expected output.
6. Report format: diff summary, commands with exit codes and log paths, evidence label (SOURCE/BUILD/SIMULATOR/HARDWARE_VERIFIED or NOT_TESTED), blockers. Return artifacts; do not delete them.
7. Stop conditions: scope conflict, two failed fixes on the same hypothesis, or any safety doubt. Stop and report.

### Tracking

Slack channel `#nove-os-development` and its package list mirror package status for visibility. Package contracts stay in [STEP 09](../../docs/design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md). Do not claim a specific model was used unless it was.

### Unchanged gates

Firmware-handoff with four review axes (at most two rounds), explicit human acceptance, no commit without approval, no push, PR or flash without an explicit instruction. Hardware testing remains the human's responsibility.

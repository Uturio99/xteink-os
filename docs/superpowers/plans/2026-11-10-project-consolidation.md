# XTEInk X4 Pro Consolidation — Deepseek Flash V4.1 Implementation Plan

> **For agentic workers:** use `superpowers:executing-plans` and Ponytail full. The main executor owns changes, tests and builds. Subagents are read-only reviewers under the repository rules. Track the checkboxes below; do not restart completed work after context loss.

**Goal:** consolidate development into one authoritative checkout at `D:\Work\AI\XTEInk X4 Pro\xteink-os`, with `develop` as its only active local branch, while preserving useful work and recoverable history.

**Architecture:** retain the current firmware foundation, selectively adopt compatible changes from other branches, archive superseded alternatives, and retire redundant worktrees. Preserve CrossPoint/FreeInk ownership boundaries and existing X4 Pro functionality.

**Tech Stack:** C++/PlatformIO, pinned FreeInk SDK, existing CMake/CTest suites, and the pinned Linux/WSL2 X4 Pro simulator.

**Spec:** the approved decisions in this plan. Persist them in `D:\Work\AI\XTEInk X4 Pro\xteink-os\docs\superpowers\specs\2026-11-10-project-consolidation-design.md`.

**Plan file:** update the existing [consolidation plan](2026-11-10-project-consolidation.md). Do not create another competing master plan.

**Execution status (2026-11-10):** CONS-01–04 are implemented, verified and committed; CONS-05 (archive refs, detach external snapshot, retire five worktrees, promote `develop`) is **not started** and awaits explicit owner confirmation. See the [CONS-04 handoff](../audits/CONS-04-HANDOFF.md); the campaign ledger is `.local-archive/consolidation-2026-11-10-campaign/progress.md` (Git-ignored).

---

## 1. Execution contract, known state and bootstrap

### Global constraints

These are settled decisions. Do not reopen them during routine execution.

- The CONS plan dated **2026-11-10** controls the consolidation campaign.
- **NoveOS V2 / Hybrid Editorial Index** controls product presentation.
- The **STEP 09 REC-02 revision at `04b6b754`** supplies the latest inspected native integration guidance. Reconcile it with the current source.
- Target **X4 Pro first**. Preserve the HAL/SDK boundaries needed for future devices.
- Preserve currently available reading, Library, import, dictionary, OPDS, plugin, sync, settings, storage and recovery functionality.
- This campaign produces a stable consolidated foundation. It does **not** finish every outstanding native NoveOS V2 screen.
- Keep the SDK and simulator pins unless a separately authorized task changes them.
- Preserve personal configuration and uncommitted work.
- Do not create/amend commits, push, change GitHub branches, open/close PRs or flash.
- Do not merge an entire WIP branch merely because it is newer.
- Keep the external Codex worktree as a detached historical snapshot. Do not delete its folder or chat data.
- Use one writer. If files change unexpectedly while you are reading, pause mutations and resolve competing writers.
- Store repository-facing documentation in English. Communicate with the owner in Vietnamese.

### Known starting state

Recheck these values once at execution start.

| Item | Recorded value |
|---|---|
| Main checkout | `D:\Work\AI\XTEInk X4 Pro\xteink-os` |
| Current branch | `codex/wp1-library-reader-return` |
| Current HEAD | `01878fe194fb511b68c3f15d710f371dd57c63a9` |
| SDK pin | `946e6ac3d7a9d590ba3224428e6183d55ac7917c` |
| Simulator pin | `6730fe5fe576c9d924e44096a9e267d8538d184c` |
| Registered worktrees | Seven: main, five sibling worktrees, one external Codex worktree |
| Existing recovery set | `.local-archive/consolidation-2026-11-10/` |
| Recovery verification | 28 recorded refs covered; 74 manifest entries matched |
| Firmware baseline | Existing X4 Pro log reports SUCCESS |
| Host/simulator baseline | Not yet established by executed suites in the recorded environment |

The existing [CONS-01 inventory](../audits/CONS-01-INVENTORY.md) contains recovered state and restoration instructions. Read it before repeating inventory work.

### Shell and logging rules

Use PowerShell for Windows commands. Do not send PowerShell syntax to `cmd.exe`.

```powershell
$repoPath = 'D:\Work\AI\XTEInk X4 Pro\xteink-os'
$runPath = Join-Path $repoPath '.local-archive\consolidation-2026-11-10-campaign'
Set-Location -LiteralPath $repoPath
$env:PYTHONIOENCODING = 'utf-8'
```

Use Linux commands only inside WSL. Do not enumerate files in one shell and pass generated deletion commands into another.

For native commands, capture the exit code immediately:

```powershell
& pio run -e x4pro *> (Join-Path $runPath 'logs\x4pro-final.log')
$buildExit = $LASTEXITCODE
```

A zero exit code with an empty or unrelated log is insufficient evidence.

### Bootstrap tasks

- [x] Read `AGENTS.md`, the existing consolidation plan and CONS-01 inventory.
- [x] Run the required branch, remote, status and SDK checks.
- [x] Create the campaign directory and verify that Git ignores it.
- [x] Copy the existing plan and inventory into the campaign’s preflight snapshot before editing them.
- [x] Save this complete plan into the existing plan file and save the standalone design specification.
- [x] Create the execution ledger at `$runPath\progress.md`.

The ledger begins with the absolute plan path and records, for each CONS package:

- Status: `NOT STARTED`, `IN PROGRESS`, `DONE` or `BLOCKED`.
- Source HEAD and candidate fingerprint.
- Changes made.
- Commands, observed outcomes and log paths.
- Any ruling, its reason and its consequence.
- The exact next action.

**Context recovery:** read the ledger and current diff, then resume the next unfinished action. CONS-01–05 are defined here; do not search GitHub or session histories for their meaning.

### Review focus

Every package must account for these five risks:

1. Losing a unique commit, untracked file or ignored personal asset.
2. Removing code reached through configuration, registration or generated sources.
3. Leaving build/runtime paths dependent on a retired worktree.
4. Presenting historical evidence as verification of the current candidate.
5. Regressing reading state, progress, navigation or SD/USB ownership.

---

## 2. CONS-01 — Verify recovery and establish a usable baseline

**Purpose:** complete the preservation boundary before source adoption or retirement.

### A. Verify the existing recovery work

- [ ] Compare live refs against both existing bundles. Preserve their current verified state; do not overwrite the original recovery set.
- [ ] Check for changes since the recorded inventory, including the newly saved plan and specification.
- [ ] Create a fresh campaign snapshot containing all current relevant refs and detached tips.
- [ ] Fetch the recovery bundles into a temporary bare repository and compare restored refs with the source snapshot.
- [ ] Confirm that required commit objects exist, including `c9d12b52`, both detached worktree tips and the five remote checkpoint tips inspected during planning.

`git bundle verify` proves bundle validity; it does not by itself prove that every required ref was included.

If using a new all-ref bundle:

```powershell
git bundle create (Join-Path $runPath 'history.bundle') --all
git bundle verify (Join-Path $runPath 'history.bundle')
```

Preserve otherwise unreachable detached tips through archival refs before creating the bundle.

### B. Verify files and submodules

- [ ] Check staged, unstaged, untracked and valuable ignored content in every in-scope worktree.
- [ ] Check each SDK checkout separately. A clean main SDK does not prove that another worktree’s SDK has no unique content.
- [ ] For the broken `wp1b` SDK gitfile, preserve files and metadata before attempting repair or reinitialization.
- [ ] Read the simulator worktree through WSL because its gitfile uses `/mnt/d/...`.
- [ ] Distinguish CRLF-only changes from real edits. Do not discard the approximately 1,040 reported changes solely on an earlier report.
- [ ] Preserve the real simulator configuration delta, additional simulator INI, `rec01` patch, font experiment, personal overrides and WP1A evidence.
- [ ] Generate a manifest covering every retained archive file except the manifest itself. Do not exclude every file named `SHA256SUMS.txt`.
- [ ] Restore copies to a temporary directory and compare hashes with both the archive and original source files.

### C. Establish baseline commands

Read the repository environment guidance before installing or replacing tools.

- [ ] Locate existing CMake, compiler, Python and PlatformIO installations, including documented virtual environments.
- [ ] Repair only an identified missing dependency.
- [ ] Treat **CMake as required** for host suites. Ninja is required only if the selected generator needs it.
- [ ] Keep Windows and WSL CMake/PlatformIO build caches separate.
- [ ] Record host and simulator baseline results before adopting firmware changes.

For WSL host tests, use a separate ignored build directory:

```sh
cmake -S test -B build/test-consolidation-wsl -DCMAKE_BUILD_TYPE=Release
cmake --build build/test-consolidation-wsl
ctest --test-dir build/test-consolidation-wsl --output-on-failure
```

The root CMake configuration pins GoogleTest. Reuse it; do not upgrade dependencies to make setup convenient.

**Exit criteria**

- All required history and personal work can be restored.
- Any live-state differences since the previous inventory are covered.
- Baseline checks have observed outcomes, or precise environment blockers.
- No worktree or branch has been removed.

A missing verification tool blocks dependent verification and retirement. It does not prevent independent documentation work.

---

## 3. CONS-02 and CONS-03 — Unify authority and disposition every branch delta

### CONS-02 — One authoritative documentation chain

**Inputs:** current source, approved decisions above, recovered STEP 03–05, latest inspected STEP 09 REC-02.

**Outputs:** one operating contract, one current status, one capability ledger and one consolidation plan.

- [ ] Recover the execution contract and audit documents from `17773ef2`.
- [ ] Incorporate the relevant STEP 09 updates from `04b6b754`.
- [ ] Reconcile them with `01878fe1` and this campaign’s actual authorization.
- [ ] Update the project map, project status, baseline index and `AGENTS.md` entry point to identify the canonical checkout and plan.
- [ ] Preserve safety, maintenance, review and publication requirements.
- [ ] Mark superseded M0/M1/WP2.4 instructions as historical. Preserve their provenance.
- [ ] Keep historical approval separate from current implementation and current test evidence.
- [ ] Preserve F01–F44 identifiers while reconciling their routes and conditions with current source. Add newly found capabilities separately.

Do not restart completed Library-return implementation or claim that no simulator environment exists. Both statements are stale for the current source.

**Check**

Inspect local links and active instructions. An executor must be able to identify the next task from repository files without reading this chat.

**Exit:** no two active documents prescribe incompatible direction or different authoritative workspaces.

### CONS-03 — Produce the disposition manifest

Create:

`D:\Work\AI\XTEInk X4 Pro\xteink-os\docs\superpowers\audits\CONS-03-DISPOSITION.md`

Each record contains:

| Field | Required content |
|---|---|
| Source | Ref and pinned SHA, or worktree and file hashes |
| Delta | The unique change being evaluated |
| Requirement | The approved capability or maintenance need |
| Owner/use | Existing owner, callers, registrations and build dependencies |
| Decision | `KEEP`, `ADOPT`, `REPAIR`, `ARCHIVE` or `GENERATED` |
| Verification | Relevant check and observed result |
| Recovery | Bundle ref, patch or preserved file location |

Evaluate these sources explicitly:

| Source | Required treatment |
|---|---|
| Current `01878fe1` | Default source foundation, including current Home V2 work |
| `17773ef2` and `04b6b754` | Recover documentation; reconcile the competing simulator patch implementations |
| `080c5619` | Recover useful planning/audit evidence; do not restore an older product authority |
| `f6c9151e`, `1ab06079`, `2586f4de` | Assess Home/progress/parser/settings/test deltas independently |
| `773284fb` | Preserve typography experiment and evidence; distinguish duplicated Home V2 changes from experimental font registration |
| `wp1b` working tree | Preserve font registration experiment and six untracked headers |
| `c9d12b52` and historical checkpoints | Preserve historical assets and the local-only commit; do not silently lose them during branch convergence |
| STEP 06 and simulator worktrees | Recover any remaining useful documents, fixtures, scripts and configuration |
| Two WP1A folders | Preserve binaries, checksums, screenshots, fixtures, logs and revision provenance |

**Decision rules**

- `KEEP`: required current implementation already present.
- `ADOPT`: missing change fits the approved contract and passes its relevant checks.
- `REPAIR`: required implementation has a demonstrated consolidation-related failure.
- `ARCHIVE`: superseded, experimental, conflicting or insufficiently proven for adoption.
- `GENERATED`: reproducible output whose source and regeneration path remain available.

Use the installed codebase-memory workflow for structural discovery. Check coverage for relied-on paths and read reported gaps directly. Search configuration, scripts, code generation and runtime registration before concluding that a module is unused.

Do not classify a feature as removable merely because its code is old or difficult to test.

**Exit:** every unique contribution has a disposition. An archived change remains recoverable and has a clear condition for reconsideration.

---

## 4. CONS-04 — Integrate selected changes and verify the main checkout

**Purpose:** make the canonical checkout work independently of every sibling worktree.

### A. Integrate in bounded batches

Use this order:

1. Documentation and authority reconciliation.
2. Host tools and simulator compatibility.
3. One necessary firmware/parser/progress change at a time.
4. Removal of superseded alternatives after their replacement is verified.

For each batch:

- [ ] Record its input SHA, source files and expected behavior.
- [ ] Inspect the relevant owner and affected callers.
- [ ] Reproduce any claimed defect before fixing it.
- [ ] For new non-trivial logic, write the smallest meaningful failing regression check, observe the failure, implement the fix and observe the pass.
- [ ] Review the complete diff, including new files.
- [ ] Record results before moving on.

Do not add meaningless tests for prose changes or tests that merely duplicate implementation text.

### B. Simulator and tooling convergence

- [ ] Compare the committed REC-02 simulator fix with the `rec01` local patch.
- [ ] Keep one implementation that handles the demonstrated Windows/WSL patch failure while retaining the simulator revision check and already-applied detection.
- [ ] Check fresh application, already-applied behavior, wrong revision rejection and genuine patch mismatch.
- [ ] Run the existing `simulator_x4_pro` profile from the main checkout in WSL.
- [ ] Use a disposable simulator filesystem. Do not point it at the owner’s physical SD card.
- [ ] Preserve existing font-generation inputs and tooling. Keep experiments outside the active firmware font registry unless separately selected and verified.
- [ ] Replace executable paths that reference retired sibling checkouts with paths derived from the main project root.

### C. Host and firmware verification

Relevant existing test targets include:

- `X4ProHomeLayoutTest`
- `LibraryIndexFileTest`
- `ButtonPressNavigationTest`
- `HomeButtonInputTest`
- `LibraryTextTest`
- `Utf8ComposeTest`

Use `ctest -N` to inspect registered tests before selecting filters. A filter matching zero tests is not a pass.

Run the relevant suites for each batch. Run the integrated host suite once after the final substantive integration, then repeat only checks invalidated by later changes.

Use the repository formatter wrapper:

```sh
./bin/clang-format-fix -g
```

Run it in an appropriate shell. Inspect new C/C++ files separately because the wrapper scopes itself through Git-tracked paths.

Build X4 Pro with captured output:

```powershell
& pio run -e x4pro *> (Join-Path $runPath 'logs\x4pro-candidate.log')
$candidateBuildExit = $LASTEXITCODE
```

A build failure must be diagnosed from the log. Do not repeatedly rerun an unchanged failing command without collecting new evidence.

### D. Runtime scenarios

Check the affected scenarios in the simulator:

- Home with and without a resumable book.
- Library browsing/search and opening a book.
- Reader contextual Back and explicit Home.
- Preserved Library context where the current implementation supports it.
- Settings entry and return.
- Missing-book and relevant recovery paths.
- Long Vietnamese titles and NFC/NFD samples.

Source assertions, simulator interaction and hardware behavior are separate evidence layers.

**Exit:** accepted changes work from the main checkout; active tools no longer need sibling worktrees. Existing V2 gaps are documented, and new consolidation regressions are resolved.

---

## 5. CONS-05 — Archive branches, retire worktrees and hand off

### A. Prepare the final candidate

- [ ] Freeze the candidate during review.
- [ ] Identify it by HEAD plus the tracked diff and hashes of relevant untracked build inputs. HEAD alone is insufficient while changes remain uncommitted.
- [ ] Complete self-review and relevant checks.
- [ ] For firmware/build-affecting changes, follow `firmware-handoff` with four independent read-only review axes: correctness, architecture, embedded, and i18n/docs.
- [ ] Reviewers inspect source and recorded results only. They do not write files, run tests/builds, commit or publish.
- [ ] Resolve findings within the handoff’s bounded review process.
- [ ] If the harness cannot obtain the required reviews, report the missing review explicitly. Do not fabricate approval.

Documentation-only and host-only changes use the lighter applicable review unless they alter firmware output/build.

### B. Preserve refs before reducing visible branches

Archive every relevant branch/ref under:

```text
refs/archive/consolidation-2026-11-10/
```

Preserve original name → archived name → SHA mappings.

- [ ] Create archival refs using the recorded expected SHA.
- [ ] Verify every archived ref resolves correctly.
- [ ] Refresh the campaign recovery bundle so it also preserves the archival namespace.
- [ ] Test restoration before deleting visible branch refs.
- [ ] Never modify Codex-owned checkpoint refs or delete Git objects.

Keep remote GitHub branches unchanged.

### C. Detach the external Codex snapshot

External path:

```text
C:\Users\Uturio\.codex\worktrees\xteink-step07a-visual\xteink-os
```

- [ ] Recheck its tracked, untracked and valuable ignored state.
- [ ] Preserve newly discovered work before changing branch attachment.
- [ ] Detach at its actual recorded HEAD, expected to be `ea8dbd94`.
- [ ] Confirm that the HEAD and file content remain unchanged.
- [ ] Keep the folder and application/chat metadata.

If unexpected changes exist, preserve them first. Do not discard them to make detachment convenient.

### D. Retire in-scope sibling worktrees

The retirement allowlist is exactly:

```text
D:\Work\AI\XTEInk X4 Pro\xteink-os-rec01
D:\Work\AI\XTEInk X4 Pro\xteink-os-simulator-wp1a-372ccca1
D:\Work\AI\XTEInk X4 Pro\xteink-os-step06
D:\Work\AI\XTEInk X4 Pro\xteink-os-wp1b
D:\Work\AI\XTEInk X4 Pro\xteink-os-wp24
```

Before each retirement:

- [ ] Resolve the absolute target and any junction/symlink destination.
- [ ] Confirm it matches the allowlist and is not the primary checkout.
- [ ] Verify its unique content against the disposition manifest and recovery set.
- [ ] Handle submodules and WSL metadata in the appropriate environment.
- [ ] Remove through Git worktree mechanisms after handling its dirty state.
- [ ] Do not use forced removal to bypass unpreserved dirty content.
- [ ] Record the outcome and check remaining worktree registrations.

Do not use blanket `git clean -fdx`, broad recursive deletion, or filesystem deletion to bypass Git errors.

Absorb the two WP1A folders into the canonical evidence/archive structure after verifying the existing archived copies. Preserve root harness configuration such as `.commandcode`; it is not an additional firmware project.

### E. Promote one active local branch

Perform these steps only after the old `develop` and every other branch have verified archival refs.

- [ ] Preserve the existing `develop@c9d12b52` under the archival namespace.
- [ ] Remove the old visible local `develop` ref using an expected-SHA guard.
- [ ] Rename the current canonical branch to `develop`. Do not reset the working tree to old `develop` content.
- [ ] Remove other archived local branch refs only after no worktree is attached to them.
- [ ] Preserve the full uncommitted candidate during the rename.
- [ ] Configure local `develop` to track `origin/develop`.
- [ ] Restrict `origin` fetch to `develop` so retired remote-tracking branch names are not recreated during ordinary fetches.
- [ ] Preserve then remove obsolete remote-tracking refs. Keep `origin/develop`, symbolic `origin/HEAD`, and `upstream/develop` for comparison.
- [ ] Leave remote URLs and GitHub branch state unchanged.

Do not use a force rename that silently overwrites an unverified branch.

Expected final checks:

```powershell
git branch --show-current
git for-each-ref --format='%(refname)' refs/heads
git worktree list --porcelain
git status --short
```

Expected outcomes:

- Current branch: `develop`.
- Local heads: only `refs/heads/develop`.
- Worktrees: primary checkout plus the external detached snapshot.
- Primary source and personal files preserved.
- Remaining modifications and untracked deliverables accounted for.

### F. Final verification and completion boundary

- [ ] Check active scripts and documentation for retired executable paths. Historical manifests may retain those paths as provenance.
- [ ] Verify bundles, archival refs and final file manifests.
- [ ] Run final relevant formatting, host checks and X4 Pro build after substantive firmware/build changes.
- [ ] Confirm simulator operation from the main checkout after retirement.
- [ ] Complete firmware-handoff’s human acceptance boundary where applicable.
- [ ] Provide the owner with the physical X4 Pro checklist for affected reading, navigation, Unicode, input, refresh and storage behavior.
- [ ] Update the plan checkboxes, project status, disposition manifest and ledger.
- [ ] Retain the recovery set. With no authorized commits, Git history is not yet a record of the uncommitted consolidation.

The final report must state:

1. Canonical checkout and branch.
2. Adopted and archived contributions.
3. Retired worktrees and the external snapshot state.
4. Commands/results and candidate identity.
5. Unresolved checks, hardware work and remaining V2 backlog.
6. Recovery locations and restoration instructions.

Mark the campaign **DONE** only when its required criteria are met. Otherwise report the completed packages and precise remaining blockers. Do not ask for routine permission to continue between packages, and do not claim commits, publication, hardware verification or acceptance that did not occur.

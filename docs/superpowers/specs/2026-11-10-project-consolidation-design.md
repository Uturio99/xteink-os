# XTEInk X4 Pro Project Consolidation — Design Specification

Status: approved
Date: 2026-11-10
Plan: [2026-11-10-project-consolidation.md](../plans/2026-11-10-project-consolidation.md)
Inventory: [CONS-01-INVENTORY.md](../audits/CONS-01-INVENTORY.md)

## 1. Purpose

Consolidate development of the XTEInk X4 Pro firmware into one authoritative checkout at
`D:\Work\AI\XTEInk X4 Pro\xteink-os`, with `develop` as its only active local branch. Preserve
useful work and recoverable history from the sibling worktrees and branches, archive
superseded alternatives, and retire redundant worktrees without losing unique commits,
untracked files, ignored personal assets, or evidence.

This campaign produces a stable consolidated foundation. It does not finish every
outstanding native NoveOS V2 screen; remaining V2 work becomes backlog.

## 2. Context

The repository currently has seven registered worktrees: the main checkout, five in-scope
sibling worktrees, and one external Codex worktree. Work has accumulated across local
branches, remote branches, detached tips, uncommitted patches, and two evidence folders
(`WP1A-hardware-checkpoint-372ccca1`, `WP1A-simulator-evidence-372ccca1`). The current
CONS-01 recovery set at `.local-archive/consolidation-2026-11-10/` already preserves 74
files (verified by hash) and 28 refs across two verified bundles.

The main checkout is at `01878fe1` on `codex/wp1-library-reader-return`, with the FreeInk
SDK pinned at `946e6ac3`. No stash exists.

## 3. Architecture and boundaries

- Firmware foundation: C++/PlatformIO, CrossPoint Reader, pinned FreeInk SDK.
- Host suites: existing CMake/CTest targets pinned to GoogleTest by the root CMake file.
- Simulator: pinned Linux/WSL2 X4 Pro profile (`simulator_x4_pro`).
- X4 Pro is the acceptance device; target it first while keeping the HAL/SDK boundaries
  needed for future devices.
- Preserve CrossPoint/FreeInk ownership boundaries and existing runtime contracts.
- Do not introduce new APIs, state managers, or frameworks solely for the consolidation.

## 4. Settled decisions

1. The CONS plan dated 2026-11-10 controls the campaign.
2. NoveOS V2 / Hybrid Editorial Index controls product presentation.
3. The STEP 09 REC-02 revision at `04b6b754` supplies the latest inspected native
   integration guidance and must be reconciled with current source.
4. Preserve currently available reading, Library, import, dictionary, OPDS, plugin, sync,
   settings, storage, and recovery functionality.
5. Keep the SDK and simulator pins unless a separately authorized task changes them.
6. Preserve personal configuration and uncommitted work.
7. Do not create/amend commits, push, change GitHub branches, open/close PRs, or flash.
8. Do not merge an entire WIP branch merely because it is newer.
9. Keep the external Codex worktree as a detached historical snapshot; do not delete its
   folder or chat data.
10. Use one writer. If files change unexpectedly while reading, pause mutations and resolve
    competing writers.
11. Repository-facing documentation is written in English; owner communication is in
    Vietnamese.

## 5. Disposition model

Every unique contribution receives exactly one decision:

- `KEEP` — required current implementation already present.
- `ADOPT` — missing change fits the approved contract and passes its relevant checks.
- `REPAIR` — required implementation has a demonstrated consolidation-related failure.
- `ARCHIVE` — superseded, experimental, conflicting, or insufficiently proven for adoption.
- `GENERATED` — reproducible output whose source and regeneration path remain available.

A module is never classified as removable merely because its code is old or hard to test.
Before removal, check callers, build configuration, runtime registration, scripts, code
generation, and generated assets. Archived changes remain recoverable and carry a clear
reconsideration condition.

## 6. Work packages

- **CONS-01** — Verify the recovery boundary and establish a usable host/firmware
  baseline. No worktree or branch is removed.
- **CONS-02** — Produce one authoritative documentation chain: operating contract, current
  status, capability ledger, and consolidation plan. Resolve incompatible direction.
- **CONS-03** — Produce `docs/superpowers/audits/CONS-03-DISPOSITION.md` dispositioning every
  branch/worktree/evidence delta.
- **CONS-04** — Integrate selected changes in bounded batches and verify the canonical
  checkout independently of every sibling worktree.
- **CONS-05** — Archive refs, detach the external snapshot, retire the allowlisted
  worktrees, promote `develop`, and hand off.

## 7. Verification layers

Source assertions, host suites, simulator interaction, and hardware behavior are separate
evidence layers and must not be conflated. Historical evidence does not verify the current
candidate. A required check that cannot run leaves the relevant status incomplete and is
reported with its precise blocker.

Relevant host test targets include `X4ProHomeLayoutTest`, `LibraryIndexFileTest`,
`ButtonPressNavigationTest`, `HomeButtonInputTest`, `LibraryTextTest`, and
`Utf8ComposeTest`. Firmware is verified with `pio run -e x4pro`; the simulator with the
`simulator_x4_pro` profile from the main checkout in WSL.

## 8. Five priority risks

1. Losing a unique commit, untracked file, or ignored personal asset.
2. Removing code reached through configuration, registration, or generated sources.
3. Leaving build/runtime paths dependent on a retired worktree.
4. Presenting historical evidence as verification of the current candidate.
5. Regressing reading state, progress, navigation, or SD/USB ownership.

## 9. Recovery boundary

- Original recovery set: `.local-archive/consolidation-2026-11-10/` (never overwritten).
- Campaign workspace: `.local-archive/consolidation-2026-11-10-campaign/` (Git-ignored via
  `*.local*` and `.git/info/exclude`).
- Archival namespace for refs: `refs/archive/consolidation-2026-11-10/`.
- Campaign bundle: refreshed to preserve the archival namespace before visible refs are
  reduced.
- Git objects are never deleted; Codex-owned checkpoint refs are never modified.

## 10. Completion boundary

The campaign is `DONE` only when: all history and personal work can be restored; the
canonical checkout builds and its active tools no longer reference sibling worktrees;
dispositions are complete and recoverable; archived refs verify; the allowlisted worktrees
are retired through Git mechanisms; `develop` is the only local head; and the handoff reports
commands, candidate identity, unresolved checks, hardware work, and the remaining V2
backlog. Anything short of that is reported as completed packages plus precise blockers.

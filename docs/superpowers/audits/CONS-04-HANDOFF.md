# CONS-04 handoff — 2026-11-10

Status: **CONS-01..04 implemented and checked; CONS-05 not started** (destructive, awaiting
explicit owner confirmation).
Canonical checkout: `D:\Work\AI\XTEInk X4 Pro\xteink-os`.
Base: `01878fe1` on `codex/wp1-library-reader-return`.

The campaign ledger is Git-ignored:
`.local-archive/consolidation-2026-11-10-campaign/progress.md`. This note is the
repository-visible continuation record.

## Completed

- **CONS-01** — Recovery verified. Fresh all-ref bundle covers 36/36 live refs (including
  the 7 `refs/codex/*` refs the old bundles missed); 17/17 required commits present;
  archive 76/76 hashes. SDK uniqueness proven by full-tree comparison.
- **CONS-02** — Documentation chain recovered and reconciled to the canonical checkout.
  `EXECUTION_CONTRACT`, `EXECUTION-RECOVERY`, `STEP-03/04/05` recovered; `STEP-09` from
  `04b6b754`; `PROJECT_MAP`/`PROJECT_STATUS`/`BASELINE`/`AGENTS.md` updated; M0/M1 marked
  historical. Local links: 184/184.
- **CONS-03** — `docs/superpowers/audits/CONS-03-DISPOSITION.md` records a decision for
  every branch/worktree/evidence delta, including accepted limitations.
- **CONS-04** — Integrated and checked:
  - `scripts/simulator_compat.py` uses the rec01 stdin form
    (`--unidiff-zero --ignore-whitespace`).
  - `scripts/simulator_c11.py` added (required pinned-simulator shim).
  - `.gitattributes` normalizes the simulator/JPEGDEC/SdFat patch series to LF.
  - `lib/Xtc/Xtc/XtcParser.cpp` rejects `chapterCount > 1024` before `reserve()`.
  - `test/xtc_chapters/` added (6 tests); the over-limit test failed before the fix
    (read end 98552 vs 56) and passes after.
  - Four read-only reviews completed (correctness, architecture, embedded, i18n/docs).

## Recorded evidence

| Check | Result |
|---|---|
| Host suite (WSL, `build/test-consolidation-wsl`) | **909/909 PASS** |
| `pio run -e x4pro` | **SUCCESS** — RAM 111740/327680 (34.1%), Flash 5403842/6553600 (82.5%) |
| `simulator_x4_pro` | **SUCCESS** at pinned simulator `6730fe5f` |
| Simulator patch paths | fresh apply, already applied, genuine mismatch, wrong revision all exercised |
| Formatting | clang-format 23.1.5 `--dry-run --Werror` passes for changed C++ |

The simulator was built from a byte-equivalent ext4 rsync mirror: a direct `/mnt/d`
build stalls in WSL 9p (`p9_client_rpc`). This is an environment blocker, not a code
failure. Visible simulator launch and hardware behavior were **not** run.

## Next actions (CONS-05 — do not start without owner confirmation)

1. Confirm the destructive scope with the owner.
2. Create `refs/archive/consolidation-2026-11-10/` using the recorded expected SHAs;
   verify every archived ref; never modify Codex-owned checkpoint refs.
3. Refresh the campaign recovery bundle to preserve the archival namespace; test restore.
4. Detach the external Codex snapshot at its actual HEAD (expected `ea8dbd94`); preserve
   any new work and keep the folder/chat metadata.
5. Retire exactly these worktrees through `git worktree remove`, after verifying their
   unique content against the disposition manifest:
   - `xteink-os-rec01`
   - `xteink-os-simulator-wp1a-372ccca1`
   - `xteink-os-step06`
   - `xteink-os-wp1b`
   - `xteink-os-wp24`
   Do not use forced removal to bypass unpreserved dirty content.
6. Promote the canonical branch to `develop` only after archival: preserve old
   `develop@c9d12b52`, remove it with an expected-SHA guard, rename the current branch
   without resetting the working tree, track `origin/develop`, restrict `origin` fetch,
   and preserve then remove obsolete remote-tracking refs.
7. Re-run the final formatting, host check and X4 Pro build after substantive changes;
   confirm simulator operation; update the plan checkboxes, status and ledger.
8. Hand off the physical X4 Pro checklist and the recovery instructions.

## Known limitations and risks

- The owner's personal `platformio.local.ini` redefines `[env:simulator]`, uses an
  unpinned simulator URL and omits `simulator_compat.py`. It was backed up, temporarily
  emptied for pinned verification, then restored byte-identically
  (SHA256 `5258D33F...66AB87C`). The owner should fix this file to stop shadowing the
  tracked pin.
- `MAX_CHAPTERS = 1024` caps record count, not bytes (~28 KB vector plus up to ~98 KB of
  names). This narrows, but does not eliminate, the `-fno-exceptions` abort risk on the
  380 KB C3 baseline. Reconsider after an X3/X4 peak-heap measurement.
- A valid XTC table with more than 1024 records loses chapter navigation (the book stays
  readable); a table whose upper bound falls back to `fileSize` with a large trailing
  region can be rejected. This is a deliberate safety trade-off.
- No commit, push, flash or hardware acceptance was claimed before this handoff; the
  reviews were read-only and did not run tests.

## Recovery

- Original recovery set: `.local-archive/consolidation-2026-11-10/` (never overwritten).
- Campaign workspace: `.local-archive/consolidation-2026-11-10-campaign/`
  (`progress.md`, `history.bundle`, `logs/`, `manifests/`, `verify/`).
- Disposition and accepted limitations:
  [CONS-03-DISPOSITION.md](CONS-03-DISPOSITION.md).
- Plan: [2026-11-10-project-consolidation.md](../plans/2026-11-10-project-consolidation.md).

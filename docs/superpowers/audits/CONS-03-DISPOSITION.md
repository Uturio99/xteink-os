# CONS-03 — Branch, Worktree and Evidence Disposition

Status: **complete** (decisions recorded; ADOPT items implemented and verified in CONS-04, 2026-11-10)
Date: 2026-11-10
Base: `01878fe194fb511b68c3f15d710f371dd57c63a9` (`codex/wp1-library-reader-return`)
Plan: [2026-11-10-project-consolidation.md](../plans/2026-11-10-project-consolidation.md)
Recovery: `.local-archive/consolidation-2026-11-10/` (original) and `.local-archive/consolidation-2026-11-10-campaign/` (campaign)

Every unique contribution below has one decision: `KEEP`, `ADOPT`, `REPAIR`, `ARCHIVE` or
`GENERATED`. `ARCHIVE` means recoverable and preserved, not deleted. No branch is merged
wholesale. Verification names the check and its observed state; a pending check is marked
PENDING rather than passed.

## Summary

| Source | Unique delta | Decision |
|---|---|---|
| Current `01878fe1` | Foundation: Home V2 layout, Library return (via `372ccca1`), pinned simulator env | **KEEP** |
| `17773ef2` | Execution contract, recovery audit, STEP 03–05, reconciled entry points | **ADOPT** (applied) |
| `04b6b754` | Latest STEP 09, `.gitattributes` LF for simulator patches, `--unidiff-zero` | **ADOPT** (STEP 09 + attributes; unidiff folded into rec01 fix) |
| `rec01` working tree | CRLF/WSL-safe simulator patch application | **ADOPT** (supersedes `04b6b754`) |
| `080c5619` | WP2.1–2.4 Home source, agent skills, STEP 07 browser report | **ARCHIVE** (historical evidence) |
| `f6c9151e` local wp24 | Home/cover/nav, BookMilestone, Settings/OPDS Browse + Plugin Hub, tests | **ARCHIVE** (superseded/unaccepted) |
| `1ab06079` remote wp24 | XTC chapter-count bound + `xtc_chapters` test; PROJECT_PLAN; rest duplicate | **ADOPT** (XTC bound + test), rest **ARCHIVE** |
| `2586f4de` wp24-home-lean | Alternative lean Home variant | **ARCHIVE** (alternative) |
| `773284fb` typography bake-off | Bake-off scripts/evidence, 8 font headers, `main.cpp` registration | **ARCHIVE** (experiment) |
| `wp1b` working tree | Font registration experiment + 6 headers | **ARCHIVE** (experiment) |
| `c9d12b52` / `0861aeaf` | Design assets/design-002, FontDecompressor, serif specimen, `simulator_c11.py` | **ADOPT** `simulator_c11.py`; rest **ARCHIVE** |
| Historical checkpoints | `91eb3416`, `bfe6140a`, `877461d3`, `1225e580`, `f9b6e526`, `7f04359f` | **KEEP** history / **ARCHIVE** content; no merge |
| STEP 06 worktree `57d0e9b1` | No unique commits; clean | **ARCHIVE** (worktree retirement) |
| Simulator worktree `372ccca1` | `platformio.ini` +193 and untracked WSL INI (CRLF noise otherwise) | **ARCHIVE** config; **KEEP** main profile |
| Two WP1A evidence folders | Firmware binary, checksums, screenshots, fixtures, logs, scripts | **ARCHIVE** (evidence) |
| Main personal/ignored files | `.serena/`, `platformio.local.ini`, `font_convert_dev/`, review zip | **KEEP** (personal) |

## Detailed records

### 1. Current foundation — `01878fe1` — **KEEP**

| Field | Content |
|---|---|
| Source | `refs/heads/codex/wp1-library-reader-return` → `01878fe194fb511b68c3f15d710f371dd57c63a9` |
| Delta | Current firmware, including Home V2 hero/shelf layout (`X4ProHomeLayout.h`, `CoverGridHomeUi.cpp`), NAV-001 Library return inherited from `372ccca1`, and `[env:simulator_x4_pro]` in `platformio.ini`. |
| Requirement | Canonical development foundation for the single-workspace target. |
| Owner/use | All firmware; `src/activities`, `src/components`, `lib/`, `test/`. |
| Decision | **KEEP**. |
| Verification | Host suite at `01878fe1`: 903/903 PASS in WSL (2026-11-10). Recorded X4 Pro firmware baseline PASS (prior log, historical layer). |
| Recovery | Branch head; campaign `history.bundle`; working-tree diff preserved. |

### 2. Documentation recovery — `17773ef2` — **ADOPT**

| Field | Content |
|---|---|
| Source | `17773ef2` (`fix/rec02-simulator-patch-wsl`, `checkpoint/rec-01-execution-recovery-2026-11-09`), merge-base `7d63f480`. |
| Delta | `docs/EXECUTION_CONTRACT.md`; `EXECUTION-RECOVERY.md`; `STEP-03/04/05`; reconciled `PROJECT_MAP`, `PROJECT_STATUS`, `BASELINE`, `STEP-06`, `STEP-09`; historical notices in design docs. |
| Requirement | One operating contract, one current status, one capability ledger and one consolidation plan. |
| Owner/use | Agent entry points (`AGENTS.md`, `docs/PROJECT_MAP.md`), design authority, audit trail. |
| Decision | **ADOPT** (applied 2026-11-10). |
| Verification | Current branch changed none of these docs since merge-base `7d63f480`, so recovery cannot clobber branch work. Local Markdown link check: 179 links, 0 broken. |
| Recovery | Campaign `history.bundle`; extracted copies in `recovered/full17773ef2/`. |

### 3. REC-02 documentation and attributes — `04b6b754` — **ADOPT**

| Field | Content |
|---|---|
| Source | `04b6b754` (`origin/feat/noveos-native`), child of `17773ef2`. |
| Delta | Updated `STEP-09-NATIVE-INTEGRATION-PLAN.md`; new `.gitattributes` (`scripts/simulator-simlog-init.patch`, `scripts/jpegdec_patches/*.patch`, `scripts/sdfat_patches/*.patch` as `text eol=lf`); `simulator_compat.py` adds `--unidiff-zero`. |
| Requirement | Latest inspected STEP 09; stable LF patch bytes for Windows/WSL. |
| Owner/use | Native delivery plan; simulator build scripts. |
| Decision | **ADOPT** STEP 09 and `.gitattributes`; adopt `--unidiff-zero` as part of the rec01 fix (record 4), not as a separate file-path variant. |
| Verification | STEP 09 header reconciled with `01878fe1`; patch file is LF-only (0 CRLF, 21 LF). |
| Recovery | Campaign `history.bundle`; `recovered/full04b6b754/`. |

### 4. Simulator patch fix — `rec01` working tree — **ADOPT**

| Field | Content |
|---|---|
| Source | `xteink-os-rec01`, `fix/rec02-simulator-patch-wsl` @ `17773ef2`, ` M scripts/simulator_compat.py`. |
| Delta | Reads the patch as text and pipes it to `git apply` with `--unidiff-zero --ignore-whitespace`, retaining revision check and already-applied detection. |
| Requirement | The demonstrated Windows/WSL CRLF patch failure. |
| Owner/use | `scripts/simulator_compat.py`, simulator `pre:` script. |
| Decision | **ADOPT**; it supersedes the `04b6b754` file-path variant because it handles CRLF and zero-context hunks. |
| Verification | PASS (2026-11-10): fresh apply, already-applied, genuine mismatch and wrong revision all exercised; pinned simulator `6730fe5f`. |
| Recovery | `patches/rec01-simulator_compat.patch`, `files/rec01/simulator_compat.py`. |

### 5. WP2 Home source and STEP 07 browser evidence — `080c5619` — **ARCHIVE**

| Field | Content |
|---|---|
| Source | `origin/feat/m2-wp2-home` @ `080c5619`, merge-base `f9b6e526`; 7 commits. |
| Delta | WP2.1–2.4 Home/cover/settings source; four agent skills; `docs/superpowers/step-07/` browser report, captures and `verify.js`. |
| Requirement | Planning/audit evidence; not current product authority. |
| Owner/use | Historical Home candidate; browser-only evidence. |
| Decision | **ARCHIVE**. Current `01878fe1` already carries the Home V2 foundation, and the plan forbids restoring an older product authority. |
| Verification | Base has Home V2 layout markers (`heroCover`, `shelfHeader`); no active document depends on this branch. |
| Recovery | Campaign `history.bundle` (`refs/remotes/origin/feat/m2-wp2-home`); commit content recoverable from the bundle. |

### 6. WP2.4 local candidate — `f6c9151e` — **ARCHIVE**

| Field | Content |
|---|---|
| Source | `feat/wp24-fixes` @ `f6c9151e`, merge-base `f9b6e526`; 7 commits. |
| Delta | Home/cover/layout/FileBrowser changes; `BookMilestone` (chapter/page detail); Settings Browse row and Plugin Hub entry; `book_progress` tests. |
| Requirement | Earlier Home/progress/settings candidate; unaccepted. |
| Owner/use | Home, BookProgress, Settings/OPDS, plugins. |
| Decision | **ARCHIVE**. Superseded by the current Home V2 foundation; `BookMilestone` is unused in the base; the Settings/OPDS changes are unaccepted UX features beyond consolidation scope. |
| Verification | Base lacks `test/book_progress`; base `BookProgress.h` exposes only `loadBookProgress`; host suite passes without these deltas. |
| Recovery | Campaign `history.bundle` (`refs/heads/feat/wp24-fixes`); diffs reproducible from the bundle. |

### 7. WP2.4 remote + XTC hardening — `1ab06079` — **ADOPT (XTC bound), ARCHIVE (rest)**

| Field | Content |
|---|---|
| Source | `origin/feat/wp24-fixes` @ `1ab06079`, merge-base `f9b6e526`; 8 commits (adds one to `f6c9151e`). |
| Delta | `lib/Xtc/Xtc/XtcParser.cpp`: reject `chapterCount > 1024` before `reserve()`; `test/xtc_chapters/` (227-line test + stubs); `docs/PROJECT_PLAN.md`. |
| Requirement | Data-safety: a crafted XTC header can inflate `chapterCount`; `m_chapters.reserve()` can abort under `-fno-exceptions`, and the loop reads unbounded. |
| Owner/use | `XtcParser::readChapters`; XTC reader. |
| Decision | **ADOPT** the 10-line bound and its test; **ARCHIVE** the duplicate Home/progress/settings deltas and `PROJECT_PLAN.md`. |
| Verification | PASS (2026-11-10): the over-limit test failed before the fix (read end 98552 vs 56) and 6/6 pass after; host 909/909; `pio run -e x4pro` SUCCESS. Four read-only reviews returned; accepted limitations are recorded under "Review findings and accepted limitations". Human acceptance remains open. |
| Recovery | Campaign `history.bundle` (`refs/remotes/origin/feat/wp24-fixes`); test source recoverable from the bundle. |

### 8. WP2.4 lean Home — `2586f4de` — **ARCHIVE**

| Field | Content |
|---|---|
| Source | `origin/feat/wp24-home-lean` @ `2586f4de`, merge-base `f9b6e526`; 6 commits. |
| Delta | Alternative lean Home/cover/BookProgress variant. |
| Requirement | Alternative Home candidate. |
| Owner/use | Home/cover. |
| Decision | **ARCHIVE**; an alternative, not the current foundation. |
| Verification | Superseded by current Home V2; no active dependency. |
| Recovery | Campaign `history.bundle`. |

### 9. Typography bake-off — `773284fb` — **ARCHIVE**

| Field | Content |
|---|---|
| Source | `origin/checkpoint/native-typography-bakeoff-2026-11-10` @ `773284fb`, merge-base `7d63f480`. |
| Delta | `docs/design/experiments/native-typography-bakeoff/` (scripts, variants, `visual-review.json`), eight font headers, `main.cpp` registration (+32), Home/cover tweaks, font-coverage test changes. |
| Requirement | Typography experiment and evidence. |
| Owner/use | Font pipeline experiment; not the active registry. |
| Decision | **ARCHIVE**. The experiment's own review is NOT TESTED; it must stay out of the active font registry. |
| Verification | `visual-review.json` states NOT TESTED; base font registry unchanged. |
| Recovery | Campaign `history.bundle`; extracted evidence can be restored from the bundle. |

### 10. wp1b font registration experiment — working tree — **ARCHIVE**

| Field | Content |
|---|---|
| Source | `xteink-os-wp1b`, detached `cc31d678`; ` M src/main.cpp`; six untracked `notoserif_*_regular.h`. |
| Delta | Strategy B font registration in `main.cpp` and six headers. |
| Requirement | Font feasibility experiment. |
| Owner/use | Font registration experiment. |
| Decision | **ARCHIVE**; preserved outside the active registry. |
| Verification | Recovery-set hashes match the live files (12/12 single-file pairs, 0 mismatches). |
| Recovery | `patches/wp1b-main-cpp.patch`, `files/wp1b/*.h`, `files/wp1b/main.cpp`. |

### 11. Develop-line design assets and simulator shim — `c9d12b52` / `0861aeaf` — **ADOPT (one file), ARCHIVE (rest)**

| Field | Content |
|---|---|
| Source | `refs/heads/develop` @ `c9d12b52` (local-only), `0861aeaf`, merge-base `ad53eb64`. |
| Delta | Design v0.x–v1.1 assets, design-002 explorations, `FontDecompressor` change, serif specimen, HomeActivity/`main.cpp`, and `scripts/simulator_c11.py` (3 lines). |
| Requirement | Historical design provenance; `simulator_c11.py` is a required build input for the pinned simulator dependency. |
| Owner/use | Design archive; simulator build. |
| Decision | **ADOPT** `scripts/simulator_c11.py`; **ARCHIVE** the design assets and other source changes as historical. |
| Verification | PASS (2026-11-10): build failed with `missing SConscript file 'scripts/simulator_c11.py'`; with the recovered 3-line file, `simulator_x4_pro` builds against pinned `6730fe5f`. |
| Recovery | Campaign `history.bundle`; `scripts/simulator_c11.py` content reproduced from `c9d12b52`. |

### 12. Historical checkpoints and upstream — **KEEP history, no merge**

| Field | Content |
|---|---|
| Source | `91eb3416` (`backup/pre-m0-consolidation-2026-11-02`), `bfe6140a` (`chore/m0-closeout`), `877461d3` (`checkpoint/h1.1a-native-home`), `1225e580` (`feat/m1-unified-ui-collection`), `f9b6e526` (`feat/m2-wp1-font-feasibility`), `7f04359f` (`upstream/develop`). |
| Delta | Historical design/checkpoints; upstream has 52 commits not in this project's line. |
| Requirement | Provenance and comparison only. |
| Owner/use | History. |
| Decision | **KEEP** refs/history; **ARCHIVE** content as reference. Do not merge `upstream/develop`. |
| Verification | Refs resolve; required commits present in the campaign bundle (17/17). |
| Recovery | Campaign `history.bundle`; original two bundles. |

### 13. STEP 06 worktree — `57d0e9b1` — **ARCHIVE / retire**

| Field | Content |
|---|---|
| Source | `xteink-os-step06`, `codex/step06-ux-flows` @ `57d0e9b1`; merge-base with base is `57d0e9b1` (0 unique commits). |
| Delta | None; clean working tree; SDK uninitialized. |
| Requirement | None beyond history already in the base. |
| Owner/use | — |
| Decision | **ARCHIVE** ref; retire the worktree in CONS-05 (no unique content). |
| Verification | `git status` clean; `git rev-list --count 57d0e9b1..57d0e9b1 = 0`; SDK 0 files. |
| Recovery | Campaign `history.bundle`. |

### 14. Simulator worktree — `372ccca1` — **ARCHIVE config, KEEP main profile**

| Field | Content |
|---|---|
| Source | `xteink-os-simulator-wp1a-372ccca1`, detached `372ccca1`; WSL-created. |
| Delta | Real `platformio.ini` +193 lines (extra simulator envs), untracked `platformio.simulator-linux-wsl.ini`; 1040 CRLF-only modifications. |
| Requirement | X4 Pro simulator profile. |
| Owner/use | Simulator environments. |
| Decision | **KEEP** the main `[env:simulator_x4_pro]`; **ARCHIVE** the extra worktree envs/INI as preserved configuration. |
| Verification | `git diff --ignore-cr-at-eol` = only `platformio.ini` (+193); main defines `simulator_x4_pro` and builds successfully against pinned `6730fe5f` from the byte-equivalent ext4 mirror; the direct `/mnt/d` build stalls in WSL 9p. |
| Recovery | `patches/simulator-platformio.patch`, `files/simulator/platformio.ini`, `files/simulator/platformio.simulator-linux-wsl.ini`. |

### 15. WP1A evidence folders — **ARCHIVE**

| Field | Content |
|---|---|
| Source | `WP1A-hardware-checkpoint-372ccca1`, `WP1A-simulator-evidence-372ccca1`. |
| Delta | Firmware binary, checksums, screenshots, fixtures, logs, replay scripts. |
| Requirement | Revision/hash provenance for the WP1A result. |
| Owner/use | Evidence archive. |
| Decision | **ARCHIVE**; absorb into the canonical evidence structure after verifying the existing copies. |
| Verification | 47 files checksummed; archived copies match originals (0 mismatches); campaign manifest 76/76. |
| Recovery | `wp1a/hardware-checkpoint-372ccca1/`, `wp1a/simulator-evidence-372ccca1/`. |

### 16. Personal and ignored files (main) — **KEEP**

| Field | Content |
|---|---|
| Source | `.serena/`, `platformio.local.ini`, `scripts/font_convert_dev/`, `xteink-os-design-002-review.zip`. |
| Delta | Personal tool configuration and local tooling. |
| Requirement | Preserve owner configuration; never stage. |
| Owner/use | Local environment. |
| Decision | **KEEP** (ignored, archived). |
| Verification | Archive hashes match live originals (trees/single files 0 mismatches). |
| Recovery | `files/main/.serena/`, `files/main/platformio.local.ini`, `files/main/font_convert_dev/`, `files/main/xteink-os-design-002-review.zip`. |

## Coverage of the plan's source list

- Current `01878fe1`, `17773ef2`, `04b6b754`, `080c5619`, `f6c9151e`, `1ab06079`,
  `2586f4de`, `773284fb`, `wp1b`, `c9d12b52`, STEP 06, simulator worktree and both WP1A
  folders are all dispositioned above.
- Historical checkpoints and `upstream/develop` are preserved as reference, never merged.
- `F01–F44` identifiers are preserved in
  [STEP-03-FEATURE-INVENTORY.md](STEP-03-FEATURE-INVENTORY.md); this manifest does not
  renumber them.

## Review findings and accepted limitations

Four independent read-only reviews ran on 2026-11-10 (correctness, architecture,
embedded, i18n/docs). Resolved findings:

- Stale authority text was removed from `AGENTS.md`, `EXECUTION_CONTRACT.md`, `STEP-09`
  and `PROJECT_STATUS.md`: no second workspace, REC-01 current-head reconciliation is
  complete, and the simulator/Library-return are present.
- The rejection tests now assert `hasChapters() == false` directly instead of a
  near-tautological compound assertion.
- `.gitattributes` is documented with its actual three scoped globs.
- CONS-01 now records the resolved host/simulator baseline as a dated update while
  retaining its historical "NOT RUN" table.

Accepted limitations (not fixed in this consolidation; no hardware measurement yet):

- The `MAX_CHAPTERS = 1024` bound caps record count, not retained bytes. A table at the
  limit can still allocate about 28 KB for the vector plus up to ~98 KB of chapter names,
  which is significant on the 380 KB C3 baseline. Reconsider after a measured X3/X4
  peak-heap result; then lower the cap, cap the `reserve`, or add a name-byte budget.
- A valid table with more than 1024 records loses chapter navigation entirely (the book
  remains readable). A table whose upper bound falls back to `fileSize` with a large
  trailing region can be rejected. Cap-and-parse would preserve partial navigation but
  spends the same memory; the hard bound is a deliberate safety trade-off.
- `readChapters()` does not set `m_lastError`, so `CORRUPTED_HEADER` is not observable
  through `getLastError()`; this is pre-existing and no caller depends on it.
- The simulator revision constant is duplicated from `platformio.ini`; a pin bump fails
  with a clear error but does not point at the pin.
- `scripts/simulator_c11.py` appends `-std=gnu11` env-wide rather than only to the QRCode
  dependency; benign for the verified build.

These are backlog items with reconsideration conditions below; none blocks the
consolidated foundation.

## Reconsideration conditions

- Home/cover/BookMilestone/Settings deltas: reconsider only inside an authorized V2
  screen task that names the acceptance case; not as part of consolidation.
- Typography and wp1b font experiments: reconsider only with measured footprint and a
  separate font-selection decision.
- XTC bound: if the adopted regression check cannot be made to fail on the unfixed base,
  record it as already-handled and do not claim a fix.
- Simulator extra envs: reconsider only if an X4 Pro scenario cannot run with the main
  `simulator_x4_pro` profile.

# CONS-01 — Inventory and Recovery Point

Status: **complete for inventory and recovery**; baseline firmware build recorded, host suites blocked by missing host tools (see Baseline results).
Date: 2026-11-10
Plan: [2026-11-10-project-consolidation.md](../plans/2026-11-10-project-consolidation.md)
Base: `xteink-os` at `01878fe194fb511b68c3f15d710f371dd57c63a9`, branch `codex/wp1-library-reader-return`.

> **Update 2026-11-10 (consolidation campaign).** The recorded host "NOT RUN" gap is
> resolved: CMake 4.4.4 and Expat dev headers were installed locally in WSL, and the host
> suite passed **909/909** at the candidate. The X4 Pro build succeeded (RAM 34.1%,
> Flash 82.5%). The simulator builds against pinned `6730fe5f` from a byte-equivalent ext4
> mirror; the direct `/mnt/d` build stalls in WSL 9p. Logs:
> `.local-archive/consolidation-2026-11-10-campaign/logs/`. The Baseline results table
> below is the historical 2026-11-09 snapshot.

## Environment

- Host: Windows; the simulator worktree was created from WSL2 (`Ubuntu`) and its `.git` file uses a `/mnt/d/...` path.
- PlatformIO Core 6.2.0.post1.
- SDK pin: `freeink-sdk` at `946e6ac3d7a9d590ba3224428e6183d55ac7917c` (`origin/feat/boox-picco-131-g946e6ac`), clean, detached. No SDK change is part of this consolidation.
- No stash exists (`git stash list` empty).

## Scope folders

| Folder | Kind | State |
| --- | --- | --- |
| `xteink-os` | Main worktree | Development base, `01878fe1` |
| `xteink-os-rec01` | Linked worktree | `17773ef2`, one uncommitted file |
| `xteink-os-simulator-wp1a-372ccca1` | Linked worktree, detached | `372ccca1`, WSL-created |
| `xteink-os-step06` | Linked worktree | `57d0e9b1`, clean |
| `xteink-os-wp1b` | Linked worktree, detached | `cc31d678`, uncommitted font experiment |
| `xteink-os-wp24` | Linked worktree | `f6c9151e`, clean, 7 commits |
| `WP1A-hardware-checkpoint-372ccca1` | Evidence folder | Firmware binary, checksums, logs |
| `WP1A-simulator-evidence-372ccca1` | Evidence folder | Fixtures, logs, screenshots, scripts |

`C:\Users\Uturio\.codex\worktrees\xteink-step07a-visual\xteink-os` (`ea8dbd94`, `feat/xteink-os-refoundation`) is outside the folder and is recorded only; it is not touched.

## Worktrees and personal work

| Worktree | HEAD | Ref | Working tree | Personal work to preserve |
| --- | --- | --- | --- | --- |
| `xteink-os` | `01878fe1` | `codex/wp1-library-reader-return` | 2 untracked entries + new plan doc | `.serena/`, `xteink-os-design-002-review.zip`; ignored `platformio.local.ini`, `.serena/project.local.yml`, `scripts/font_convert_dev/` |
| `xteink-os-rec01` | `17773ef2` | `fix/rec02-simulator-patch-wsl` | ` M scripts/simulator_compat.py` | CRLF/WSL-safe patch application |
| `xteink-os-simulator-wp1a-372ccca1` | `372ccca1` | detached | 1040 files "modified" (CRLF only), 1 real edit, 1 untracked | `platformio.ini` (+193 lines), `platformio.simulator-linux-wsl.ini` |
| `xteink-os-step06` | `57d0e9b1` | `codex/step06-ux-flows` | clean | none |
| `xteink-os-wp1b` | `cc31d678` | detached | ` M src/main.cpp`, 6 untracked headers | Strategy B font registration experiment |
| `xteink-os-wp24` | `f6c9151e` | `feat/wp24-fixes` | clean | 7 commits |
| Codex worktree (outside) | `ea8dbd94` | `feat/xteink-os-refoundation` | clean, behind 12 | recorded, not touched |

### Branch inventory

Local branches with unique commits relative to the base:

| Branch | HEAD | Note |
| --- | --- | --- |
| `feat/wp24-fixes` | `f6c9151e` | 7 commits since base: WP2.1–WP2.4 + governance docs |
| `fix/rec02-simulator-patch-wsl` | `17773ef2` | REC-01 close-out docs + uncommitted simulator patch |
| `feat/m2-wp1-font-feasibility` | `f9b6e526` | Parent of wp1b's detached `cc31d678` |
| `checkpoint/rec-01-execution-recovery-2026-11-09` | `17773ef2` | Same commit as `fix/rec02` |
| `codex/wp1-library-reader-return` | `01878fe1` | Current base, also `origin/feat/xteink-os-refoundation` |
| `develop` | `c9d12b52` | 2 commits ahead of `origin/develop`. `0861aeaf` is also on `origin/codex/xteink-os-checkpoint-2026-10-31`; `c9d12b52` is local-only and exists on `develop` and `backup/develop-c9d12b52` only. |
| `backup/develop-c9d12b52` | `c9d12b52` | Backup ref only |
| `noveos-home` | `8bd39511` | Ancestor of base, no unique commits |

Remote-only branches with commits not in the base: `origin/feat/m2-wp2-home` (`080c5619`, 7 commits including the planning docs and STEP 03–06), `origin/feat/noveos-native` (`04b6b754`, 2 REC-02 doc commits), `origin/feat/wp24-home-lean` (`2586f4de`, 6 commits), `origin/checkpoint/native-typography-bakeoff-2026-11-10` (`773284fb`, 1 experiment commit).

## Ignored files reviewed

| Path | Decision |
| --- | --- |
| `.pio/`, `.venv/`, `.venv-font/`, `build/`, `.cache/`, `fs_/` | Regenerable build/runtime output. `fs_/` contains empty directories only. |
| `lib/I18n/I18n*.{h,cpp}`, `src/**/*.generated.h` | Build-generated from sources by existing scripts. Not personal work. |
| `platformio.local.ini` (main) | Personal build configuration — archived. |
| `.serena/project.local.yml`, `.serena/project.yml` (main) | Personal tool configuration — archived; `.serena/cache` excluded. |
| `scripts/font_convert_dev/` (main) | Local font tooling — archived. |

## Recovery set

Location: `.local-archive/consolidation-2026-11-10/`, excluded through `.git/info/exclude` and never built (dot-prefixed, outside `src/`, `lib/`, `test/`).

| Item | Content |
| --- | --- |
| `patches/rec01-simulator_compat.patch` | Uncommitted diff from `rec01` |
| `patches/wp1b-main-cpp.patch` | Uncommitted font experiment diff |
| `patches/simulator-platformio.patch` | Real simulator `platformio.ini` diff (line endings ignored) |
| `files/rec01/simulator_compat.py`, `files/wp1b/main.cpp`, `files/simulator/platformio.ini` | Full copies of the modified files |
| `files/wp1b/notoserif_{12,13,15,17,30,34}_regular.h` | Six untracked font headers |
| `files/simulator/platformio.simulator-linux-wsl.ini` | Untracked simulator environment |
| `files/main/platformio.local.ini`, `files/main/.serena/`, `files/main/font_convert_dev/` | Personal configuration and host tooling |
| `files/main/xteink-os-design-002-review.zip` | Untracked review package |
| `wp1a/hardware-checkpoint-372ccca1/`, `wp1a/simulator-evidence-372ccca1/` | Both evidence folders |
| `manifests/worktrees.txt`, `branches.txt`, `refs.txt`, `submodules.txt`, `stashes.txt` | Git state snapshot |
| `manifests/unique-work.bundle` | Complete history for 5 local branches with unique work, verified with `git bundle verify` |
| `manifests/remaining-refs.bundle` | Complete history for the other 8 local branches and all 15 remote refs (23 refs), verified with `git bundle verify` |
| `manifests/WP1A-SHA256SUMS.txt` | SHA256 for all 47 files in both WP1A evidence folders |
| `manifests/SHA256SUMS.txt` | Hashes for all 74 archive files (includes the firmware baseline log) |

### Verification

- `git bundle verify`: both bundles are okay. `unique-work.bundle` holds 5 refs; `remaining-refs.bundle` holds 23 refs (8 local branches + 15 remote refs). Together they cover all 13 local branches and all 15 remote refs.
- Git restore test: both bundles fetched into an empty repository in the session scratchpad. All 28 refs (13 local + 15 remote, excluding symbolic `origin/HEAD`) resolve to the same commit as the source repository; 17 required HEAD commits (`01878fe1`, `17773ef2`, `372ccca1`, `cc31d678`, `f6c9151e`, `57d0e9b1`, `ea8dbd94`, `c9d12b52`, `080c5619`, `773284fb`, `04b6b754`, `2586f4de`, `0861aeaf`, `91eb3416`, `8bd39511`, `877461d3`, `7f04359f`) are present.
- WP1A checksums: the two binary hashes recorded in `WP1A-hardware-checkpoint-372ccca1/SHA256SUMS.txt` match the files; a fresh manifest covers all 47 files, and archived copies match their originals.
- Restore test: archive copied to the session scratchpad and every file re-hashed — 74/74 match, 0 mismatches.
- Source fidelity: archived copies of the font headers, simulator ini, `platformio.local.ini`, `.serena` files, `font_convert_dev/`, and both WP1A folders match their originals byte for byte.

### Restoring history from the bundles

```sh
git init /path/to/restore && cd /path/to/restore
git fetch "<archive>/manifests/unique-work.bundle" "+refs/heads/*:refs/heads/*"
git fetch "<archive>/manifests/remaining-refs.bundle" "+refs/heads/*:refs/heads/*" "+refs/remotes/*:refs/remotes/*"
```

## Findings and risks

1. **Simulator worktree is WSL-created.** `.git` contains `gitdir: /mnt/d/...`. Windows Git cannot read it; all state was read through `wsl`. This must be normalized when the worktree is retired (CONS-04/05).
2. **CRLF noise, not 1040 edits.** `git status` in that worktree lists 1040 modified files. `git diff --ignore-cr-at-eol` leaves only `platformio.ini` (+193). Do not treat the list as personal work.
3. **wp1b submodule link is broken.** `freeink-sdk/.git` points to `../.git/modules/freeink-sdk`, which does not exist inside a linked worktree. Files are present, but `git status` fails without `--ignore-submodules=all`. Needs repair or re-init before that worktree is used or retired.
4. **Local `develop` is 2 commits ahead of `origin/develop`.** `0861aeaf` is pushed on `origin/codex/xteink-os-checkpoint-2026-10-31`. `c9d12b52` is local-only: it exists on `develop` and `backup/develop-c9d12b52`, and `backup/pre-m0-consolidation-2026-11-02` does **not** contain it. Both bundles preserve it.
5. **Archive lives inside the repo tree.** It is excluded from Git and is not a PlatformIO source directory, so it cannot enter the build.
6. **Generated i18n and icon/web headers are ignored outputs.** They are regenerated by existing scripts and are not personal work.
7. **Host test toolchain is missing.** `cmake` and `ninja` are not installed on Windows or in WSL. Host suites cannot run here until they are installed or the suites run in CI. CONS-05 depends on resolving this.
8. **Windows console encoding breaks PlatformIO logs.** Use `set PYTHONIOENCODING=utf-8` when redirecting `pio run` output.

## Baseline results

| Check | Command | Revision | Result |
| --- | --- | --- | --- |
| Firmware build | `pio run -e x4pro` | `01878fe1` | **PASS** — RAM 34.1% (111,740 / 327,680 B), Flash 82.5% (5,403,838 / 6,553,600 B), 98.86 s. Log: `.local-archive/consolidation-2026-11-10/logs/firmware-build.log` |
| Host unit tests | `pio run -t unit-tests` | `01878fe1` | **NOT RUN** — exit 1 because `cmake` and `ninja` are absent on the Windows host; WSL has g++ 15.2 but also no cmake/ninja. The target shells out to CMake/CTest (`scripts/register_unit_tests_target.py`), so this is an environment gap, not a code failure. |
| Simulator | `pio run -e simulator_x4_pro -t run_simulator` | `01878fe1` | **NOT RUN** — WSL has no `pio`; only the Windows PlatformIO install exists and the simulator env is a Linux target. A prior WSL build exists under the simulator worktree `.pio/build/simulator_x4_pro`. |

Log capture note: PlatformIO's console thread raises `UnicodeEncodeError` on the Windows cp1252 console for non-Latin output, which silently empties redirected logs. Captured logs above were produced with `set PYTHONIOENCODING=utf-8`. Any future baseline command must set that variable or its log will be empty.

## CONS-01 checklist

- [x] Inventory folders, worktrees, branches, detached HEADs, SDK, staged/unstaged/untracked changes.
- [x] Check ignored files for data that cannot be regenerated.
- [x] Record SHAs, paths, hashes, dependency relationships; read WSL worktree in WSL.
- [x] Create the recovery set outside Git and the build flow.
- [x] Test restore and compare hashes.
- [x] Record initial build/test results, including checks that could not run and why.

**Completion evidence:** every worktree, branch, detached HEAD, SDK pin, staged/unstaged/untracked change, and personal ignored file is listed above; the recovery set restores 74/74 files by hash and all 28 refs from the two bundles; no worktree was deleted or modified.

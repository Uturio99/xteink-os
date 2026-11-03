# XTEInk OS — M2 WP1 Font Feasibility Results

> **Document ID:** M2-WP1-RESULTS
> **Status:** **WP1 CLOSED — engineering feasibility accepted by the Product
> Owner, 2026-11-03.** D9 direction recorded in §14. D11 remains provisional
> (§14.1). Tier W (hardware) was **not** performed.
> **Repository:** `xteink-os`
> **Branch / HEAD:** `feat/m2-wp1-font-feasibility` @ `cc31d678`
> **Date:** 2026-11-03
> **Contract:** [M2-ADDENDUM.md](M2-ADDENDUM.md) §4 (WP1)
> **Next work package:** [M2-WP2-HOME-PLAN.md](M2-WP2-HOME-PLAN.md) (plan only)

This document separates four kinds of evidence, which must not be mixed:

| Class | Section | Trust level |
| :-- | :-- | :-- |
| **A. Host measurements** | §4–§7 | Measured on this machine, x86-64, not device behaviour |
| **B. Firmware measurements** | §9–§11 | Measured from real `pio run -e x4pro` cross-compiled binaries |
| **C. Previously reported figures** | §12 | Historic or externally supplied, **not** remeasured here |
| **D. Hardware behaviour** | §13 | **Not measured at all.** Must not be inferred from this document |
| **E. Product decision** | §14 | The Product Owner's D9 direction — a decision, not a measurement |

No production firmware, SDK, build configuration, or test source was modified.
Nothing was flashed, uploaded, committed, pushed, or merged.

---

## 1. Authorization and scope

- **Tier H** (host tests) — completed; 10/10 tests pass (§4).
- **Tier F** (firmware size measurement) — completed; baseline vs Strategy B
  measured in an isolated worktree (§8–§11). No upload, no flash, no serial
  monitor, no `platformio.ini` edit, no dependency version change.
- **Tier W** (hardware) — not authorized, not attempted.
- **WP1 outcome** — accepted by the Product Owner on 2026-11-03. The accepted
  D9 direction is recorded in §14; WP2 planning is a separate document and
  authorizes no implementation.

## 2. Host environment

| Item | Value |
| :-- | :-- |
| Host OS | Windows 11 IoT Enterprise LTSC 10.0.26100 |
| Host test environment | WSL 2, Ubuntu 26.04.1 LTS, kernel `6.18.54.1-microsoft-standard-WSL2` |
| C/C++ compiler (host) | GNU g++ 15.2.0 (Ubuntu 15.2.0-16ubuntu1) |
| CMake / CTest (host) | 3.31.6, user-local, no root, no system package |
| zlib | 1.3.1 |
| GoogleTest | v1.17.0 via `FetchContent` |
| PlatformIO Core | 6.2.0.post1 (firmware builds) |
| Firmware toolchain | `toolchain-xtensa-esp-elf` from the shared PlatformIO package store |

## 3. Host test commands

### 3.1 Contract command — blocked on this host

```text
cmake -S test -B build/test
```

Fails at configure time on an unrelated sibling target:

```text
Could NOT find EXPAT (missing: EXPAT_LIBRARY EXPAT_INCLUDE_DIR)
  content_opf_parser/CMakeLists.txt:1 (find_package)
```

Only the `libexpat1` runtime is installed; no `expat.h`, no `libexpat.so`.
Installing `libexpat1-dev` needs `sudo` and was out of scope.

### 3.2 Commands actually used

A gitignored scaffold (`build/wp1-host/CMakeLists.txt`) reproduces only the
shared scaffolding from `test/CMakeLists.txt` and consumes the committed
`test/vietnamese_font_coverage/CMakeLists.txt` **verbatim** via
`add_subdirectory`:

```text
cmake -S build/wp1-host -B build/wp1-host/out
cmake --build build/wp1-host/out --target VietnameseFontCoverageTest -j 4
ctest --test-dir build/wp1-host/out -R "<filter>" --output-on-failure -V
```

`ctest -R VietnameseFontCoverage` matches zero tests: `gtest_discover_tests`
registers the GoogleTest case names. Correct filter:

```text
-R "A_BuiltinSansCoverage|B_BundledSerifCoverage|C_Vector|FallbackFloor|F1_FlashBytes"
```

## 4. Host test outcomes

| Registered | Passed | Failed | Skipped | Wall time |
| :--: | :--: | :--: | :--: | :-- |
| 10 | 10 | 0 | 0 | 0.23 s |

All five coverage assertions require zero missing codepoints over the
505-codepoint set defined at
[`VietnameseFontCoverageTest.cpp:51-63`](../../../test/vietnamese_font_coverage/VietnameseFontCoverageTest.cpp)
(U+1EA0–U+1EF9, U+01A0/01A1/01AF/01B0, U+0300–U+036F, U+00C0–U+017F,
U+0020–U+007E, plus 12 M1 punctuation/currency codepoints).

## 5. Strategy comparison (host)

| Property | **A. Built-in bitmap** | **B. Bundled UI Serif** | **C. SD vector Serif** |
| :-- | :-- | :-- | :-- |
| Sizes measured | NotoSans 12/14/16/18 | NotoSerif 12/13/15/17/30/34 | NotoSerif-Regular, 12–34 px |
| Vietnamese coverage | **Pass** (505/505 ×4) | **Pass** (505/505 ×6) | **Pass** (505/505) |
| NFC + NFD fixtures | **Pass** | **Pass** | Covered by glyph presence |
| Rasterization | **Pass** | Same `EpdFont` path as A | **Pass** (non-empty ink, 10 sizes) |
| Availability | Always | Static once registered | Only with SD TTF installed |
| Runtime fallback chain | Is the floor | Not exercised | Not exercised |

`FallbackFloor` **passed**: the union of the four built-in NotoSans faces covers
all 505 codepoints.

## 6. F1 — exact const-data flash (host, `sizeof`-based)

| Face | F1 bytes |
| :-- | --: |
| `notoserif_12_regular` | 46,629 |
| `notoserif_13_regular` | 50,442 |
| `notoserif_15_regular` | 54,988 |
| `notoserif_17_regular` | 61,011 |
| `notoserif_30_regular` | 100,774 |
| `notoserif_34_regular` | 115,458 |
| **Total (6 regular faces)** | **429,302** |
| `notosans_16_regular` (methodology check) | 55,416 |

F1 counts const data only and excludes registration code and linker padding, so
it is an estimate of the flash delta, not a substitute for §9.

## 7. PSRAM / glyph-arena diagnostics (host-derived, x86-64)

```text
[C] X4 Pro profile: capacity=1048576 used=51538 highWater=51538 failedAllocSize=0
[C] no-PSRAM profile: capacity=32768 used=29050 highWater=32728 failedAllocSize=84 rasterNull=0
```

The X4 Pro (PSRAM) profile peaks at 4.9 % of the 1 MiB arena with zero refused
allocations. The 32 KiB profile is a pathological full-set sweep, record-only.

## 8. Firmware build method (Tier F)

| Item | Value |
| :-- | :-- |
| Environment | `x4pro` (`esp32-s3-devkitc1-n16r8`, ESP32-S3) |
| App partition | `app0` = `0x640000` = **6,553,600 B** (`partitions.csv`) |
| Command | `pio run -e x4pro` (size report only; no upload/flash/monitor) |
| Baseline tree | main worktree `xteink-os` @ `cc31d678`, unmodified |
| Experiment tree | isolated worktree `xteink-os-wp1b`, **detached HEAD** @ `cc31d678` |

**Why a separate worktree:** the Strategy B experiment adds real firmware
sources. It was isolated so the branch working tree stays clean. No branch was
created and nothing was committed.

**Experiment contents (Strategy B, minimal registration):**

1. The six committed headers copied from
   `test/vietnamese_font_coverage/generated/` into `lib/EpdFont/builtinFonts/`
   (the include directory already used by `builtinFonts/all.h`).
2. `src/main.cpp`: six includes, six `EpdFont`/`EpdFontFamily` pairs, and six
   `renderer.insertFont()` calls next to the existing built-in registrations
   (`main.cpp:547-555`). No `all.h` edit, no `fontIds.h` edit, no
   `platformio.ini` edit, no SDK edit.

**Two implementation notes, recorded because they affect reproducibility:**

- *Font IDs.* The committed `src/fontIds.h` cannot be reproduced from the
  current font headers: neither raw bytes nor CRLF→LF normalized content
  reproduces the committed `NOTOSANS_12_FONT_ID`, so that generated file is
  stale relative to `lib/EpdFont/builtinFonts/*.h`. The experiment therefore
  used IDs computed with the same formula from
  `lib/EpdFont/scripts/build-font-ids.sh`, guarded by a
  `static_assert(... != 0)`. IDs are registry keys only; they do not affect
  code size.
- *SdFat patch.* `scripts/patch_sdfat.py` rejects a **fresh** SdFat install in
  this environment (`RuntimeError: Unexpected patched SdFat source:
  FsCache.cpp`), because a 2488 B patched file versus a 2490 B freshly resolved
  file fails its digest check — a pre-existing line-ending sensitivity,
  unrelated to fonts. To keep the two builds comparable, the already-patched
  SdFat sources were copied into the experiment worktree and verified
  byte-identical by SHA-256 (`F46A551F…`). This changes no library version.

## 9. Firmware measurements

| Build | Flash used | of 6,553,600 B | RAM used | of 327,680 B | Build time |
| :-- | --: | --: | --: | --: | --: |
| **Baseline** (main worktree) | **5,397,378** | **82.4 %** | **111,740** | **34.1 %** | 149.5 s |
| **Baseline** (experiment worktree, same path) | **5,397,378** | 82.4 % | **111,740** | 34.1 % | 133.9 s |
| **Strategy B** (+6 Serif Regular) | **5,826,518** | **88.9 %** | **111,884** | **34.1 %** | 361.6 s / 90.8 s rebuild |

```text
Baseline  : RAM: 34.1% (used 111740 bytes from 327680 bytes)
            Flash: 82.4% (used 5397378 bytes from 6553600 bytes)
Strategy B: RAM: 34.1% (used 111884 bytes from 327680 bytes)
            Flash: 88.9% (used 5826518 bytes from 6553600 bytes)
```

**Delta (the WP1 F2 result):**

| Metric | Baseline | Strategy B | **Delta** |
| :-- | --: | --: | --: |
| Flash | 5,397,378 | 5,826,518 | **+429,140 B (419.1 KiB)** |
| RAM | 111,740 | 111,884 | **+144 B** |

Two controls support these numbers:

1. **Path independence.** Rebuilding the baseline inside the experiment
   worktree produced byte-identical totals (5,397,378 / 111,740), so the
   directory difference between the two builds contributes nothing.
2. **Determinism.** Strategy B was linked twice and reported the same totals
   both times.

**Symbol-level attribution** (`xtensa-esp32s3-elf-nm` on the Strategy B ELF, 90
serif symbols):

| Face | `.rodata` bytes |
| :-- | --: |
| `notoserif_12_regular` | 46,533 |
| `notoserif_13_regular` | 50,346 |
| `notoserif_15_regular` | 54,892 |
| `notoserif_17_regular` | 60,915 |
| `notoserif_30_regular` | 100,678 |
| `notoserif_34_regular` | 115,362 |
| **Sum of font tables** | **428,726** |

The remaining 414 B of the 429,140 B flash delta is linker alignment and
padding. The +144 B RAM delta matches the 12 registered `EpdFont` /
`EpdFontFamily` wrapper objects created in `setup()`.

Cross-check of the three independent figures: F1 `sizeof` estimate **429,302 B**,
symbol sum **428,726 B**, measured flash delta **429,140 B** — spread under
0.04 %. The `sizeof` estimate is a slight over-count because it adds a struct
that the linker places in `.data` (RAM), not flash.

## 10. Headroom for WP2–WP6

Derived from §9 against the fixed 6,553,600 B `app0` partition:

| Scenario | Flash free | Free share |
| :-- | --: | --: |
| Baseline (no bundled serif) | **1,156,222 B** (1.10 MiB) | 17.6 % |
| Strategy B adopted | **727,082 B** (710 KiB) | 11.1 % |

Strategy B consumes **429,140 B**, i.e. **37.1 %** of the baseline headroom.
Static RAM headroom is effectively unchanged (215,940 B → 215,796 B free).

**No D11 budget is assumed here.** Whether 727,082 B is sufficient for
WP2–WP6 is a Product Owner decision; this document deliberately sets no
threshold.

## 11. Comparison with the previously reported figures

| Figure | Previously reported | Measured here | Difference |
| :-- | --: | --: | --: |
| Baseline flash | 5,397,410 | **5,397,378** | −32 B (−0.0006 %) |
| +6 Serif Regular flash | 5,826,550 | **5,826,518** | −32 B |
| **Delta** | **+429,140** | **+429,140** | **0 B — exact** |
| App partition | 6,553,600 | 6,553,600 | 0 |

The delta reproduces **exactly**. The constant 32 B offset on both absolute
totals is not explained by build path (§9 control 1) and most likely reflects a
small toolchain or source difference between the reporting machine and this
one; it cancels in the delta. Treat the absolute totals as ±32 B and the delta
as exact.

## 12. Previously reported / historical figures (NOT remeasured)

The only other firmware size record on this machine is a **pre-WP1** artifact
outside version control, `build/xos-f2d-x4pro-utf8.log` (2026-11-01 20:09,
F2D serif-specimen experiment, not this branch):

| Metric | Value | Label |
| :-- | :-- | :-- |
| Flash | 5,807,378 B of 6,553,600 B (88.6 %) | previously reported, not remeasured |
| RAM | 111,788 B of 327,680 B (34.1 %) | previously reported, not remeasured |

It is a baseline-only build from a different tree and is **not** comparable to
the Strategy B measurement above.

## 13. What was measured, and what was not

Measured (trust): host glyph coverage/rasterization/arena; real firmware flash
and static RAM for baseline and Strategy B on `x4pro`.

Not measured (do not infer):

| Gap | Status |
| :-- | :-- |
| Panel behaviour: page-render latency, ghosting, refresh | **Not measured** — Tier W, hardware only (VE-9) |
| On-device PSRAM glyph-arena behaviour | **Not measured** — §7 numbers are x86-64 host-derived |
| Startup cost of loading a vector face (§4.2 item 6) | **Not measured** — no timed test exists in the suite |
| Runtime fallback chain (`SdCardFontSystem.h:66-84`) | **Not exercised** — only the built-in union was proven |
| Bold/italic serif faces | **Not measured** — Strategy B here is 6 *regular* faces only |
| Per-size printed missing-glyph table | Evidence is assertion-based (0/505 at each size) |
| Other test suites | Cannot configure on this host (`libexpat1-dev` absent) |
| D11 flash headroom threshold | **Not set by the PO** — no budget invented here |

Strategy B as measured is a **lower bound** for a full typographic system: a
shipping UI serif would likely also need bold (and possibly italic) faces at
several sizes, each adding flash.

## 14. D9 — Product Owner decision (2026-11-03)

**This section is a decision record, not a measurement.** The measurements it
rests on are §4–§11.

### 14.1 Accepted direction

WP1 engineering feasibility is accepted. The font distribution direction is:

| Track | Decision | Consequence |
| :-- | :-- | :-- |
| **A** | Built-in NotoSans stays the **mandatory** fallback floor | No change: registrations at `src/main.cpp:547-555` stay exactly as they are; the 505-codepoint floor already holds (§5 `FallbackFloor`) |
| **C** | Reader Serif remains the **existing SD-loaded TTF** path | Already implemented (`src/SdCardFontSystem.h:43,152`); no firmware work in M2 |
| **B** | A **minimal bundled UI Serif subset** is evaluated during WP2, prioritizing the approved 30/34 px editorial headings | B stops being "all six sizes" and becomes a per-face decision inside WP2 |

**Explicitly rejected:** permanently bundling all six Serif sizes. That is the
configuration measured in §9 (**+429,140 B** flash).

**D11 (flash reserve) remains provisional.** The numeric floor is deferred until
native Home size measurements exist. This document still sets no threshold, and
§10's 727,082 B figure is *not* adopted as one.

### 14.2 What the measurements imply for a minimal subset

Per-face flash is measured (§9, `nm` on the linked Strategy B ELF). Subset costs
below are **arithmetic on those measurements, not linked builds**:

| Subset | Role it serves (M1 §1.3) | Symbol `.rodata` | Share of baseline headroom |
| :-- | :-- | --: | --: |
| `{34}` | `type.display` (hero titles) | 115,362 B | 10.0 % |
| `{30}` | `type.screenTitle` (Library/Files/Settings headers) | 100,678 B | 8.7 % |
| `{30, 34}` | both editorial headline roles | 216,040 B | 18.7 % |

If `{30, 34}` were adopted, the derived headroom would be
1,156,222 − 216,040 ≈ **940,182 B** — but see the three cautions below before
treating that as a plan.

Cautions carried with those numbers:

1. **They are symbol sums, not a linked delta.** For the six-face bundle the
   symbol sum ran 414 B (0.1 %) below the linked delta (§9), so treat subset
   figures as ±0.1 % and re-measure on link.
2. **Weight is unmeasured.** Every measured face is Regular. M1 §1.3 specifies
   SemiBold (600) for both `type.display` and `type.screenTitle`. The font
   pipeline has only bold (700) variants, and `EpdFontFamily::getFont`
   **silently falls back to Regular** when no bold face is registered
   (`lib/EpdFont/EpdFontFamily.cpp:3-19`) — so a regular-only `{30, 34}` subset
   would render the approved display weight incorrectly rather than fail
   visibly. A bold face costs a further per-face amount of the same order as
   the regular face (measured faces span 46.5–115.4 KB, §9).
3. **Only two serif roles exist.** `type.section`/`body`/`secondary`/`micro`
   (17/15/13/12 px) are UI Sans in the same M1 table, so they need no serif
   faces at all. This is why a 2-face subset is sufficient for the M1 scale,
   and why the six-face Strategy B measurement was over-provisioned.

### 14.3 Open at WP1 close (carried into WP2)

| # | Open item | Owner |
| :-- | :-- | :-- |
| 1 | Which serif sizes the bundle keeps: `{34}` alone, or `{30, 34}` | PO, during WP2 |
| 2 | Whether Regular weight is acceptable for a SemiBold-specified role | PO, during WP2 |
| 3 | D11 numeric flash floor | PO, after native Home size measurement |
| 4 | Tier W: startup and page-render behaviour of any bundle | WP7/M6 hardware gate |
| 5 | `src/fontIds.h` regeneration (§15) must work before a face is bundled | Engineering |

### 14.4 Superseded agent recommendation (kept for the record)

Before the decision, this document recommended "B for UI typography + C for the
reader serif, with A as the fallback floor, conditional on accepting ≥727,082 B
remaining as the D11 floor; otherwise C-only + A with the B headers shelved",
plus a warning that bold/italic faces were unmeasured. The PO accepted A, C and
a *minimal* B, and deferred D11 — so the headline recommendation is superseded,
while the bold/italic caveat survives as §14.2 caution 2 and §14.3 item 2.

## 15. Build reproducibility risks carried into WP2

Two pre-existing issues were hit during Tier F. Both are recorded in §8; they
are repeated here because WP2 will bundle a font face and therefore depends on
them.

| Risk | Observed effect | State |
| :-- | :-- | :-- |
| **Stale `src/fontIds.h`** | The committed `NOTOSANS_12_FONT_ID` cannot be reproduced from the committed headers by `lib/EpdFont/scripts/build-font-ids.sh` — neither raw bytes nor CRLF→LF normalization matched, so the generated file is stale relative to `lib/EpdFont/builtinFonts/*.h`. Any newly bundled face needs an ID that cannot currently be regenerated deterministically. | Open, engineering |
| **SdFat patch line-ending sensitivity** | `scripts/patch_sdfat.py` rejected a *freshly resolved* SdFat in the isolated worktree (`RuntimeError: Unexpected patched SdFat source: FsCache.cpp`, 2488 B patched vs 2490 B fresh). The main worktree only built because its `.pio` already held patched sources. Unrelated to fonts. | Open, pre-existing |
| Whole-tree build cost | Baseline 149.5 s, Strategy B 361.6 s (§9) — relevant to CI budget, not to correctness. | Informational |

Neither open risk changes any number in §9–§11, and both predate this branch.

## 16. Change footprint

- Main worktree `feat/m2-wp1-font-feasibility` @ `cc31d678`:
  - new untracked files: `docs/design/specs/M2-WP1-RESULTS.md` (this
    document) and `docs/design/specs/M2-WP2-HOME-PLAN.md` (WP2 plan, no
    implementation)
  - pre-existing untracked items untouched: `.serena/`,
    `xteink-os-design-002-review.zip`
  - **no tracked file modified**; `git diff --check` clean
- Isolated experiment worktree `xteink-os-wp1b` (detached HEAD, never
  committed, preserved as evidence): `M src/main.cpp` (+42 lines) and six
  untracked `lib/EpdFont/builtinFonts/notoserif_*_regular.h`
- Untracked, gitignored build artifacts: `build/wp1-host/`, `build/test/`,
  `build/tierf-*.log`
- No production firmware, SDK, `platformio.ini`, partition table, or test
  source was changed; nothing was flashed, uploaded, committed, pushed, or
  merged.

## 17. WP1 closeout record

| Item | State |
| :-- | :-- |
| Tier H (host tests) | Complete — 10/10 pass, 0.23 s (§4) |
| Tier F (firmware size) | Complete — baseline and Strategy B linked and measured (§9) |
| Tier W (hardware) | **Not performed** — deferred to WP7/M6 (§13) |
| F2 delta reproduces report | Yes — +429,140 B flash, exact (§11) |
| D9 | **Decided** — direction in §14.1; subset and D11 open (§14.3) |
| Firmware merged | No — no tracked file changed, nothing committed or flashed |
| Evidence retained | Yes — `xteink-os-wp1b` worktree preserved |

WP1 is closed on evidence, not on an implementation. The next work package
(FR-01 Home) is planned in [M2-WP2-HOME-PLAN.md](M2-WP2-HOME-PLAN.md) and is
**not** authorized by this document.

# Atelier Glyph Fallback Implementation Plan

**Goal:** Preserve supported non-Latin text in X4 Pro Atelier UI without regressing rendering or resource limits.

**Architecture:** First reproduce the direct-draw failure and verify actual fallback-font coverage. Choose a shared rendering and measurement path only after confirming font capabilities, grayscale behavior and the project authorization gates.

**Tech stack:** Embedded C++, FreeInk/GfxRenderer, host CTest, PlatformIO, X4 Pro simulator and hardware.

**Authority:** `HANDOFF_CLAUDE_CODE.md` §4; `docs/EXECUTION_CONTRACT.md`; `docs/design/specs/STEP-09-NATIVE-INTEGRATION-PLAN.md`.

## Constraints

- Keep IRAM usage within its existing budget; no new `IRAM_ATTR` functions.
- No heap allocation in steady-state render passes or row providers.
- Preserve three grayscale planes, multi-board compatibility and existing translations.
- Do not claim a script is supported unless the actual fallback font and shaping path support it.
- Preserve dirty files; no commit, push, flash or SDK upgrade without separate approval.

## Gate 1: Authorization and baseline

- [ ] Reconcile this experiment branch with STEP 09 G1–G4 and obtain the named package authorization before firmware edits. Approval of this plan alone does not close project gates.
- [ ] Record branch, HEAD, dirty files, SDK gitlink and actual checkout. Do not treat prior handoff results as current test results.
- [ ] Confirm available build/simulator environment and preserve existing generated files and user changes.

## Gate 2: Reproduce and select an approach

- [ ] Trace `AtelierChrome::drawText`, `measureText`, `wrapText`, ellipsis and `GfxRenderer` text hooks in `src/components/AtelierChrome.cpp` and their relevant callers.
- [ ] Reproduce a direct-draw missing glyph with a known available fallback glyph, plus ASCII and Vietnamese NFC/NFD controls. Record actual pixels or a simulator screenshot, not only source assertions.
- [ ] Inspect fallback font glyph coverage, shaping and grayscale-plane behavior. Identify whether measurement, wrapping, truncation, alignment and rendering can share the same font choice without recursion or heap allocation.
- [ ] If no suitable fallback can meet those requirements, stop and report the limitation instead of promising universal CJK, Arabic or Cyrillic coverage.

## Gate 3: Narrow fix and verification

- [ ] Add a focused regression check using the existing host/simulator test infrastructure; demonstrate failure before the fix. Cover mixed text, fallback width, alignment, truncation and NFC/NFD controls as applicable to the chosen path.
- [ ] Implement the smallest shared fix in the existing renderer/caller path, consistent with the measured fallback capabilities and `#if FREEINK_DEVICE_X4PRO` plus portrait runtime guards where applicable. Avoid unrelated refactoring.
- [ ] Run the focused checks and relevant host tests; build `simulator`, `simulator_x4_pro`, and `x4pro` where available. Record commands, results, IRAM/flash changes and any unavailable checks.
- [ ] Verify screenshots in the X4 Pro simulator with WSLg for supported mixed-script strings, narrow widths and each affected screen; inspect text clipping and grayscale passes.
- [ ] Complete the mandatory `firmware-handoff` process, including four independent read-only review axes and human acceptance. Delegate independent reviews to Sonnet/Haiku when agent execution is available; the main agent owns edits, builds and tests.
- [ ] Request human device verification for physical contrast, ghosting and controls on the exact firmware build; never flash without explicit authorization.

## Current blockers

- `docs/PROJECT_STATUS.md` records native integration blocked by G1–G4; the relationship of this experimental fix to the authorized package must be resolved before edits.
- Agent launches were rejected due to a worktree/core.worktree redirect; do not alter Git configuration or delete worktrees without inspecting and obtaining permission. If unavailable, report the required independent reviews as incomplete rather than claiming completion.

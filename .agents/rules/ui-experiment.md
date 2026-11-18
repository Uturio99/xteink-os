---
trigger: always_on
description: Governance, isolation boundaries, model routing, and verification rules for the Antigravity CrossPoint UI Engineering Experiment in xteink-os-antigravity-ui.
---

# Antigravity CrossPoint UI Experiment Governance

## 1. Isolation & Dependency Authority
- Worktree: `G:/AI-Setup/XTEInk-X4-Pro/xteink-os-antigravity-ui` (`experiment/antigravity-crosspoint-ui`, branched from `ff7a0e02a16c1855d533e8f52abbc9f907a8077c`).
- Do not modify `xteink-os` or any NoveOS worktree. Keep `freeink-sdk` pinned at `946e6ac3d7a9d590ba3224428e6183d55ac7917c`.
- Upstream CrossPoint rules in `AGENTS.md`, `.agents/rules/*.md`, and safety contracts in `docs/EXECUTION_CONTRACT.md` remain binding.

## 2. Gate Discipline & Architectural Freedom
- Do not edit firmware C++ headers or sources before PO approves the visual direction and scope at Gate G1.
- Prefer existing `FreeInkUI` (`fui::*`) components, `UiAppHost`, `UiListActivity`, `UiTabListActivity`, `UITheme`, and `ThemeMetrics`, but allow justified, reversible extensions (e.g., dedicated presenter/layout adapters) when needed for next-generation UI/UX.
- All geometry must derive from `UITheme::getContentArea(renderer)` / `getOrientedViewableTRBL()` and measured font line heights, using two-anchor clamped bounds so elements never overlap across font scales or orientations.
- Steady-state `renderUi()` and `provideRow()` must remain allocation-free.

## 3. Model Delegation & Retry Budget
- **Orchestrator (Claude, Plan-Only):** Uses builder-provided codebase evidence to write detailed, exact plans (target files + line anchors, exact code/values, exact commands, expected outputs, acceptance criteria, ≤20-line return contract), performs gate reviews, and verifies returned PNGs/metrics. The orchestrator does not search the codebase, edit source, build, run tests, or capture simulator shots. Opus may act separately as the exceptional writer under the escalation below. One delegation message per plan step; no re-planning unless a builder fails twice.
- **Builder (Claude subagent, routine writer):** Searches the codebase, reports evidence for planning, and executes plan-specified code edits, WSL mirror sync, CTest (`ctest`), `pio run` (`simulator_x4_pro` & `x4pro`), SD fixtures, headless simulator captures, BMP→PNG conversion, and filtered `git status`. Verifies Opus repairs. Max 2 fix attempts per hypothesis, then escalate to Orchestrator with logs.
- Reuse an existing live builder via `send_message` when continuing the same work; spawn new only for new scope.
- **Locked UI style (PO-approved 2026-11-17):** Concept 01 *Atelier Typographic* + `AtelierSelection.h` row selection (`5px` Ink bar, `16px` gap, `+21px` indent, bold Ink index when selected). Do not alter without PO approval.
- **Active Token-Optimization Skills:** Enforce `caveman` (full terse style), `cavecrew` (≤20-line subagent return contracts), `rtk` (source-filtered CLI/build logs), and `ponytail` (minimum diff, zero redundant re-runs).
- **Reviewer (Sonnet, independent and read-only):** Inspects diffs and logs only; never edits, builds, or tests. Use 1 lightweight spec/quality reviewer during G2-G3, and full 4-axis `firmware-handoff` at G4.
- **Opus (exceptional writer):** May review and repair when a review finds too many errors. Only one writer may edit at a time; the builder pauses edits while Opus repairs, then verifies the repairs. All existing safety, technical verification, and review gates still apply.
- **No AI trailer:** Never add AI `Co-Authored-By` trailers.

## 4. Six Independent Verification Layers
- Report separately: `HOST_TEST_VERIFIED`, `SIMULATOR_BUILD_VERIFIED`, `SIMULATOR_RENDER_VERIFIED`, `SIMULATOR_INTERACTION_VERIFIED`, `FIRMWARE_BUILD_VERIFIED`, and `HARDWARE_VERIFIED` (human-owned). Never infer one layer from another.

## 5. Ponytail E-Ink UX & Anti-Ghosting Protocol
- **Single-pass home entry:** Pre-seed `noteThumbHeight(...)` during `begin()` (before Pass 1) so opening Home never triggers extra cover-reload / full-screen re-render passes.
- **Zero Bayer dither in UI chrome:** Never paint `Color::LightGray` / `Paint::dither(...)` backgrounds, tracks, or selection fills in UI chrome; use solid `1px` rules (`Black` on `White`), solid capsule CTAs, and `outlinedButtonStyles(..., fui::Color::White)` to prevent checkerboard ghosting under partial refresh.
- **Low-pixel-flip focus indicators:** Mark focused cards/rows with a `2px` crisp focus stroke or the `AtelierSelection.h` `5px` solid left bar + `+21px` indent rather than inverting large cover/row areas.
- **Dynamic Vietnamese diacritic clearance:** Size title/body slots from runtime `lineHeight(...)` metrics (never hardcoded tight heights) with explicit `>= 0` inter-slot gaps so stacked NFC/NFD Vietnamese glyphs remain unclipped and overlap-free across font scales.

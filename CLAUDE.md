# CLAUDE.md — NoveOS / XTEInk X4 Pro (`xteink-os-antigravity-ui`)

This directory is a standalone Git repository (`experiment/antigravity-crosspoint-ui`) for **NoveOS** on **XTEInk X4 Pro** (CrossPoint/FreeInk firmware).

## Mandatory Reading on Startup

1. **[HANDOFF_CLAUDE_CODE.md](HANDOFF_CLAUDE_CODE.md)** — Current verified state, Atelier Typographic UI architecture, build/test/simulator workflows, critical memory/E-Ink guardrails, and next tasks (features, interactions, display optimizations).
2. **[AGENTS.md](AGENTS.md)** — Repository embedded C++ rules, FreeInk SDK boundary (`freeink-sdk` pinned at `946e6ac3`), and safety rules under `.agents/rules/`.

## Non-Negotiable Constraints

- **IRAM is at 100.0% (`16,384 / 16,384 B`) on `env:x4pro`:** Never add new `IRAM_ATTR` functions or move large code into IRAM.
- **No heap allocations in steady-state render loops:** Code executed inside `AtelierChrome::renderGrayscalePass` (`paintPass` runs 3 times: BW, LSB, MSB), `provideRow()`, or `renderUi()` must use fixed stack buffers / preallocated storage.
- **No Bayer dither in X4 Pro chrome:** Never use `Color::LightGray` or `Paint::dither(...)` in Atelier chrome (`#if FREEINK_DEVICE_X4PRO && AtelierChrome::isPortrait480x800(renderer)`). Use the 4-level grayscale shades (`0..3`) in `AtelierChrome`.
- **Keep multi-board compatibility:** Guard X4 Pro `480x800` portrait logic with `#if FREEINK_DEVICE_X4PRO` + runtime `AtelierChrome::isPortrait480x800(renderer)` so non-X4 Pro builds (`env:simulator`) keep working cleanly.
- **Never edit auto-generated files directly:** Edit `lib/I18n/translations/*.yaml` (not `I18nKeys.h` / `I18nStrings.*`) and regenerate with `python3 scripts/gen_i18n.py lib/I18n/translations lib/I18n/`.
- **System language vs. content glyphs:** All system UI chrome must use `tr(STR_...)` (`I18N`), never hardcoded Vietnamese or forced language overrides. Keep Vietnamese/CJK book & font glyphs intact for reader content and book metadata.
- **Simulator requires WSLg display:** `HalDisplay::begin()` requires an accelerated SDL renderer; do NOT set `SDL_VIDEODRIVER=dummy` when capturing screenshots (`DISPLAY=:0`, `WAYLAND_DISPLAY=wayland-0`, `XDG_RUNTIME_DIR=/mnt/wslg/runtime-dir`).

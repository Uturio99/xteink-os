# XTEInk OS — M2 WP2 Plan: FR-01 Home

> **Document ID:** M2-WP2-HOME-PLAN
> **Status:** PLAN ONLY. No implementation is authorized by this document.
> **Repository:** `xteink-os`, branch `feat/m2-wp1-font-feasibility` @ `cc31d678`
> **Date:** 2026-11-03
> **Design authority:** [UNIFIED_UI_COLLECTION.md](UNIFIED_UI_COLLECTION.md) (M1, frozen) + [M2-ADDENDUM.md](M2-ADDENDUM.md) §3
> **Inputs:** [M2-WP1-RESULTS.md](M2-WP1-RESULTS.md) §9–§14; `docs/design/BASELINE.md` §3
> **Scope:** FR-01 (Populated Home) only. FR-02 and FR-03 are **not** in this plan.

---

## 1. What bounds the work

| Constraint | Source |
| :-- | :-- |
| No competing UI framework; extend the existing activity/`fui::` stack | M2-ADDENDUM §3 |
| SDK is a pinned submodule (`946e6ac`) — no SDK edits, no dependency changes | `AGENTS.md`; M2-ADDENDUM §4.5 |
| X4 Pro path is the target; other boards must still compile and behave as today | `CoverGridHomeUi.cpp:122-127`, `202-207`; AGENTS.md C3 baseline |
| D11 flash reserve is **provisional** — WP2 must not assume a number | M2-WP1-RESULTS §14.1 |
| D9: A (built-in NotoSans) is the mandatory floor; C (SD TTF) is the reader serif; B is a *minimal* subset evaluated in WP2 | M2-WP1-RESULTS §14.1 |

## 2. Current state (evidence)

| Element | Where it lives now |
| :-- | :-- |
| Home activity, data load, cover prefetch | `src/activities/home/HomeActivity.cpp:233-265` (`onEnter`), `:483-520` (`render`) |
| X4 Pro screen composition | `src/components/CoverGridHomeUi.cpp:196-355` (`drawX4ProPortrait`) |
| Geometry, pure and constexpr | `src/components/X4ProHomeLayout.h:43-86` (`makeX4ProHomeLayout`) |
| Visible-count and CTA action rules | `src/components/X4ProHomeLayout.h:37-41` |
| Host test for the geometry | `test/x4pro_home_layout/X4ProHomeLayoutTest.cpp:5-33` |
| Font slots for a frame | `freeink-sdk/.../FreeInkUIGfxRenderer.h:35-42` (4 slots: SMALL/BODY/TITLE/LABEL) |
| Which font each slot gets | `src/components/UIScale.h:14-23` (UI_10 / UI_12 / UI_12) |
| Row/header/touch metrics derive from the **body** line height | `freeink-sdk/.../src/FreeInkUI.cpp:119-129` |
| Branded type scale (34/30 serif, 17/15/13/12 sans) | M1 §1.3 (`UNIFIED_UI_COLLECTION.md:33-43`) |
| X4 Pro always uses this home | `src/components/UITheme.cpp:33-41` |

The Home screen today is a **centered hero cover** (76×114) above a centered
3-line title, with a 3-column 125×188 cover shelf and a 5-icon bottom tab bar.
The M1 type scale is not implemented anywhere in the UI: all chrome text runs
at 8/10/12 px.

## 3. FR-01 gap analysis

| Element | M1 FR-01 (`:76-87`) | Current | Gap |
| :-- | :-- | :-- | :-- |
| Jacket | 108×154, right column | 76×114, centered above title (`X4ProHomeLayout.h:48-49,68`) | size + position |
| Title | 34/42 serif, left column 270 px, ≤3 lines | 12 px Ubuntu bold, centered (`CoverGridHomeUi.cpp:229-235`) | family/size/column |
| Author | 15/22 sans, under title | smallText, centered (`:237-243`) | size/alignment |
| Milestone | `Chương 4 · Trang 42/280`, 13/19 | absent — only a `%` label (`:245-251`) | missing element (needs chapter data) |
| Progress | `15 %` + indicator, in the hero band | `%` + 2 px track below author (`:245-261`) | placement |
| CTA | 56 px black pill | 44 px (`X4ProHomeLayout.h:73`) | height; 44 is also under the 48 px touch minimum in M1 §1.5 |
| Recent shelf | 2 one-column rows, 40×56 thumb + title + author + % | 3-column 125×188 grid, titles only (`:284-351`) | structure |
| Navigation | "Zero persistent bottom 4-tab dock" | 5-icon bottom tab bar (`:353`, `:492-522`) | conflicts with M1 (see §9 R4) |

The milestone string is the only FR-01 element with no data source yet:
`ChapterPosition.h` exposes chapter-local page and fraction, but Home does not
read it (M2-ADDENDUM C7 already fixes the display format for it).

## 4. Required component changes

Bounded, in dependency order. None of these is proposed for this session.

| # | Change | Files | Notes |
| :-- | :-- | :-- | :-- |
| 1 | Hero band: left text column + right jacket, CTA 56 px | `X4ProHomeLayout.h`, `CoverGridHomeUi.cpp` (hero block only) | Pure geometry, host-testable |
| 2 | Recent shelf: 2 rows of 40×56 thumb + title + author + % | `X4ProHomeLayout.h`, `CoverGridHomeUi.cpp` (shelf block) | Reuse `fui::bookCard` (`book-card.h:14-48`, `coverSize{40,56}`) — no new component |
| 3 | Navigation band per M1 | `CoverGridHomeUi.cpp:353`, `:492-522` | Product decision first (§9 R4) |
| 4 | Milestone line | `HomeActivity.cpp` (data), `CoverGridHomeUi.cpp` (paint) | Needs chapter/page wiring; largest unknown |
| 5 | Typography (serif 34 on the hero) | `lib/EpdFont/builtinFonts/`, `src/main.cpp`, `CoverGridHomeUi.cpp` | Gated on the PO's subset decision (§5) |

## 5. Minimal typography integration

A frame has **four font slots** (`FreeInkUIGfxRenderer.h:35-42`), each bound to
one GfxRenderer font id, and all row/header/touch metrics derive from the
**body** slot's line height (`FreeInkUI.cpp:119-129`). That is the hard budget:

| Slot | Today | FR-01 needs | Proposal |
| :-- | :-- | :-- | :-- |
| TITLE | UI_12 (12 px) | 34 px serif display | bind to a new serif-34 face **in a hero-local frame** |
| BODY | UI_12 | 15 px sans (author) | NotoSans 16 (nearest built-in; already registered, `main.cpp:551`) |
| SMALL | UI_10 | 13 px sans (milestone) | NotoSans 12 (`main.cpp:549`) or UI_12 |
| LABEL | UI_10 | chrome only | leave at SMALL — header/status text is deliberately fixed (`BaseTheme.cpp:423-424`) |

Two consequences follow, and both are design-relevant:

1. **Per-frame binding, not global.** Rebinding BODY globally would inflate
   `rowHeight`, `headerHeight`, `minTouchSize` and `spaceSm` on **every** screen
   (`FreeInkUI.cpp:123-127`). The established pattern for a locally-scaled band
   is a dedicated `GfxRendererFrame` with its own `setFont()` calls
   (`BaseTheme.cpp:411-412`). FR-01 should do the same for the hero band only.
2. **Serif 34 is the whole font dependency.** M1 §1.3 assigns serif only to
   `type.display` (34) and `type.screenTitle` (30); every other role is sans.
   The cheapest FR-01-visible subset is therefore `{34}` at **115,362 B** of
   `.rodata` (M2-WP1-RESULTS §14.2) — 10 % of the current baseline headroom.

Weight caveat: M1 asks for SemiBold (600) and the pipeline has only 700, and
`EpdFontFamily::getFont` **silently substitutes Regular** when a bold face is
absent (`lib/EpdFont/EpdFontFamily.cpp:3-19`). A regular-only serif 34 will
render the hero title lighter than specified without any error. The PO must
choose regular-only or regular+bold before the face is generated.

## 6. Host test coverage

Extend the existing target — no new test directory is needed:

- `test/x4pro_home_layout/X4ProHomeLayoutTest.cpp` — assert the new rect set:
  jacket `{340, 88, 108, 154}`, text column `{32, *, 270, *}`, CTA height 56,
  and the relational invariants below.
- Relational assertions (the ones that actually catch layout regressions):
  title/author/progress share `x` and width; text column right edge ≤ jacket
  left edge; every rect inside `content` `{32, 52, 416, 720}`; the shelf block
  still ends above the tab bar; CTA ≥ 48 px (M1 §1.5).
- Existing assertions at `:8-14` and `:21-25` will need updating — they pin the
  current 76×114/44 px geometry, so the diff is expected and reviewable.
- `x4ProVisibleBookCount` / `x4ProHeroCtaAction` tests (`:28-33`) must keep
  passing: the storage cap (`MAX_BOOKS = 7`) is unchanged by a layout change.

Note: the committed `cmake -S test` command is still blocked by missing
`libexpat1-dev` on this host (M2-WP1-RESULTS §3.1–§3.2); the documented
scaffold is the workaround until that is resolved.

## 7. Firmware build checks

| Check | Command | Expected |
| :-- | :-- | :-- |
| X4 Pro size unchanged by a layout-only task | `pio run -e x4pro` | flash/RAM identical to baseline (5,397,378 / 111,740) — a *check*, not a D11 measurement |
| Other boards still compile | `pio run -e x4c` | compiles; X4 Pro code stays behind `#if FREEINK_DEVICE_X4PRO` |
| Serif subset cost, when added | `pio run -e x4pro` vs baseline | new delta recorded so the PO can finally set D11 |
| Whitespace | `git diff --check` | clean |

## 8. Hardware validation requirements (human-owned, WP7 gate)

None of these can be produced by host tests or by reading this plan:

- Jacket 108×154: cover thumbnails must be **regenerated** at the drawn height,
  or dithered 1-bit art aliases (`CoverGridHomeUi.h:28-30`; the regeneration
  path is `HomeActivity.cpp:506-516`).
- 3-line Vietnamese title at 34 px: diacritic clearance and no clipping at the
  hero band's bottom edge.
- CTA 56 px and jacket tap targets: minimum 48×48 px (M1 §1.5).
- Ghosting and refresh behaviour of the larger jacket on the panel.
- Startup cost of one extra bundled bitmap face (unmeasured in WP1, §13).

## 9. Risks

| # | Risk | Mitigation |
| :-- | :-- | :-- |
| R1 | `src/fontIds.h` cannot be regenerated deterministically (M2-WP1-RESULTS §15) | Fix or document the ID path before generating the serif face |
| R2 | Bold-less serif silently renders Regular | PO decision on weight before generation; assert the family wiring in a host test |
| R3 | Rebinding fonts globally inflates every screen's metrics | Hero-local frame only; no change to `UIScale.h` |
| R4 | M1 says "zero persistent bottom tab dock"; the current home's only navigation is that dock | Product decision needed before WP2 removes it — removing it without replacement would strand Library/Files/Settings |
| R5 | Cover cache holds one thumb per visible book at one height | Keep the shared-height rule (`CoverGridHomeUi.cpp:67-95`) when the jacket and shelf sizes diverge |

## 10. Proposed first task — WP2.1 (bounded)

**FR-01 hero band recomposition. Layout only, no font changes, no new faces.**

| Item | Value |
| :-- | :-- |
| Files changed | `src/components/X4ProHomeLayout.h`, `src/components/CoverGridHomeUi.cpp` (hero block, `:216-282`), `test/x4pro_home_layout/X4ProHomeLayoutTest.cpp` |
| Explicitly untouched | `src/main.cpp`, `lib/EpdFont/**`, `src/components/UIScale.h`, `freeink-sdk/**`, `HomeActivity.cpp`, the shelf band, the tab bar, `platformio.ini` |
| Expected diff | ~90–130 lines, one new layout contract, rewritten geometry test |
| Flash delta | 0 B expected (no new const data) |

Acceptance criteria:

1. Jacket rect is 108×154 and right-aligned inside the content box (right edge
   at x = 448).
2. Title, author, and progress sit in a 270 px left column starting at x = 32;
   the column never overlaps the jacket.
3. The title box holds exactly 3 lines at the live title line height.
4. The CTA is 56 px tall and centered; it remains a single solid black fill
   (M1 §1.4 "single CTA rule").
5. Every rect stays inside `content` `{32, 52, 416, 720}` and above the tab bar.
6. `pio run -e x4pro` reports the baseline size unchanged; `pio run -e x4c`
   still compiles.
7. Human confirms on the panel that no text clips and touch targets respond.

Deliberately deferred to later WP2 steps: shelf restructure (WP2.2), tab-bar
question (WP2.3, needs R4), milestone line (WP2.4, needs chapter data),
serif typography (WP2.5, needs the PO subset decision).

## 11. What this plan does not do

- No source file is modified by this document.
- FR-02 (empty home) and FR-03 (curated home) are not analyzed here.
- No D11 number is proposed; §7 only says WP2 will produce the measurement.
- No hardware claim is made — every panel statement in §8 is a requirement on
  a future human test, not a result.

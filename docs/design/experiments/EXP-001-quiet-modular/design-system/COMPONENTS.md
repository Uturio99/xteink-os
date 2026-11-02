# COMPONENT CONTRACT — XOS-DL v1.0 (PROPOSED)

Every component has a **visual role, state, interaction and fallback**. New variants require Product Owner approval and a specimen screenshot.

| Name | Geometry / type | States | Behavior / constraints |
| --- | --- | --- | --- |
| `SystemLine` | Max 36px height, content inset 32px | Normal, disconnected | Only relevant time/battery/network; never fake status |
| `PageTitle` | Serif 30/38; leading aligned | Normal, 2 lines | Primary page identity. One per screen |
| `SectionLabel` | Sans 17/23; aligned leading | Normal | No compulsory all-caps eyebrow |
| `BookHero` | Flexible title block + optional 104–116px jacket | Complete, missing cover, long title, missing progress | Primary book always dominates jacket |
| `ReadingProgress` | Text 13/19 with optional short rail | Known, unknown, complete | Never show dummy values or duplicated badges |
| `PrimaryAction` | At least 56px hit height | Default, focused, pressed, disabled, loading | Black fill only if single primary action; semantic verb label |
| `TextAction` | At least 48×48px hit area | Default, focused, disabled | Usually no outline; label + optional arrow |
| `BookRow` | Cover 48×68; row ≥76px; 1–2 lines of title | Normal, focused, unread, completed, missing cover | Search/list/recents; support long Vietnamese titles |
| `UtilityRow` | Row ≥56px; 15/22 label | Default, selected, disabled | One-tap direct navigation; avoid three boxed tiles |
| `FilterAction` | Tap area ≥48px; active indicated via underline/weight | Active, inactive, disabled | Sort/search. Avoid dense segmented switches |
| `ContextPanel` | Opaque white surface; 1px border only if necessary | Open, closed, focus | No gray transparency overlay assumption; e-paper-aware redraw |
| `EmptyState` | Serif 30/38 headline + short explanation + one CTA | First use, unavailable, no results | CTA must solve actual problem |
| `ErrorNotice` | Direct wording + recover/back | SD missing, import failed, unsupported file | No invented status; destructive operations confirm |
| `HardwareFocus` | 2px black inset ring or inversion, 2px clearance | Focused, active | Touch and key paths consistent; do not require color |

## Interaction contract

- All actionable controls must have visible labels or discoverable semantics, a mapped route, an inactive state where needed, and **actual 48px minimum browser hit boxes**. Primary 56px. Hidden hit area may extend beyond visual icon; do not overlap adjacent targets.
- For page turns, buttons and touch zones must not conflict with text selection or reader contextual controls. Hardware binding is marked **unverified** until tested.
- Opaque, high contrast states only; avoid flashing entire screen for tiny widget changes if hardware's partial-refresh constraints allow regional updates. This is a *design request*, not a claim about controller support.
- `disabled` is visually distinct using label and outline/strike, never only lower-contrast gray.
- No toast as the **only** form of confirmation for destructive operations. Confirm dialogs need clear labels and reversible logic when possible.

## Content samples used in acceptance

- Vietnamese long title: **Những người khốn khổ — Tập thứ nhất**
- Stacked Vietnamese diacritics: **Nỗi buồn chiến tranh — Bảo Ninh**
- Missing cover: typography-only neutral jacket (no pretend source artwork)
- Missing progress: show **Chưa bắt đầu** or hide percentage, based on verified data
- Library empty: **Chưa có sách**, primary action **Thêm sách**
- SD missing: **Không tìm thấy thẻ nhớ**, recovery action **Thử lại** only if implemented

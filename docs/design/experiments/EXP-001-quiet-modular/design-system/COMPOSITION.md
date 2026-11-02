# COMPOSITION GRAMMAR — XOS-DL v1.0 (PROPOSED)

## 1. Five non-negotiable rules

1. **One scene, one protagonist.** The primary book or primary job must command the strongest contrast and largest semantic area. No competing KPI, clock, chart, floating pill or gratuitous dashboard component.
2. **Meaningful asymmetry, not random offset.** If cover is right, content left must carry the reading title and progress. Image width is subordinate to text. In narrow text cases, drop/relocate the cover rather than truncate the primary title into nonsense.
3. **Zones follow task order.** Read → Resume → Other Books → Library/Import/Settings. The whole layout, not just labels, must convey that order.
4. **Whitespace is structure.** Separate zones with breathing room; group within zones tightly. Never use arbitrary `space-between` to disguise unresolved hierarchy or create unexplained dead space.
5. **One visual anchor per screen.** Top/bottom actions remain consistent, but are **not** a persistent four-tab app dock. Home provides one-tap shortcuts; other screens have a clear Back/Home path and contextual controls.

## 2. Screen archetypes (mandatory)

### HOME / populated — `READING_STAGE`

Full 480×800 viewport. Position bands are a reference **range**, not hardcoded dynamic data placement:

| Band (Y px) | Responsibility | Constraint |
| --- | --- | --- |
| 0–60 | Quiet identity / status | A single compact row; no storage diagnostics |
| 88–352 | Active reading | Dominant title and book object; one progress line; one CTA |
| 386–615 | Recent | Two legible recent books, using one-column rows rather than three decorative tiles |
| 650–790 | Utilities | Library with book count; Import and Settings direct links; no permanent dock |

**Contrast rhythm:** 80% of hierarchy is in type, size and relative placement; strong filled surface reserved for CTA. The screen should remain comprehensible if all hairlines disappear. Use the right jacket (`104–116 × 148–164`) only if the title can retain at least 190px of width. Prefer dynamic stacking for longer Vietnamese book names. One hero only.

**Content precedence (if vertical overflow):** Current title → resume action → author/progress → Library action → recents (shorten to one) → auxiliary status. Do not silently shrink fonts below tokens to make everything fit.

### HOME / empty — `INGEST_STAGE`

Remove book metadata and fake progress entirely. The headline occupies the main typographic region. **Add books** becomes the one filled CTA. Secondary methods: Browse Files / Wi‑Fi transfer only if supported in the current CrossPoint build. Storage status is shown only in failure/recovery context or explicitly requested. Preserve one-tap Library/Settings access; copy should explain what the action actually does.

### LIBRARY — `EDITORIAL_INDEX`

Large screen title, one compact search/sort zone, followed by **book rows**, not a dashboard card grid. Row: 48×68 cover fallback + title 15/22, author 13/19, concise progress on the trailing side when known. Default List; Grid optional only if separately validated. Tap row opens book; long-press actions are discoverable and have a fallback method. Sorting/search/pagination must function; preserve active sort and position after returning from Reader.

### READER — `QUIET_PAGE`

Reading text owns the screen. No persistent app nav, no permanent Home shortcuts, no low-contrast decorative microtext. Optional one-line book/chapter label and a restrained page/progress indicator. Three-zone tap model is an experiment hypothesis, not a hardware fact. Controls are accessed through a center tap or supported hardware key; users can always return to Library/Home without losing position. Reader formatting is subordinate to EPUB/CrossPoint engine requirements.

### FILES / SETTINGS — `FUNCTIONAL_INDEX`

Clear screen title and simple one-line/two-line grouped rows. Navigation hierarchy and current path visible. Keep technical metadata one step away unless essential to the task. Do not make every setting an outlined card; use whitespace and a rule at group boundaries. Value/state on the trailing edge, never indistinguishable from an action. Full functionality remains accessible even when not all options appear on the landing screen.

## 3. Forbidden compositional shortcuts

- Four boxed tabs at bottom of every screen; dense card stacks; three equally weighted status panels on Home.
- Full-width 56px black slab simply because `PrimaryAction` exists: choose a width and position that serves the composition and tap ergonomics.
- Repeated percentage tags on both cover and label when one representation is enough.
- Extreme serif headline with clipped Vietnamese diacritics.
- 9–10px functional UI labels, raw IP on normal Home, excessive uppercase or all-bold body.
- Empty state that just swaps book title for "No Books" without restructuring actions.
- Invented personal statistics, reading speed estimates or storage health to fill blank space.

## 4. Optical review test

At 100% physical screen size, answer all five:

1. Where does the eye land first? (Must be current book / primary task.)
2. Can I resume reading without scanning icons? (One prominent action.)
3. Can I find Library, Import and Settings? (Visible from Home, one tap.)
4. Remove every rule and badge: does the hierarchy still work? (Must.)
5. A three-line Vietnamese title appears: is the hierarchy preserved without clipping? (Must.)

## 5. Visual reference to concrete implementation

For actual spacing and relative scale, **open `specimen.html` and use its Home and Library examples**, not a new interpretation of the text above. Components must be shared across Home, Library, Reader and Settings; no custom spacing systems per page.

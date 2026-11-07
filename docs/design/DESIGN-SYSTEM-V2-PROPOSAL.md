# NoveOS Design System 2.0

**Design System V2 architecture approved; implementation acceptance remains separate.** The PO-approved Hybrid Editorial Index and semantic architecture remain authoritative. The decisions below record approved evolution without replacing that visual DNA. See the [component inventory](COMPONENT-INVENTORY.md) and [standalone gallery](design-system-gallery.html).

## Authority

- **Approved / required:** NoveOS on the CrossPoint foundation. Hybrid Editorial Index combines Field Index structure with Quiet Editorial typography. Soft Editorial follows **Rounded Interaction, Structured Content**. Retain the approved composition, full-pill primary and secondary actions, English system UI, Vietnamese NFC/NFD content, portrait 480 × 800 monochrome, and touch. Existing physical behavior stays in place. Physical navigation design is deferred.
- **PO-approved V2 decisions:** Library tabs are Recent, Title, Author, mapped to the verified CrossPoint modes. Remove the redundant Sort dropdown; preserve per-mode sort direction and show its direction in the active tab indicator, which can be activated again to reverse it. Wi-Fi import may report success and offer Open Library only after successful transfer and rescan. USB MSC preserves safe disconnect, SD ownership restoration and restart to Home; it does not offer direct Library navigation. Dialog radius is 12 px. These decisions refine interactions while retaining the approved Hybrid Editorial Index DNA.
- [Immutable approved demo](step07a/xteink-hybrid-demo.html) defines visual DNA. [Design System v1](DESIGN-SYSTEM.md) documents the baseline. [STEP 07B decisions](step07b/STEP-07B.1R-DECISIONS.md) take precedence for approved interaction and language evolution. [STEP 06](specs/STEP-06-UX-FLOWS.md) governs behavior and parity. [STEP 07B plan](step07a/STEP-07B-IMPLEMENTATION-PLAN.md) records prior prototype evidence. The V2 gallery illustrates proposed consolidation.
- [WCAG 2.2](https://www.w3.org/TR/WCAG22/) supplies web checks for contrast, keyboard operation, visible and unobscured focus, labels, and status messages. Its AA target-size rule is 24 CSS px with exceptions. NoveOS uses the stricter STEP 06 target of 44 × 44 logical px with 8 px separation. Use [Apple HIG accessibility](https://developer.apple.com/design/human-interface-guidelines/accessibility) as platform guidance and [Nielsen’s heuristics](https://www.nngroup.com/articles/ten-usability-heuristics/) to review status visibility, error recovery and clear exits.

## Tokens

Use logical px at 1:1. The gallery uses the names below as CSS custom properties. Fixed dimensions describe the reference composition. Rows and controls can grow for wrapping.

| Role / token | Value | Basis |
|---|---|---|
| `screen-w`, `screen-h`, `inset-top`, `inset-x`, `inset-bottom` | 480, 800, 29, 31, 22 | Approved frame; 418 px content width |
| `s1`, `s2`, `s3`, `s4`, `s5`, `s6` | 4, 8, 12, 16, 24, 32 | **PROPOSED** shared rhythm; keep named composition exceptions below |
| `paper`, `ink`, `muted`, `rule` | #fff, #121212, #555, #767676 | **PROPOSED** consolidation of reference neutrals. Ink/paper ≈18.7:1, muted/paper ≈7.5:1, rule/paper ≈4.5:1. Device 1-bit proof maps all meaningful marks and text to ink; communicate hierarchy with type, spacing and rules |
| `edge`, `focus`, `selected-rule` | 1, 2, 3 | Demo rules, prototype outline and selected tab. **PROPOSED** shared inset focus with inverse outline on dark surfaces |
| `pill`, `circle`, `dialog-radius`, `structure` | 9999 px, 50%, 12 px, 0 px | PO-approved dialog radius; other geometry follows semantic roles below |
| `target`, `tab-h`, `action-h`, `setting-h`, `book-h` | 44, 48, 50, 64, 107 | STEP 06 minimum and baseline component heights. 56 px primary remains a preference where room allows |
| `icon`, `battery-w`, `battery-h` | 16, 19, 9 | **PROPOSED** 16 px icon box using existing arrows/chevrons; reference battery body. Icons sit inside the control hit region |
| `hero-w` × `hero-h`, `recent-w` × `recent-h`, `cover-w` × `cover-h` | 145 × 210, 42 × 61, 57 × 80 | Reference image geometry by role. Decorative artwork may be square edged |
| `hero-gap`, `hero-top`, `shelf-top`, `reader-top` | 19, 28, 37, 57 | Named reference exceptions for the hero grid, hero/shelf spacing and Reader content offset |

### Typography

| Token | Logical type |
|---|---|
| `serif`, `sans` | Georgia → Times New Roman → serif. Arial → Helvetica Neue → sans-serif. Browser reference stacks |
| `display`, `hero-title`, `chapter`, `section-title` | Serif 43/1.05, 28/1.09, 27/1.2, 23/1.2 |
| `book-title`, `recent-title`, `brand`, `reader` | Serif 20/1.1, 17/1.2, bold 20/1.2, 20/1.62 |
| `body`, `setting`, `meta`, `label` | Sans 13/1.5, 17/1.25, 11/1.45, bold 11/1.45 |
| `display-track`, `label-track` | −0.05em for English display headings, 0.125em for English eyebrow labels. **PROPOSED** zero tracking for book/Reader text with combining marks |

**PROPOSED native font policy:** evaluate the existing Noto Serif / Noto Sans source assets and current EpdFont path against reference metrics and glyph coverage. Select native fallback by run or grapheme so combining marks stay with the base letter. Keep source NFC/NFD intact, normalize only a derived search key, and verify Vietnamese collation including Đ/đ. Let important titles wrap. Validate diacritic clearance, glyph fallback, baseline, line breaks and fit on the device before choosing native font IDs. Browser Georgia and Arial measurements do not establish device metrics.

## Semantic architecture

**User intent → semantic role → component behavior → visual tokens.** Classify by outcome first. Choose a native element and state model for that outcome, then apply the shared treatment. A label, icon, or similar geometry does not make controls interchangeable.

| Family | User intent | Subtypes |
|---|---|---|
| Navigation | Move to a destination or return to prior context | Root destination, contextual Back, tab/view switching, content and service navigation |
| Actions | Run a command | Primary, secondary, destructive, compact and standalone icon action |
| Inputs / Controls | Enter text or change a value | Search, text input, Select, on/off toggle |
| Selection | Choose a value from a set | Select options, selected tab and selected item where a selection model exists |
| Content / Data | Read or inspect information | Book hero and cover, titles and metadata, Reader text, status, position and progress |
| Feedback / Overlays | Understand an outcome or respond to an interruption | Inline notice, loading, empty, error, confirmation dialog |

Tabs are navigation within a view and expose a selection state. Select is the value input and its options are the choices. A book row opens a destination and has no persistent selected state merely because it received focus. See the [component contract matrix](COMPONENT-INVENTORY.md#component-contract-matrix) for per-component behavior and geometry.

### Visual role rules

- **Actions and inputs:** Primary, secondary and destructive text actions, Search, text and closed Select triggers use `pill`. A labeled toggle uses the same radius but communicates a persistent on/off value. A standalone icon action uses a `target` × `target` circle. The select arrow and row chevrons have no independent targets. The expanded native Select menu is browser-owned in this gallery.
- **Navigation:** Root links and upper-left contextual Back are borderless at rest. The root rail is a structural rule. The current destination uses weight, an underline and `aria-current="page"`. Back has a directional cue and a meaningful name. Library tabs retain the approved borderless V2 treatment with a selected underline and active-mode direction cue.
- **Content and overlays:** Content and service rows keep full-width editorial geometry with dividers. Covers, progress tracks and read-only status use `structure`. Dialog containers use the PO-approved 12 px `dialog-radius`. Their actions use the normal command roles.

### State contract

- **Pressed (active input):** Transient `:active` feedback inverts a light control or adds an inset paper ring to an ink control. Release clears the press indication.
- **Focused:** Show the `focus` outline independently of the press, current route or selection. Use the contrasting inverse outline on ink. Keyboard focus on a tab moves to its associated view under the gallery’s automatic activation model. A focus outline alone does not identify a selected tab or current route.
- **Current / selected / on:** Use `aria-current` for the current root destination, `aria-selected` for tabs, native option selection for Select and `aria-pressed` for the toggle. Preserve a persistent non-color cue (underline, value or fill with contrasting text). Action buttons and Back have no persistent selection.
- **Disabled / busy / error:** Unavailable commands have a reason and do not activate. Keep disabled field and action geometry. Busy feedback blocks only conflicting actions. Invalid input retains its label, value, shape and error description. Empty and error messages offer a safe recovery when one exists. Meaningful browser status updates use an appropriate live region.
- **Touch and keyboard:** Controls have a `target` × `target` minimum and `s2` separation. Use native links for destinations and buttons for commands. Enter activates links, Enter or Space activates buttons, arrows move among tabs and native options, and Escape dismisses a cancellable dialog. Return focus to the opener or the valid successor after dismissing.

## Templates and behavior

- **Home:** brand/status, ruled heading, current-book hero, Resume, recent shelf, root navigation. Empty Home offers Import. Unstarted content offers Open. Missing-book recovery leaves Home usable. Home metadata comes from the same progress source as Reader.
- **Library:** title, Recent/Title/Author tabs mapped to native sort modes, labeled Search, bounded list and root navigation. The active tab indicates direction; activating that tab again reverses its mode. No separate Sort dropdown. Distinguish no books from zero results. Preserve the active visit, query, mode and direction, viewport and focused book across Reader.
- **Reader:** one contextual Back at upper left; battery icon and value immediately left of far-right native clock. Keep chapter heading and book text quiet. Footer has `Page X / Y` left and `Z%` right when backed by reliable values. Use `Position X` when the denominator is unavailable and omit unreliable percentage. Preserve existing TOC, settings and refresh access. Independent Home uses the existing native route. The browser specimen illustrates contextual Back only.
- **Settings:** grouped, ruled rows and root navigation. Service child Back restores the originating Settings row and viewport. Optional service errors leave reading and local Library usable.
- **Import:** child header, source choices, stable status and scoped Back. Enable a source only when supported and available. Preserve local file position and the source file on failure. Wi-Fi success and Open Library are available only after transfer completion and successful rescan. USB requires safe host disconnect/eject, restored firmware SD ownership and rescan, then follows native restart-to-Home behavior; it does not navigate directly to Library. Browser states remain simulations and cannot claim device success.
- **NAV-001:** keep tab, query, sort, viewport anchor and offset, and focused book identity. Back restores the visit and uses the nearest valid row with a nonblocking notice if the item vanished. Explicit Home ends that visit. Modal Back closes the modal first. Preserve existing root and physical input ownership.
- **Input / E-Ink:** use one visible target per action with at least `target` on each axis and `s2` separation. Keep Reader tap zones distinct from the HUD and honor modal ownership. Preserve native press/hold/release behavior. Use discrete feedback with stable frames and no required animation. Request bounded redraw through existing activity/render paths. Validate refresh region, stale focus pixels, ghosting, latency and repeated input on hardware before choosing waveform or cadence.

## Open PO decisions and migration

No PO decisions from this scope remain open. Native font selection still needs glyph/layout evidence; independent Home and physical-input parity still need supported-device review. These are implementation and hardware verification items, not pending visual-direction decisions.

After approval, migrate shared tokens and controls in a separate browser change, then migrate one template at a time. Verify Journeys A–D, long-list NAV-001 restoration, empty/error recovery, English/Vietnamese, focus and 1-bit/geometry checks before accepting browser behavior. Map to existing `ThemeMetrics`, `fui::` and activity owners only in a separately authorized integration. Require simulator/hardware evidence and the STEP 06 44-feature parity gate before firmware acceptance.


## Validation evidence

- Earlier browser evidence applies to the prior gallery revision only. Current source/static verification is recorded in the task report; browser interaction and hardware behavior require fresh manual review after these approved changes.
- This is a browser and source review. Native glyph rendering, device touch, physical input, panel contrast, refresh and CrossPoint feature parity still require separate verification.

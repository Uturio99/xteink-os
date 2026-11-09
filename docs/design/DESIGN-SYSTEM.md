# XTEInk OS Design System — Hybrid Editorial Index

> Baseline visual provenance. Later approved shapes, language, Library mapping and import behavior are governed by [V2](DESIGN-SYSTEM-V2-PROPOSAL.md); original values below do not override that evolution.

> **STEP 07A.2 · PO-approved visual baseline.** The Product Owner approved the exact visual DNA of the Hybrid Editorial Index demo. This system records that artifact; it does not claim runtime or hardware acceptance.

## Authority and scope

- Immutable visual reference: [`step07a/xteink-hybrid-demo.html`](step07a/xteink-hybrid-demo.html), copied byte-for-byte from `G:\AI-Setup\XTEInk-X4-Pro\xteink-hybrid-demo.html` (SHA-256 `1777709e0f8f6de4e86409d04094dad015e5e39dd794fd2ddabfcf37320ad513`). Use its rendered device screen and CSS as the source of visual values; the surrounding demo page is review chrome.
- The approved direction combines **Field Index structure** for Home and Library with **Quiet Editorial typography** for Reader and text hierarchy. It supersedes the previous A-only/B-only choice for future design work. Existing A/B/C comparison explorations remain preserved as explorations.
- This lock records visual approval. STEP 07B browser implementation is separately authorized; runtime behavior, firmware integration, simulator acceptance, and hardware QA remain separate gates.

## Foundations and tokens

| Token | Demo value | Use |
|---|---|---|
| Logical screen | 480 × 800 px, portrait, monochrome | `.screen`; aspect ratio 3:5 |
| Screen paper | `#fff` | Device page background |
| Primary ink | `#121212` (`#111` for rules/text; `#151515`/`#171717` for primary/cover fills) | High-contrast text, rules, controls |
| Secondary line | `#c9c9c9` | Base divider token; item rules use `#ddd`, secondary rules `#aaa`/`#bbb` |
| Muted text | `#444`, `#555`, `#666` | Supporting labels and metadata |
| Preview surround | `#e7e7e5` | Demo-only page canvas; not device UI |
| Serif stack | `Georgia, 'Times New Roman', serif` | Brand, page and book titles, cover, Reader |
| Sans stack | `Arial, 'Helvetica Neue', sans-serif` | Status, labels, controls, settings metadata |
| Screen inset | `29px 31px 22px` | Top, horizontal, bottom padding |
| Frequent spacing | 7, 8, 10, 11, 12, 13, 17, 18, 22, 25, 26, 28, 29, 37, 57 px | Use component values below; the demo defines no separate spacing scale |
| Minimum control height | 44 px | Bottom navigation and text/back controls; inherited STEP 06 baseline |

Use the demo’s flat monochrome fills, borders and rules. Do not introduce visual treatments absent from the approved artifact. Its scaling shadow belongs only to the external browser review frame, not the device screen. Preserve Vietnamese text and combining marks; render NFC and NFD correctly without destructive normalization, clipping, or truncating important titles.

The Georgia/Arial stacks describe the browser demo’s system-font fallbacks; they do not establish native firmware font availability. Reuse available project fonts/assets and verify the rendering path separately before any later implementation. Do not add fonts or dependencies as part of this lock.

## Type and layout rules

- Screen header: 27px high; brand 20px bold serif with `-0.065em` tracking; status 11px sans with `0.04em` tracking.
- Page title: 43px Georgia, normal weight, `1.05` line-height, `-0.05em` tracking; 25px top and 8px bottom margins.
- Supporting line: 13px sans, `1.5` line-height. Eyebrows: 11px bold sans, uppercase, `0.125em` tracking. Small metadata uses the demo’s 9–12px sans values.
- Book titles retain the demo’s serif hierarchy: Home detail 28px/1.09, shelf title 23px, recent title 17px, Library item title 20px/1.1. Cover title is 27px/1.03.
- Reader body: 20px/1.62 Georgia, justified; chapter title 27px Georgia, centered. Reader content has 8px horizontal inner padding and a 57px top gap after its header.
- Settings item labels: 17px sans; descriptions 11px; rows at least 64px tall.
- Keep the 480×800 artboard fixed and scale it uniformly to available preview width. The demo’s responsive outer wrapper is presentation framing, not a new device breakpoint contract.

## Screen composition

### Home

Brand/status row and divider; a large editorial page title and short supporting line; current-book hero with a 145×210px cover beside title, author, progress and page metadata (145px/1fr columns, 19px gap); one full-width dark 50px Resume action; ruled shelf with 42×61px recent-book covers; bottom navigation. The cover’s restrained circular line is in the approved source; do not add other decoration. Empty Home behavior and capability-gated imports follow STEP 06, not this populated visual sample.

### Library

Same header, title and supporting line; three equal-width tabs labeled `Tab 01`–`Tab 03` only as neutral placeholders; search and sort affordances; a vertical list with 57×80px covers, title, author, progress/state, and chevron; shared bottom navigation. The names, ordering, and mapping of the three tabs remain unresolved. Do not infer filter semantics from the sample labels.

### Reader

Quiet, text-first page: status header, thin divider, contextual Back at upper-left, centered chapter heading, justified serif body, and page-position footer separated by a rule. Preserve STEP 06 NAV-001: contextual Back restores the originating Library tab, viewport, focus, and query/sort context. Home remains a separate CrossPoint Home key/gesture route; the approved STEP 07B.1R2 evolution removes the redundant Reader Home control. See `step07b/STEP-07B.1R-DECISIONS.md` for its approved styling and language decisions.

### Settings

Title and short supporting line; grouped settings with uppercase tracked group labels and bottom rules; 64px minimum rows with a 17px label, 11px description, and trailing chevron; compact version/footer row and shared bottom navigation. Sample groups are reading experience and connectivity/data, with display, controls, Wi-Fi, OPDS, and extensions entries. Keep all existing CrossPoint settings, services, recovery and capability behavior reachable per STEP 06; the sample is not a complete feature inventory.

## STEP 07B.1R3 — Reader footer evidence boundary

The browser prototype derives its sample footer from mock page position 128 and mock total 320, producing Page 128 / 320 and 40%. These values demonstrate the requested layout only; they are not EPUB pagination or measured book progress. If a runtime format has no reliable total-page denominator, omit / Y and show Position X; show a percentage only when the format provides a reliable progress value. No total is to be inferred from an estimate.
## Interaction and constraints

- Interactive targets are at least 44×44px; the demo uses 44px bottom-navigation/text/back controls, 48px tabs, 50px primary action, and 64px settings rows. Keep at least 8px separation between adjacent touch regions where STEP 06 requires it.
- Demo interactions only switch among sample screens and placeholder tabs; they do not establish journeys, focus restoration, import/USB lifecycle, capability gating, browser QA, or runtime behavior.
- Preserve the STEP 06 1-bit monochrome legibility, Vietnamese NFC/NFD, bounded stable E-Ink states, CrossPoint feature parity, and NAV-001 contracts. Browser visual approval is separate from runtime and hardware validation.
- No firmware or SDK changes are included. The SDK submodule pin remains the repository’s existing pin.

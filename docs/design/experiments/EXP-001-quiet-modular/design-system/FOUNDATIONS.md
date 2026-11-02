# FOUNDATIONS — XOS-DL v1.0 (PROPOSED)

## 0. Design intent

**Quiet Editorial Utility**: A dedicated e-reader, not a miniature Android launcher, analytics dashboard, or static magazine cover. The UI's compositional signature is a **reading-led asymmetric field**, restrained utility list, editorial type with **one** dominant voice, and deliberate vertical rhythm. A user must be able to resume a book immediately and find Library, Import and Settings without hunting.

## 1. Geometry / coordinate system

| Token | Value | Binding rule |
| --- | ---: | --- |
| `screen.width` | 480 | Fixed logical portrait viewport |
| `screen.height` | 800 | Fixed logical portrait viewport |
| `inset.x` | 32 | Shared left/right alignment line |
| `inset.top` | 20 | Preferred top breathing room; system chrome may override |
| `inset.bottom` | 24 | Minimum content clearance for active touch elements |
| `content.width` | 416 | Derived = 480 - 2 × 32 |
| `grid.columns` | 4 | Alignment guides only; **not** four dashboard tiles |
| `grid.gutter` | 16 | Optional internal grid alignment |
| `tap.min` | 48 × 48 | EXP-001 minimum target, intentionally >44px |
| `tap.primary` | 56 × 56 | Preferred primary target, independent of icon size |
| `rule.thickness` | 1 | Rare divider; omit when spacing alone is sufficient |

**Critical:** 480×800 are logical UI pixels. 44pt in Apple HIG is a device-independent iOS unit and **cannot** be mapped blindly to e-paper physical pixels. Actual X4 Pro touch size requires hardware validation. Minimum 48px is a *prototype design assumption*, not a proven physical accessibility standard. Prefer 56–64px for frequent controls.

Use **one shared X-axis** (`32px`) for section text, section labels, and action labels. Book images may align to a secondary guide; do not center every component simply because its container allows it.

## 2. Spacing and vertical rhythm

Allowed spacing scale: `4 / 8 / 12 / 16 / 24 / 32 / 40 / 56` px. No arbitrary `gap: 19px` or filler flex tricks. Large gaps (`40/56`) separate **semantic zones**, medium (`16/24/32`) separates groups, small (`4/8/12`) binds label-to-value relationships.

**Negative space is active:** it establishes a quiet pivot between reading continuity and secondary access. It must not be created by pushing all content to screen edges using `justify-content:space-between` without a compositional plan. Prefer explicit section sizing, minimum clearances and content-aware overflow policy.

## 3. Typography

Two roles only (not endless font stacks):

| Role | Prototype font | Size/line (px) | Usage |
| --- | --- | --- | --- |
| `type.display` | Noto Serif / Georgia fallback | 34 / 42 | Active book title or empty-state headline |
| `type.screenTitle` | Noto Serif / Georgia fallback | 30 / 38 | Library / Files / Settings top-level title |
| `type.section` | UI sans | 17 / 23 | Section title, strong row title |
| `type.body` | UI sans | 15 / 22 | Actions, book title rows, descriptive body |
| `type.secondary` | UI sans | 13 / 19 | Author, chapter, progress |
| `type.micro` | UI sans | 12 / 17 | Status, incidental labels **only** |

- Do **not** assume Noto Serif, SF Pro, Merriweather, or Georgia exists in CrossPoint native. Font names are browser specimen preferences only. Renderer team must choose one verified serif/sans pair **after** Vietnamese glyph and memory tests.
- Prefer sentence case. Uppercase micro labels are permitted only for **one** eyebrow per section, with modest tracking.
- Long Vietnamese title: NFC normalized input in mock data, minimum 1.22 line height with ascender clearance; dynamically allow 2–3 lines where essential; do not ellipsize primary book title to preserve a decorative image.
- No faux drop caps in Reader until EPUB styles and native engine compatibility have been validated.
- Font weight 400/500/600 only in body UI; 700 limited to critical functional emphasis. Do not make every control bold.

## 4. Color, line and imagery

**1-bit information contract:** all semantic distinctions must be understandable on pure **#FFFFFF / #111111**. Browser illustrations may use `#EBEBEB` as a *neutral preview backing* but **never** as the sole carrier of meaning. Test a 1-bit threshold export. Do not claim the CSS itself drives a 1-bit framebuffer.

| Token | Intended value |
| --- | --- |
| `ink.primary` | `#111111` (native mapping: black) |
| `ink.secondary` | `#414141` (browser, must pass binary test) |
| `surface.paper` | `#FFFFFF` |
| `surface.preview` | `#EBEBEB` (preview only) |
| `rule.quiet` | `#BDBDBD` (browser only, may map to black/white in native) |

- A single solid black primary CTA **at most** per view, and only when it marks the single next best action. Outline/filled text links are not both piled up to create visual noise.
- Covers are **data**, not cards. Preserve aspect ratio. When absent, create a typography-only jacket with book title; do not fabricate cover art.
- No shadows, gradients, blurred surfaces, colored emojis, uncontrolled badges, generic 3D pseudo-device embellishments **inside the viewport**.
- Thin rules may organize lists, but do not cage each item in a box.

## 5. Icon specification

- 16–20px visual line glyph, 1.5–2px pixel-aligned stroke; shared optical center and terminal style.
- Label-first. An icon may reinforce text but must not replace a discoverable name for critical actions.
- No colored emoji, no mismatched emoji/SVG hybrids, no external icon CDN dependency.
- Hit area is not icon size; wrap the glyph in a 48–56px button region.

## 6. Not proven in this package

ESP32 font memory, real controller grayscale, physical touch bounds, display waveform timing, actual battery impact, USB/SD behavior and real reader page turns. Browser rendering is an **art-direction reference**, not a hardware benchmark.

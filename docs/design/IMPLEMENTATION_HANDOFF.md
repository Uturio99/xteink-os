# XTEInk OS — Engineering Handoff Document v1.0

> **Document ID:** XOS-HANDOFF-010  
> **Target Audience:** Codex (Firmware Architecture & C++ Implementation Lead) & Product Owner  
> **Author:** Lead Visual Designer  
> **Status:** Production-Ready Firmware Handoff Specification  
> **Hardware Target:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch + Hardware Keys)  
> **Design North Star:** Premium Editorial Minimalism × Apple HIG × Durobo Krono  

---

## 1. Executive Summary & Design Contract

The Product Owner has approved the **Visual North Star v1.0** for the XTEInk OS Home Screen. This specification transitions the interface from earlier structural wireframes into a refined, magazine-style editorial experience characterized by:
- A commanding, centered editorial hero (`116 × 174px` cover, prominent serif display title, expressive italic author credit).
- A refined, hairline progress meter (`140 × 3px` track with high-contrast completion stats).
- An ergonomic, centered `Continue Reading →` button (`210 × 44px`, Apple HIG ≥44px touch height).
- A balanced 3-book secondary shelf (`84 × 126px` thumbnails).
- A quiet top masthead (`42px`) and minimal dual-action footer (`56px`).
- Pure 1-bit black-and-white visual supremacy (solid `#000000` ink on `#FFFFFF` paper, zero decorative gradients, drop shadows, or faux-spine skeumorphism).

To facilitate seamless implementation by Codex, all UI features, architectural requirements, and assets are categorized into three distinct technical tiers:
1. **Directly Supported by Existing CrossPoint Architecture**
2. **Requires New UI Assets or Font Rendering Capabilities**
3. **Requires Hardware Validation on X4 Pro Device Units**

---

## 2. Technical Tier 1: Directly Supported by Existing CrossPoint Architecture

The following components and behaviors align cleanly with verified CrossPoint subsystems and require zero fundamental architectural changes:

| Subsystem / Feature | CrossPoint Implementation Path | Design Contract v1.0 Alignment |
| :--- | :--- | :--- |
| **Canvas & Framebuffer** | `GfxRenderer`, `DisplayController` | Direct 480×800 portrait rendering to the existing single framebuffer. No multi-layer blending or transparency required. |
| **Activity Lifecycle** | `UiAppHost`, `Activity` stack | Home operates as root activity. Pushes `LibraryActivity`, `FileBrowserActivity`, `SettingsActivity`, and `EpubReaderActivity`. |
| **Touch Coordinate Routing** | GT911 driver (`swapXY = true`) | Direct translation of touch coordinates into integer hit-rect checks. All interactive targets meet or exceed Apple HIG `44 × 44px`. |
| **Reading State & Progress** | `RecentBooksStore`, `BookProgress` | Retrieves most recently read book via `RecentBooksStore::getBooks().front()` and progress percentage via `loadBookProgress(path)`. |
| **Graceful Progress Omission** | In-Tree Logic | If progress value < 0 (unopened / uncalculated), the progress label and 3px gauge track are omitted cleanly without layout shift. |
| **In-Tree UI Typography** | `lib/EpdFont/builtinFonts/` | System masthead, static clock, battery stats, kicker, button labels, and footer actions map directly to compiled `ubuntu_8`, `ubuntu_10` (reg/bold), and `ubuntu_12_bold`. |
| **Cover Drawing Pipeline** | `GUI.drawCoverThumbFill` | Existing thumbnail rendering engine can decode and render the cover bitmap into the target bounding box with 1px black border. |
| **Hardware Home Button** | `HomeButtonSettings.h`, GT911 bit `0x10` | Preserves user-configured behavior. Home-on-Home press will **not** trigger an automatic full display refresh. |
| **Physical Button Navigation** | GPIO ISR handlers (`GPIO0`, `GPIO7`) | Left/Right hardware keys cycle shelf selection; Power key (`GPIO3`) handles immediate sleep/wake. |

---

## 3. Technical Tier 2: Requires New UI Assets or Font Rendering

The approved Visual North Star requires elegant serif typography and tailored graphic glyphs to deliver its literary reading experience. Codex must evaluate and select the implementation path:

### 3.1 High-Fidelity Serif Display Typography
- **Requirement:**
  - Hero Title: `21pt` Serif Bold (`advanceY: 26px`, line advance 26px).
  - Hero Author: `13pt` Serif Italic (`advanceY: 20px`).
  - Shelf Section Heading: `13pt` Serif Bold (`advanceY: 22px`).
  - Shelf Thumbnails Metadata: `10pt` Serif Bold / Italic (`advanceY: 14px`).
  - Preferred Families: `Literata`, `Merriweather`, or `Noto Serif`.
- **Implementation Paths for Codex Evaluation:**
  - **Option A (FreeType Dynamic Vector Rendering — Recommended):**
    CrossPoint's EPUB reading engine already embeds FreeType. Evaluate whether FreeType can be exposed to `GfxRenderer` for UI string rendering. This enables flexible kerning, scalable sizes, and native Vietnamese diacritic support (`ệ, ổ, ử, ỹ, ắ`).
  - **Option B (Compiled Bitmap Font Tables):**
    If FreeType in UI is memory-prohibitive on ESP32-C3 (380 KB usable RAM constraint), generate compiled bitmap headers (e.g. `literata_21_bold.h`, `literata_13_italic.h`) using CrossPoint's `font-builder` script. Ensure glyph tables include Latin Extended-A and Vietnamese character sets.
  - **Option C (Interim In-Tree Fallback):**
    If serif assets cannot be generated immediately, the layout coordinates gracefully support in-tree `Ubuntu 12 Bold` (scaled 2× via double-strike or standard) as a temporary engineering fallback.

### 3.2 Cover Thumbnail Dimensions in Cache
- **Requirement:**
  - Hero Cover Slot: `116 × 174px` (2:3 aspect ratio).
  - Shelf Thumbnails: `84 × 126px` (2:3 aspect ratio).
- **Engineering Task:**
  - Check whether `HomeCoverCache` generates fixed 100×150px thumbnails. If so, update the cover cache generator to output `116 × 174px` and `84 × 126px` thumbnails during library indexing to avoid runtime interpolation artifacts and CPU overhead.

### 3.3 1-Bit Vector / Glyph Assets
- **Requirement:**
  - Masthead Search icon (19×19px, 1.7px stroke).
  - Masthead Settings gear icon (19×19px, 1.7px stroke).
  - Wi-Fi signal glyph (16×14px).
  - Battery gauge glyph (26×14px with internal fill bar).
  - Empty State book emblem (48×48px line art).
  - Storage Unavailable warning emblem (48×48px line art).
- **Format:** C++ 1-bit bitmap arrays (`const uint8_t[]`) stored in flash memory.

---

## 4. Technical Tier 3: Requires Hardware Validation on X4 Pro

The following behaviors depend on physical hardware characteristics of the XTEINK X4 Pro and must be verified on bench units:

### 4.1 GT911 Touch Edge Insets & Bezel Clearance
- **Physical Context:** The X4 Pro glass has an integrated perimeter bezel.
- **Validation Points:**
  - **Masthead Touch Bounds (`Y: 0 .. 42`):** Verify that Search (`Rect{278, 0, 44, 42}`) and Settings (`Rect{324, 0, 44, 42}`) register touch events reliably without edge capacitance loss.
  - **Footer Touch Bounds (`Y: 744 .. 800`):** Verify that Library (`Rect{24, 750, 208, 44}`) and Files (`Rect{248, 750, 208, 44}`) register cleanly near the bottom glass margin.
  - *Mitigation:* If edge touch sensitivity is degraded, expand the internal touch hit rects inward by 4–6px while keeping visual rendering identical.

### 4.2 E-Paper Controller & Waveform Partial Refresh
- **Physical Context:** X4 Pro production batches may incorporate different display controllers (e.g. SSD1677, UC8179, or UC8279).
- **Validation Points:**
  - **Partial Refresh Waveform (`0xFC` / `DU4`):** When tapping shelf items or switching content states, verify that partial screen updates do not induce severe ghosting or edge artifacts on 1-bit typography.
  - **Hairline Rendering (1px Rules):** Confirm that 1px hairline rules (`(0, 41)` masthead rule, `(24, 426)` divider rule, and `(0, 744)` footer rule) remain unbroken across different controller waveform batches.

### 4.3 Static Clock RTC Wakeup Impact
- **Design Rule:** The clock display (`14:45`) does **not** run an active background ticker task. It updates only on wake-from-sleep, activity resume, or when a 1-minute hardware RTC alarm interrupt fires.
- **Validation Point:** Ensure the RTC interrupt does not trigger full panel flashing or disrupt deep sleep battery autonomy.

---

## 5. File Inventory & Deliverable Map

| File Path | Description & Purpose | Status |
| :--- | :--- | :--- |
| `docs/design/home-v1.0.html` | Authoritative 480×800 interactive prototype. Features state switcher for all 6 content states (A through F) and 1-bit monochrome simulator. | **Delivered** |
| `docs/design/DESIGN_SYSTEM_v1.0.md` | Core visual principles, 1-bit contrast supremacy, spatial budget, typography scale, and component design tokens. | **Delivered** |
| `docs/design/HOME_SPEC_v1.0.md` | Master layout coordinate table (`X, Y, W, H`), font metrics, stacked diacritic clearance rules, and navigation routing. | **Delivered** |
| `docs/design/IMPLEMENTATION_HANDOFF.md` | Engineering classification table, font asset strategy, and hardware validation items for Codex. | **Delivered** |
| `docs/design/index.html` | High-fidelity design showcase embedding `home-v1.0.html` with direct documentation links. | **To Update** |
| `xteink-os-design-v1.0.zip` | Standalone deliverable archive containing all specifications, prototypes, and assets. | **To Package** |

---

## 6. Recommended Next Actions for Codex

1. **Font Strategy Alignment:** Decide whether to expose FreeType to UI rendering for 21pt serif headings or compile static bitmap headers (`literata_21_bold.h`).
2. **Coordinate Header Creation:** Create `HomeLayout_v1.h` with the exact integer `Rect` constants specified in Section 2 of `HOME_SPEC_v1.0.md`.
3. **State Integration:** Map `HomeScreenActivity` state machines directly to States A, B, C, D, E, and F as specified.
4. **Hardware Bench Testing:** Flash test build to X4 Pro unit to confirm GT911 edge sensitivity and 1px hairline e-paper rendering.

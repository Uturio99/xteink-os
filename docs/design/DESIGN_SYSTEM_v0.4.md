# XTEInk OS — Design System v0.4: Production Editorial Foundations

> **Document ID:** XOS-DS-004  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch + Hardware Keys)  
> **Role:** Lead Product & UI Designer  
> **Status:** Production Specification for Product Owner Approval & Codex Firmware Architecture  
> **Upstream Reference:** HOME-004 Refinement (Building upon v0.3 findings)

---

## 1. Product Direction & Design Principles

Design System v0.4 refines XTEInk OS into an authoritative, production-grade Home Screen experience for the **Xteink X4 Pro**. The design marries the contemplative, literary gravitas of **Durobo Krono** with the functional clarity, predictability, and deference of **Apple Human Interface Guidelines (HIG)**.

### 1.1 Core Principles
1. **The Screen as Printed Paper (1-Bit Supremacy):** All essential reading information, book titles, progress meters, navigation triggers, and icons must be 100% legible in pure binary black (`#000000`) on pure white (`#FFFFFF`). Grayscale is strictly an optional aesthetic enhancement and is never relied upon for structural legibility.
2. **De-Skeuomorphing & De-Cluttering:** Zero drop shadows (`box-shadow`), zero fake spine creases, zero faux bevels, and zero pale-gray container backgrounds (`#FAFAFA`, `#F0F0F0`). Surfaces are honest, clean white paper.
3. **One Dominant Focal Point:** The active reading journey ("Currently Reading") commands immediate visual attention upon wake, giving the reader instant context to resume reading with a single tap.
4. **Lightweight, Discoverable Navigation:** Eliminates the heavy, persistent 5-box bottom dock in favor of an **Editorial Masthead** (top bar housing Search and Settings) combined with a streamlined **Bottom Action Ribbon** (direct access to Library and Files).
5. **Static E-Paper Discipline:** Zero UI animations, zero sliding transitions, and zero continuous clock timers. The screen state is stable and intentional, refreshing only upon user interaction or discrete system events.

---

## 2. Hardware Architecture & Display Geometry

### 2.1 Hardware Specifications (Xteink X4 Pro)
- **Physical Panel:** 800 × 480 physical e-paper display (landscape native glass).
- **Portrait UI Canvas:** **480 × 800** physical pixels (`swapXY = true` in GT911 touch and display orientation driver). All UI coordinates and layout boxes are defined strictly in this 480×800 portrait space.
- **Controller Variants:** SSD1677, UC8179, or UC8279 (auto-detected by firmware at boot). Waveforms: OTP Full (`0xF7`) and Fast Partial (`0xFC`).
- **Touch Controller:** Goodix GT911 capacitive touch over I²C (address `0x5D`, INT=GPIO10, RST=GPIO4, power=GPIO2 active-low).
- **Capacitive Home Key:** Integrated touch key on the lower glass bezel (GT911 status register `0x814E & 0x10`).
- **Physical Buttons:**
  - **Left Button (GPIO0):** Digital active-low, internal pull-up. Mapped to Up / Previous.
  - **Right Button (GPIO7):** Digital active-low, internal pull-up. Mapped to Down / Next.
  - **Power Button (GPIO3):** Sleep / Wake / Power toggle.

---

## 3. Typography Hierarchy & Firmware Alignment

To avoid unverified engineering claims, typography is strictly divided into **in-tree verified firmware bitmap fonts** and **proposed display scale tokens** that require engineering validation.

> [!IMPORTANT]
> **Renderer Note:** CSS font sizes in browser prototypes represent target optical proportions for evaluation. They do not claim a 1:1 binary match with compiled FreeInk bitmap font rendering tables.

### 3.1 Typographic Scale Table
| Scale Level | CSS Preview Token | Firmware Font Status | Line Height | Tracking | Primary Usage |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Micro** | `8px` (`--xos-font-micro`) | **Verified In-Tree** (`Ubuntu 8 Regular`) | `10px` | `+0.12em` | Uppercase category kickers (`CURRENTLY READING`), format tags (`EPUB`) |
| **Caption** | `10px` (`--xos-font-caption`)| **Verified In-Tree** (`Ubuntu 10 Regular/Bold`) | `14px` | `+0.04em` | Metadata, timestamps, reading statistics, progress percentage |
| **Nav** | `12px` (`--xos-font-nav`)| **Verified In-Tree** (`Ubuntu 12 Regular/Bold`) | `16px` | `+0.02em` | Ribbon labels, author names, chapter indicators, button text |
| **Body** | `14px` (`--xos-font-body`)| *Proposed display token* | `20px` | `0.00em` | Main UI body, shelf titles, dialog message copy |
| **Section Head** | `18px` (`--xos-font-subhead`)| *Proposed display token* | `24px` | `-0.01em` | Section headers, overlay titles ("Your Library is Waiting") |
| **Editorial Display**| `22px–26px` (`--xos-font-display`)| *Proposed display token* | `26px–30px`| `-0.015em`| Dominant hero book titles on the Home Screen |
| **Serif Accent** | Variable (`--xos-font-serif`)| *Proposed vector token* | — | — | Optional literary styling for hero titles (`Merriweather` / `Georgia`) |

---

## 4. Spacing, Geometry & 1-Bit Palette Tokens

All layout metrics are specified in integer pixel values to match firmware framebuffer blitting:

```css
:root {
  /* ==========================================================================
     COLOR & INK TOKENS (Strict 1-Bit Baseline)
     ========================================================================== */
  --xos-ink:              #000000; /* Solid black: text, active borders, progress fills */
  --xos-paper:            #FFFFFF; /* Solid white: canvas paper, inverted elements */
  --xos-ink-subtle:       #444444; /* Dark gray: secondary author/caption (Contrast 9.7:1) */
  --xos-rule:             #000000; /* Hairline primary rule */
  --xos-rule-quiet:       #E2E2E2; /* Subtle divider rule */

  /* ==========================================================================
     LAYOUT BANDS & VIEWPORT (480x800 Integer Coordinates)
     ========================================================================== */
  --xos-screen-w:         480px;
  --xos-screen-h:         800px;
  --xos-inset-h:          24px;   /* Safe horizontal inset clearing glass bezel */
  --xos-masthead-h:       44px;   /* Top masthead height */
  --xos-ribbon-h:         56px;   /* Bottom action ribbon height */
  --xos-touch-min:        44px;   /* Minimum Apple HIG touch target hit box */

  /* ==========================================================================
     SPATIAL SCALE (4px / 8px Grid)
     ========================================================================== */
  --xos-space-4:          4px;    /* Micro spacing / track height */
  --xos-space-8:          8px;    /* Element padding */
  --xos-space-12:         12px;   /* Shelf & ribbon gap */
  --xos-space-16:         16px;   /* Component separation */
  --xos-space-18:         18px;   /* Content top padding */
  --xos-space-24:         24px;   /* Major section whitespace */

  /* ==========================================================================
     GEOMETRIC PRIMITIVES
     ========================================================================== */
  --xos-border-1px:       1px solid var(--xos-ink);
  --xos-border-quiet:     1px solid var(--xos-rule-quiet);
  --xos-radius-none:      0px;    /* Clean book corners */
  --xos-radius-subtle:    2px;    /* Interactive buttons */
}
```

---

## 5. Reusable UI Components

### 5.1 Editorial Masthead (`.masthead`)
- **Dimensions:** Width: `480px`, Height: `44px`.
- **Left Region:** Brandmark `XTEINK` (11px Bold, tracking +0.14em) accompanied by a quiet clock (`14:45`).
- **Clock Policy:** Fixed rendering. The clock updates **only** on screen wake, activity transitions, or a discrete minute boundary event triggered by the BM8563 RTC when awake. No continuous interval loops.
- **Right Region:**
  - `Search` touch button (min 44×44px hit bounds, 19×19px icon).
  - `Settings` touch button (min 44×44px hit bounds, 19×19px icon).
  - System status indicators: Wi-Fi symbol and battery percentage with a 1-bit battery outline (17×9px) and solid fill.
- **Boundary:** 1px hairline solid black rule (`Y = 44`).

### 5.2 Dominant Hero Reading Card (`.hero-layout`)
- **Dimensions:** Bounded within `Y = 44` to `Y = 320`.
- **Cover Artwork:** Exactly `132 × 198px` (2:3 aspect ratio). Framed in a single 1px solid black border. No drop shadows, no fake spine crease, no gray backing.
- **Typographic Stack:**
  - Kicker: `CURRENTLY READING` (`8px Bold`, uppercase, tracking +0.12em).
  - Title: Display typography (`22px–26px Bold`), wrapping up to 3 lines with ellipsis.
  - Author: `12px Regular` (Ubuntu 12).
  - Progress Meter: 4px height progress box (1px solid black outline, solid black interior fill).
  - Reading Stats: `64%` (15px Bold) accompanied by `114 pages left` and `Chapter 14: The Escape to the Ice` (10px).
- **Resume Hit Target:** High-contrast tap area (`btn-resume`, height: 44px) with explicit label: `Resume Reading →`.

### 5.3 Typographic Book Jacket Fallback (`.cover-fallback`)
- Displayed when an EPUB file has no embedded cover artwork.
- Dimensions: Exactly matching the `132 × 198px` cover frame.
- Design: Clean 1px solid black outer border with an inner 1px inset rule, centered title (Ubuntu 10/12 Bold), centered author, and bottom `EPUB · COMPLETE` hallmark.

### 5.4 Recent Books Shelf (`.shelf-grid`)
- **Capacity:** Maximum **2 to 3 books** (enforcing generous whitespace).
- **Layout:** 3-column horizontal grid (`gap: 16px`).
- **Thumbnail Box:** Exactly `80 × 120px` (2:3 aspect ratio) with 1px black border.
- **Progress Badge:** High-contrast solid black badge at bottom-right (`font-size: 8px Bold`, white text): `31%`, `88%`, or `DONE`.
- **Metadata:** Book title (`10px Bold`, 1-line truncation) and author (`8px Regular`, 1-line truncation).
- **Header:** Section title `Recent on Shelf` (14px Bold) with right-aligned `All Books (24) →` tap target (44px vertical touch padding).

### 5.5 Bottom Action Ribbon (`.bottom-ribbon`)
- **Dimensions:** Width: `480px`, Height: `56px`, bounded at `Y = 744` to `Y = 800`.
- **Role:** Replaces the heavy 5-item dock with two generous, discoverable touch actions:
  1. **`Library (24)`**: Opens the indexed book collection.
  2. **`Files (SD Card)`**: Opens the MicroSD directory file browser.
- **Hit Ergonomics:** Each button measures `210 × 44px` with a 1px solid black border, 17×17px icon, and 12px bold typography.

### 5.6 State Overlays (Empty Library & Storage Error)
- Centered within the main viewport when active:
  - **Empty Library:** 1-bit open book icon, title `Your Library is Waiting`, explanatory copy, and `Open File Browser` action button (44px height).
  - **Storage Error:** 1-bit warning icon, title `SD Card Unavailable`, explanatory copy, and `Retry Storage Mount` action button (44px height).

---

## 6. Interaction Model & Input Routing

### 6.1 Capacitive Touch (Primary Input)
- All interactive elements meet or exceed Apple HIG's **44 × 44px** hit bounds.
- On touch release, the system executes an instantaneous 1-frame highlight/inversion (`tapFlash`) before transitioning activities, giving the user immediate tactile feedback on e-paper.

### 6.2 Hardware Buttons & Capacitive Home Mapping
1. **Capacitive Home Key (GT911 bit `0x10`):** Always returns to the Home Screen or refreshes the view.
2. **Left Nav Button (GPIO0):** Scrolls or selects Previous page/item.
3. **Right Nav Button (GPIO7):** Scrolls or selects Next page/item.
4. **Power Button (GPIO3):** Enters deep sleep / wakes device.

---

## 7. Functional Parity Matrix

All 6 core functional pillars are preserved without visual clutter:

| Core Requirement | Home v0.4 Access Point | Touch Target Size |
| :--- | :--- | :--- |
| **1. Continue Reading** | Large Hero Cover + "Resume Reading" Button | Full Hero (`>200px`) & Button (`170×44px`) |
| **2. Recent Books** | 3-Column Shelf ("Recent on Shelf") | 3 × (`80×150px`) tap zones |
| **3. Library** | Bottom Action Ribbon (`Library (24)`) & Shelf Header Link | Ribbon (`210×44px`) & Link (`100×44px`) |
| **4. Files** | Bottom Action Ribbon (`Files (SD Card)`) | Ribbon (`210×44px`) |
| **5. Search** | Top Masthead Search Icon Button | Icon Button (`44×44px`) |
| **6. Settings** | Top Masthead Settings Icon Button | Icon Button (`44×44px`) |

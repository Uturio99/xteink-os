# XTEInk OS — Design System v0.3 (QA Revised)

> **Document ID:** XOS-DS-003  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait E-Paper Display, Capacitive Touch)  
> **Status:** Revised Proposal for Product Owner Review  
> **Role:** Visual Design Lead  
> **Upstream Reference:** Design QA Corrections v0.3 (Addressing XOS-TECH-001 findings)

---

## 1. Scope & Product Direction

### 1.1 Target Hardware Lock: Xteink X4 Pro
This design system is specifically engineered for the **Xteink X4 Pro**:
- **Display:** 480 × 800 physical pixels portrait e-paper panel.
- **Input:** Capacitive touch screen. All interactive controls are touch-driven with generous hit targets (minimum 44×44px hit bounds per Apple HIG).
- **Scope Restriction:** All legacy cross-device specifications for non-touch button devices (Xteink X3/X4), physical button cursor rings, and generic ESP32-C3 memory conjectures have been removed.

### 1.2 Core Aesthetic: Hybrid Editorial Minimalism
The visual language merges the contemplative literary elegance of **Durobo Krono** with the functional clarity and deference of **Apple HIG**:
- **Black-and-White-First (1-Bit Supremacy):** All essential UI elements, text, icons, progress bars, and borders are strictly pure black (`#000000`) on pure white (`#FFFFFF`). Grayscale is treated as strictly optional and is excluded from functional chrome to prevent ghosting and waveform artifacts on X4 Pro panels.
- **De-Skeuomorphing & De-Cluttering:** Removed all artificial drop shadows (`box-shadow`), simulated book spines, fake spine creases, and unnecessary gray card backdrops (`#FAFAFA`, `#F0F0F0`). The screen behaves as authentic printed paper.
- **One Dominant Focal Point:** The active book ("Currently Reading") commands visual priority.
- **Quieter Navigation:** Replaces the heavy, persistent 5-box application dock with a lighter, more refined navigation treatment that preserves full access to **Continue Reading, Recent Books, Library, Files, Search, and Settings** without dominating the reading canvas.
- **Static E-Paper Presentation:** No continuous timer loops or JS animations that could induce unnecessary e-ink flashing.

---

## 2. Typography Specification & Font Categorization

To maintain strict engineering integrity, we explicitly distinguish between firmware-verified in-tree fonts and proposed display sizes that require engineering validation.

### 2.1 Firmware-Supported Fonts (In-Tree Verified)
CrossPoint firmware compiles fixed bitmap fonts in `lib/EpdFont/builtinFonts/`:
- **`Ubuntu 8 Regular`**: Used for micro metadata, uppercase category kickers, and format indicators.
- **`Ubuntu 10 Regular & Bold`**: Used for metadata, author names, reading percentages, and secondary timestamps.
- **`Ubuntu 12 Regular & Bold`**: Used for navigation labels, section subheads, and secondary book titles.

### 2.2 Proposed Display Typography (Requires Engineering Validation)
The editorial hero headings and primary book titles require larger sizes. These are proposed design tokens for FreeInkUI vector rendering (FreeType) or future bitmap asset generation:
- **`14px Body / Subhead`**: Main UI body, dialog prompts, primary list titles.
- **`18px – 20px Section Title`**: Section headers ("Currently Reading", "Recent Books").
- **`22px – 28px Editorial Display`**: Dominant hero book titles on the Home screen.
- **Serif Accent (`Merriweather` / `Georgia`)**: Optional literary styling for hero titles when vector font scaling is enabled; falls back cleanly to Ubuntu.

### 2.3 Typographic Scale & Metrics
```css
:root {
  /* Firmware In-Tree Fonts (Verified) */
  --xos-font-micro:       8px;  --xos-leading-micro:    10px; /* Ubuntu 8 Regular */
  --xos-font-caption:     10px; --xos-leading-caption:  14px; /* Ubuntu 10 Regular/Bold */
  --xos-font-nav:         12px; --xos-leading-nav:      16px; /* Ubuntu 12 Regular/Bold */

  /* Proposed Display Scale (Requires Engineering Validation) */
  --xos-font-body:        14px; --xos-leading-body:     20px;
  --xos-font-subhead:     18px; --xos-leading-subhead:  24px;
  --xos-font-display-sm:  22px; --xos-leading-display-sm:26px;
  --xos-font-display-lg:  28px; --xos-leading-display-lg:32px;

  /* Weights & Tracking */
  --xos-weight-regular:   400;
  --xos-weight-bold:      700;
  --xos-tracking-display: -0.015em;
  --xos-tracking-micro:   0.08em;
}
```

---

## 3. Spatial & Geometric Tokens (X4 Pro 480×800)

```css
:root {
  /* Display Geometry */
  --xos-screen-w:         480px;
  --xos-screen-h:         800px;
  --xos-inset-h:          24px;  /* Generous horizontal safe margin */
  
  /* Layout Bands */
  --xos-header-h:         36px;  /* Status bar */
  --xos-nav-h:            52px;  /* Refined navigation zone */
  --xos-touch-min:        44px;  /* Apple HIG ergonomic touch target */

  /* Spacing Scale (4px / 8px Grid) */
  --xos-space-4:          4px;
  --xos-space-8:          8px;
  --xos-space-12:         12px;
  --xos-space-16:         16px;
  --xos-space-20:         20px;
  --xos-space-24:         24px;
  --xos-space-32:         32px;

  /* Color: Pure 1-Bit Ink & Paper */
  --xos-ink:              #000000;
  --xos-paper:            #FFFFFF;
  --xos-rule:             #000000;
  --xos-rule-quiet:       #E0E0E0; /* Subtle structural divider */

  /* Borders & Geometry */
  --xos-border-1px:       1px solid var(--xos-ink);
  --xos-border-quiet:     1px solid var(--xos-rule-quiet);
  --xos-radius-none:      0px;
  --xos-radius-subtle:    2px;
  --xos-radius-pill:      999px;
}
```

---

## 4. Reusable UI Components & Navigation Refinements

### 4.1 System Status Bar (`.status-bar`)
- **Height:** 36px.
- **Layout:** Left: Context/Brandmark (`XTEINK`); Center: Time (`14:45`); Right: Wi-Fi status and battery percentage with a 1-bit battery glyph.
- **Rule:** 1px subtle divider (`#E0E0E0`) or pure whitespace separation.
- **Behavior:** Static rendering without continuous JS timers.

### 4.2 Editorial Hero Reading Card (`.hero-card`)
- **Dominant Focus:** Allocates prime vertical real estate to the current book.
- **Cover Artwork:** Authentic 2:3 aspect ratio (130×195px). Encased in a clean 1px solid black border. **Zero drop shadows, zero faux spines, zero artificial gray backgrounds.**
- **Typographic Stack:**
  - Micro Kicker: `CURRENTLY READING` (Ubuntu 8, uppercase, bold).
  - Title: 22px–26px bold display title (maximum 3 lines, clean truncation).
  - Author: 12px regular (Ubuntu 12).
  - Progress: Crisp 4px progress bar (1px solid black bounding box with solid black fill on white) accompanied by exact statistics: `64% · 114 pages left · Chapter 14`.
- **Resume Hit Target:** High-contrast tap area (min 44px height) with explicit action label: `Resume Reading →`.

### 4.3 Typographic Book Jacket Fallback (`.cover-fallback`)
When an EPUB lacks embedded artwork:
- Clean 1px solid black outer frame with an inner 1px inset rule (classic French Gallimard / Penguin editorial style).
- Center-aligned book title, author, and `EPUB` format mark.
- No faux 3D shading or skeuomorphic folds.

### 4.4 Recent Books Shelf (`.shelf-grid`)
- 3-column horizontal shelf for recently opened titles.
- Cover thumbnails (80×120px, 2:3 ratio) with clean 1px borders.
- Solid black progress badge (`31%`, `88%`, `Done`) positioned at the bottom right of each cover.
- Title and author cleanly truncated to 1 line each.

### 4.5 Navigation Refinements (Departing from Persistent 5-Box Docks)
Audit QA identified that a thick, persistent 5-box tab bar at the bottom steals vertical space and breaks the editorial feel. Design System v0.3 introduces two refined navigation solutions:

1. **Primary Navigation: The Quiet Dock (`.quiet-dock`):**
   - A streamlined, open bottom bar with 5 deliberate destinations (Home, Library, Files, Search, Settings).
   - Removes heavy box outlines, fills, and thick borders.
   - Uses generous touch paddings (44×48px tap areas) with minimal 1.5px stroke icons and clear labels.
   - Active state is communicated with an understated 2px solid black underline indicator.
2. **Alternative Navigation: Editorial Header & Footer (`home-alternative.html`):**
   - Top Masthead provides quiet utility shortcuts (`Search` and `Settings` at top-right).
   - Bottom area is an open editorial shelf with clean text/icon pills for `Library (24)` and `Files (SD)`.
   - Leaves 85% of the viewport completely unencumbered for reading immersion.

### 4.6 Empty State (`.empty-state`)
- For new devices or libraries without opened books:
- Minimalist 1-bit open book icon (36×36px).
- Headline: "Your Library is Waiting" (18px Bold).
- Clear explanatory copy: "Transfer EPUB books via USB or connect to Wi-Fi to sync your collection."
- Primary Action: "Open File Browser" (44px height black pill button).

---

## 5. Changelog: Post-QA Revisions (v0.2 → v0.3)

| QA Requirement | Action Taken in v0.3 |
| :--- | :--- |
| **1. Remove cross-device references** | Removed all specifications for X3/X4 button input, hardware focus cursor rings, and generic ESP32-C3 conjectures. Locked to **XTEINK X4 Pro** touch hardware. |
| **2. Remove unsupported benchmarks** | Removed all speculative ratings ("9.5/10") and unverified memory allocation numbers (~24KB PSRAM). Focused strictly on concrete architectural and UI attributes. |
| **3. Focus on Editorial Focus** | Made **Editorial Focus** the definitive primary direction. Borrowed clean progress tracking and library shortcut patterns from Utility without cluttering the screen. |
| **4. De-skeuomorphing & De-cluttering** | Stripped out all `box-shadow: 2px 2px 0px`, faux spine crease overlays (`::before` spine gradients), and gray card backgrounds. Surfaces are now pure white paper. |
| **5. Quieter navigation** | Replaced the heavy 5-box tab container with a quiet, open navigation bar in the Primary prototype and an Editorial Masthead in the Alternative exploration. |
| **6. Typographic clarity & whitespace** | Increased horizontal safe margins to 24px; strengthened display hierarchy; refined tracking and line heights. |
| **7. 6 Core destinations preserved** | Verified full access to Continue Reading, Recent Books, Library, Files, Search, and Settings across both prototypes. |
| **8. Firmware vs. proposed font labeling** | Explicitly categorized Ubuntu 8/10/12 as in-tree firmware supported, and marked sizes >12px as proposed tokens requiring engineering validation. |
| **9. 1-Bit black/white supremacy** | Enforced solid black on white for all essential text, progress meters, and borders. Grayscale is strictly optional. |
| **10. Static e-paper presentation** | Removed all automatic JS clock interval tickers and CSS transitions to reflect true static e-paper state. |
| **11. Retain empty & fallback states** | Maintained full support and interactive simulator toggles for missing covers and empty library states. |
| **12. Touch ergonomics** | Standardized all interactive elements to meet or exceed Apple HIG's 44×44px hit bounds. |

# XTEInk OS — Design System v1.0: High-Fidelity Editorial North Star

> **Document ID:** XOS-DS-010  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch + Hardware Keys)  
> **Design Authority:** Product Owner Approved Visual North Star (HOME-006)  
> **Role:** Lead Visual Designer  
> **Status:** High-Fidelity Design Contract for Product Owner Sign-off & Codex Firmware Planning  

---

## 1. Visual North Star & Design Philosophy

Design System v1.0 marks the definitive transition from earlier structural wireframes to the **Product Owner Approved Visual North Star**: **Premium Editorial Minimalism × Apple HIG × Durobo Krono**.

The system treats the XTEINK X4 Pro display not as a handheld computer dashboard, but as fine, exquisitely typeset literary print.

```
+=======================================================+
|  XTEINK  14:45                             [Q] [*] 86% |  Quiet Masthead (42px)
+=======================================================+
|                                                       |
|                  CURRENTLY READING                    |
|                                                       |
|                    +-------------+                    |
|                    |             |                    |
|                    | [Book Cover]|                    |  Strong, Centered
|                    |   116x174   |                    |  Editorial Hero
|                    +-------------+                    |
|                                                       |
|              The Left Hand of Darkness                |  Dominant Focal Point (Serif)
|                 Ursula K. Le Guin                     |  Expressive Italic Accent
|                                                       |
|                  64% COMPLETED                        |  Refined Progress Indicator
|                  [=========== ]                       |
|                                                       |
|             [ CONTINUE READING -> ]                   |  Prominent, Minimal Action (44px)
|                                                       |
|-------------------------------------------------------|  Subtle Rule
|                                                       |
|  Recent Books                     All Books (24) ->   |  Carefully Composed
|  +-------+         +-------+         +-------+        |  3-Book Shelf
|  | Dune  |         | Neuro |         | Found |        |
|  +-------+         +-------+         +-------+        |
|                                                       |
|                                                       |  Luxurious Whitespace
+=======================================================+
|     [ Library (24) ]       [ Files (SD Card) ]        |  Minimal Dual Footer (56px)
+=======================================================+
```

### 1.1 Core Visual Characteristics
1. **Centered Editorial Gravitas:** Replaces the previous asymmetrical horizontal split with a **strong, centered editorial hero** that commands visual priority.
2. **Expressive Serif Typography:** Book titles and author credits are set in refined serif type with expressive italic accents (`Merriweather` / `Literata` / `Georgia`), evoking traditional fine bookbinding.
3. **One Dominant Focal Point:** The active book title is the primary visual anchor of the entire canvas.
4. **Refined Reading-Progress Unit:** Replaces heavy rectangular bars with a slender, centered 3px hairline track and clear percentage completion.
5. **Prominent, Minimalist Button:** A clean, centered touch button (`Continue Reading →`) with minimum 44px touch height, unadorned by skeuomorphic gradients or bevels.
6. **Carefully Composed Three-Book Shelf:** Maximum 3 books in a balanced horizontal row, providing quick secondary continuity while maintaining expansive negative space.
7. **Quiet Masthead & Minimal Footer:** Clean top bar (42px) housing search and settings; minimal bottom dual-action ribbon (56px) for Library and Files. Zero permanent 5-item app docks.
8. **Pure Black-and-White-First (1-Bit Supremacy):** Solid black (`#000000`) ink on solid white (`#FFFFFF`) paper. No gradients, shadows, blurs, or translucent overlays.

---

## 2. Typography Specification & Font Requirements

### 2.1 Serif North Star vs. In-Tree Bitmap Fonts
The approved Visual North Star mandates **expressive serif typography** for literary immersion. To maintain technical honesty with Codex:
- **In-Tree Firmware Baseline:** `Ubuntu 8`, `Ubuntu 10` (regular/bold), and `Ubuntu 12` (regular/bold) are compiled in `lib/EpdFont/builtinFonts/`. They serve as the system UI baseline.
- **High-Fidelity Serif Requirement (For Codex Validation):** The hero title, author subtitle, and shelf titles require high-quality serif font assets (e.g. `Literata`, `Merriweather`, or `Noto Serif`). Codex will validate whether these are rasterized via FreeType vector scaling (already used in the EPUB reader engine) or pre-compiled into bitmap tables.

### 2.2 Reconciled Typographic Scale Table
| Role | Font Family | Size / Line Advance | Style & Tracking | Firmware Asset Requirement |
| :--- | :--- | :--- | :--- | :--- |
| **Kicker** | `Ubuntu Sans` | `8pt` (`advanceY: 19px`) | Bold, uppercase, tracking `+0.16em` | **In-Tree Verified** (`ubuntu_8_regular`) |
| **Hero Title** | `Serif Display` | `21pt` (`advanceY: 26px`) | Bold, tracking `-0.015em` | **Requires Serif Font Asset** (`Literata` / `Merriweather`) |
| **Hero Author** | `Serif Accent` | `13pt` (`advanceY: 20px`) | Italic, color `#444444` | **Requires Serif Font Asset** (Italic) |
| **Progress Stats** | `Ubuntu Sans` | `9.5pt` (`advanceY: 16px`)| Bold, tracking `+0.04em` | **In-Tree Verified** (`ubuntu_10_bold`) |
| **Continue Action**| `Ubuntu Sans` | `11pt` (`advanceY: 24px`) | Bold, uppercase, tracking `+0.06em` | **In-Tree Verified** (`ubuntu_10/12_bold`) |
| **Shelf Title** | `Serif Subhead` | `13pt` (`advanceY: 22px`)| Bold | **Requires Serif Font Asset** |
| **Shelf Thumb Meta**| `Serif Body` | `10pt` (`advanceY: 14px`)| Bold (Title) / Italic (Author) | **Requires Serif Font Asset** |
| **Masthead / Footer**| `Ubuntu Sans` | `10.5–12pt` (`advanceY: 24–29px`) | Bold / Regular | **In-Tree Verified** (`ubuntu_10/12`) |

### 2.3 Diacritic Headroom & Vietnamese Support
Line advances allocate full vertical clearance for stacked diacritical marks (e.g. `Nỗi Buồn Chiến Tranh`, `Bảo Ninh`). Diacritics are safeguarded by allocating at least `advanceY` (or 24px) per single line, ensuring no clipping occurs on e-paper.

---

## 3. Spatial System & Coordinate Budget (480×800 Canvas)

```css
:root {
  /* Master Dimensions */
  --xos-screen-w:         480px;
  --xos-screen-h:         800px;
  --xos-inset-h:          24px;   /* Lateral safe margin clearing bezel */
  --xos-masthead-h:       42px;   /* Quiet top masthead */
  --xos-footer-h:         56px;   /* Minimal bottom dual-action footer */
  --xos-touch-min:        44px;   /* Apple HIG minimum touch target */

  /* Color Palette */
  --xos-ink:              #000000;
  --xos-paper:            #FFFFFF;
  --xos-ink-subtle:       #444444;
  --xos-rule:             #000000;
  --xos-rule-subtle:      #E2E2E2;

  /* Geometry & Rules */
  --xos-border-1px:       1px solid var(--xos-ink);
  --xos-border-subtle:    1px solid var(--xos-rule-subtle);
  --xos-radius-subtle:    2px;
}
```

### Vertical Budget Breakdown
- `Y:   0 ..  42` (**42px**): Quiet Masthead (Brand, Clock, Search, Settings, Battery %, Wi-Fi).
- `Y:  42 ..  58` (**16px**): Top Whitespace.
- `Y:  58 .. 426` (**368px**): Centered Editorial Hero Unit (Kicker, Cover, Title, Author, Progress, Continue Button).
- `Y: 426 .. 438` (**12px**): Section Transition & Divider Rule.
- `Y: 438 .. 660` (**222px**): Carefully Composed 3-Book Shelf (Header, 3 Thumbnails, Titles, Authors).
- `Y: 660 .. 744` (**84px**): Luxurious Negative Space Buffer.
- `Y: 744 .. 800` (**56px**): Minimal Footer & Secondary Navigation (`Library (24)` & `Files (SD Card)`).
- **Total: Exactly 800px.** Zero overlapping elements; zero vertical clipping.

---

## 4. Reusable UI Components

### 4.1 Quiet Masthead (`.masthead`)
- **Dimensions:** `480 × 42px`, single 1px solid black bottom hairline rule.
- **Left:** Brandmark `XTEINK` (10.5pt Bold, tracking +0.16em) and static clock (`14:45`).
- **Clock Policy:** Fixed rendering. Updates only on wake, screen refresh, or 1-minute RTC alarm interrupt.
- **Right:** Search icon button (44×44px hit bounds), Settings icon button (44×44px hit bounds), Wi-Fi icon, battery level (86%) and 1-bit battery glyph.

### 4.2 Centered Editorial Hero (`.centered-hero`)
- **Cover Slot:** `116 × 174px` (2:3 aspect ratio). Aspect-fit with optical centering. 1px solid black border with 1px outer bounding frame. Independent touch target (taps view book details).
- **Hero Title:** Centered serif typography (`21pt`, max 2 lines standard; expands to 3 lines for long titles).
- **Hero Author:** Centered serif italic (`13pt`, color: `#444444`).
- **Progress Gauge:** Slender centered track (`140 × 3px`) with text `64% COMPLETED`. Gracefully omitted when progress < 0.
- **Continue Reading Button:** Centered black bar (`210 × 44px`, 11pt uppercase bold white text: `Continue Reading →`). Apple HIG touch-compliant.

### 4.3 Typographic Fallback Book Jacket (`.cover-fallback`)
- Rendered when cover art is missing or for text formats (`.txt`, `.xtc`).
- Clean 1px solid black outer frame with an inner 1px inset rule, centered serif title, centered italic author, and bottom hallmark (`EPUB · COMPLETE` or `TXT · DOCUMENT`).

### 4.4 Carefully Composed Three-Book Shelf (`.shelf-container`)
- Capacity: Capped at **3 books** to preserve generous horizontal margins.
- Header: `Recent Books` (Serif Bold) + right-aligned `All Books (24) →` touch link.
- 3 Columns (`84px` wide each, `50px` gap):
  - Thumbnail: `84 × 126px` (2:3 aspect-fit) with 1px black border.
  - Progress badge: High-contrast solid black micro pill (`31%`, `88%`, `DONE`).
  - Book title (Serif Bold, 1 line truncate).
  - Book author (Serif Italic, 1 line truncate).

### 4.5 Minimal Footer (`.minimal-footer`)
- Replaces heavy application docks with two quiet, discoverable touch actions:
  - `Library (24)` (`208 × 44px`)
  - `Files (SD Card)` (`208 × 44px`)
- Direct access to core storage and collection without stealing reading focus.

---

## 5. Touch Targets & Interaction Independence

- **Independent Hit Bounds:** Cover slot (`116 × 174px`) and Continue Reading button (`210 × 44px`) are spatially distinct and non-overlapping.
- **Apple HIG Compliance:** All interactive buttons and touch links meet or exceed `44 × 44px`.
- **Hardware Button Integration:**
  - `Capacitive Home Key (GT911 bit 0x10)`: Governed by user settings in `HomeButtonSettings.h` (default: Return to Home).
  - `Left Button (GPIO0)`: Navigates previous item on shelf.
  - `Right Button (GPIO7)`: Navigates next item on shelf.
  - `Power Button (GPIO3)`: Sleep / wake toggle.

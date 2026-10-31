# XTEInk OS — Design System v0.5: Design Contract Reconciliation

> **Document ID:** XOS-DS-005  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch + Hardware Keys)  
> **Role:** Lead UI/UX Designer  
> **Status:** Reconciled Specification for Codex Implementation Planning  
> **Upstream Authority:** HOME-005 Codex Technical Audit Findings & CrossPoint In-Tree Codebase

---

## 1. Authoritative Constraints & Architecture Reconciliation

Following the technical audit by Codex on Home v0.4, Design System v0.5 resolves all verified technical inconsistencies while faithfully preserving the approved **Editorial Masthead** composition.

### 1.1 Hardware Baseline & Canvas Geometry
- **Hardware Profile:** Target is locked strictly to **Xteink X4 Pro** (`BoardConfig::XTEINK_X4_PRO`, ESP32-S3, GT911 touch, SSD1677/UC8179/UC8279 batch-detected e-paper).
- **Physical Panel vs. Portrait Canvas:**
  - Physical Glass: `800 × 480` landscape raw hardware scan.
  - UI Canvas: **`480 × 800` portrait pixels** (`swapXY = true` in GT911 touch driver and display driver).
  - All UI elements, layout bounding boxes, and touch regions are defined strictly in this integer `480 × 800` coordinate space.
- **Display Refresh Policy:**
  - Fast partial update (`0xFC`) is the default e-paper refresh mode for interactive navigation and tap feedback.
  - Full refresh (`0xF7`) is executed only on major activity transitions, wake from sleep, or when triggered by the user.
  - **No extra display refresh is executed before navigation:** Tap confirmation uses an immediate in-memory 1-frame inversion (`app.clearTapFlash()`) without triggering an extra waveform cycle before activity exit.

---

## 2. Typography Contract & Font Metrics

### 2.1 Point Sizes at 150 DPI vs. Pixel Heights
The CrossPoint in-tree bitmap fonts (`lib/EpdFont/builtinFonts/`) are **point sizes rasterized at 150 DPI** (`FONT_RASTER_DPI = 150`), **not pixel heights**. 

The verified metrics from the compiled font headers are:

| In-Tree Font | Raster Point Size | Line Advance (`advanceY`) | Ascender | Descender | Total Glyph Span |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`ubuntu_8_regular`** | 8 pt @ 150 DPI | **19 px** | +16 px | -4 px | 20 px |
| **`ubuntu_10_regular` / `bold`** | 10 pt @ 150 DPI | **24 px** | +20 px | -4 px | 24 px |
| **`ubuntu_12_regular` / `bold`** | 12 pt @ 150 DPI | **29 px** | +24 px | -5 px | 29 px |

### 2.2 Diacritic Clearance (Vietnamese Text & Badges)
In Vietnamese and accented scripts, stacked diacritical marks (e.g. `ệ`, `ổ`, `ử`, `ỹ`, `ắ`) extend significantly above standard Latin uppercase ascenders and below the baseline.
- **Rule:** Containers must allocate the full font `advanceY` (or `ascender + abs(descender)` + 2px minimum padding). 
- Single-line labels in Ubuntu 10 require at least **24px** vertical slot height; Ubuntu 12 requires at least **29px**.
- Badges must include at least **4px** vertical padding (`padding: 3px 6px`) to prevent diacritic clipping.

### 2.3 Proposed Display Typography Scale
Sizes larger than 12pt (`14px, 18px, 22px`) are **proposed design tokens for FreeInkUI vector rendering (FreeType)**:
- **`--xos-font-micro`**: Ubuntu 8 (`advanceY: 19px`). Kickers, format badges.
- **`--xos-font-caption`**: Ubuntu 10 (`advanceY: 24px`). Metadata, timestamps, progress percentage.
- **`--xos-font-nav`**: Ubuntu 12 (`advanceY: 29px`). Button labels, author names, shelf links.
- **`--xos-font-body`**: Proposed 14pt (`advanceY: 34px`). Section headers, dialog text.
- **`--xos-font-display`**: Proposed 22pt (`advanceY: 38px`). Hero title (2 lines max in standard view; 3 lines with dynamic container expansion).

> [!IMPORTANT]
> **Truncation Contract:** Replaced all arbitrary character-count truncation with **glyph-width-aware truncation** using `GfxRenderer::getTextWidth()` / `fui::TextStyle.maxLines` with an ellipsis (`...`).

---

## 3. Book Metadata & State Definitions

CrossPoint's underlying stores (`RecentBooksStore`, `library::LibraryIndexFile`, `BookProgress`) do **not** guarantee chapter titles, whole-book pages remaining, or completion metadata together.

### 3.1 Authoritative Metadata Contract
- **Reliably Available:**
  - Book Title: `RecentBook.title` or `record.title` (UTF-8 string).
  - Book Author: `RecentBook.author` or `record.author` (UTF-8 string).
  - Book Format: Inferred from file extension (`.epub`, `.txt`, `.xtc`).
  - Reading Progress: `int progress = loadBookProgress(path)` returns `0 .. 100`, or **`-1` if unavailable**.
- **Graceful Omission Rule:**
  - If `progress >= 0`: The percentage label (`64%`) and 4px solid progress bar are rendered.
  - If `progress < 0`: The progress bar and percentage are **gracefully omitted**; the space is collapsed or displays format metadata (`EPUB`), never displaying a false `0%`.
  - Unverified metadata (such as chapter names or pages remaining) is **excluded** from the standard Home screen to eliminate unverified background parsing requirements.

### 3.2 The Six Explicit Content States
1. **State A — Empty Library:** No books found on SD card or internal flash. Displays 1-bit book icon, headline "Your Library is Waiting", instructions, and `Open File Browser` action button.
2. **State B — Library with Books, No Reading History:** Books exist in library index (e.g. 24 titles), but `RecentBooksStore` is empty. The hero presents a "Start Reading" welcome card with a prominent `Browse Library (24 Titles)` call-to-action, plus 3 books from the library index without progress badges.
3. **State C — Currently Reading (Active Default):** Book exists in `RecentBooksStore` with valid progress (`progress >= 0`). Dominant hero with cover, title, author, progress bar, `Resume Reading` button, and 3 recent shelf books.
4. **State D — Missing or Unavailable Cover Artwork:** EPUB/TXT/XTC book lacks embedded cover artwork. Renders the typographic book jacket fallback (`.cover-fallback`): 1px outer black border, 1px inner inset rule, centered title/author, and format hallmark (`EPUB` / `TXT`).
5. **State E — Long Book Title:** Book title exceeds 2 lines. Title wraps across up to 3 lines with glyph-width-aware ellipsis truncation. Line height respects diacritic ascenders.
6. **State F — Storage / Index Unavailable:** MicroSD card unmounted, I/O error, or index rebuilding. Displays 1-bit warning triangle icon, diagnostic text, and `Retry Storage Mount` action button.

---

## 4. Navigation, Input & Hardware Button Mapping

### 4.1 Touch Targets & Hit Independence
- Every interactive element satisfies the Apple HIG minimum touch bound: **≥ 44 × 44px**.
- **Touch Target Independence:** The Hero Cover and the Resume Button have **completely separate, non-overlapping touch regions**:
  - `Hero Cover Slot`: `Rect{24, 84, 132, 198}` (Action: Open Book Details / Cover Preview).
  - `Resume Button`: `Rect{172, 270, 284, 44}` (Action: Resume Reading in `EpubReaderActivity`).
  - No nested touch triggers.

### 4.2 Hardware Buttons & Capacitive Home Key
- **Hardware Home Button (Capacitive GT911 bit `0x10`):**
  - **Preserved Configurable Contract:** Home button behavior is governed by user settings in `HomeButtonSettings` (`homeButtonTapAction`, `homeButtonDoubleTapAction`, `homeButtonLongPressAction`).
  - **Reconciliation:** Design does **not** hardcode Home-on-Home to full refresh. In default configuration, tap returns to Home from child activities, or triggers the user-selected home action.
- **Page / Nav Buttons (`GPIO0` / `GPIO7`):**
  - Left Button (`GPIO0`): Up / Previous item on shelf.
  - Right Button (`GPIO7`): Down / Next item on shelf.
- **Power Button (`GPIO3`):**
  - Short press toggles sleep / wake. Long press enters deep standby.
- **Library Search:**
  - Tapping the `Search` button in the Masthead directly routes to `LibraryActivity` with the search input focused, leveraging CrossPoint's verified in-tree library search.

### 4.3 Logical Hardware Focus Sequence
For future physical button navigation, focus traverses predictably:
1. `Masthead Search` (`Rect{278, 0, 44, 44}`)
2. `Masthead Settings` (`Rect{324, 0, 44, 44}`)
3. `Hero Cover` (`Rect{24, 84, 132, 198}`)
4. `Resume Reading Button` (`Rect{172, 270, 284, 44}`)
5. `Shelf Header Link ("All Books")` (`Rect{330, 360, 126, 44}`)
6. `Shelf Item 1` (`Rect{40, 406, 84, 164}`)
7. `Shelf Item 2` (`Rect{198, 406, 84, 164}`)
8. `Shelf Item 3` (`Rect{356, 406, 84, 164}`)
9. `Ribbon Library Button` (`Rect{24, 750, 208, 44}`)
10. `Ribbon Files Button` (`Rect{248, 750, 208, 44}`)

---

## 5. Display Refresh & Cover Scaling Policy

### 5.1 Cover Aspect Ratio Handling (Aspect-Fit with Optical Centering)
- Slot dimensions: Hero Cover is `132 × 198px` (2:3 aspect ratio); Shelf Thumbnails are `84 × 126px` (2:3 aspect ratio).
- **Policy:** **Aspect-Fit without distortion (`GUI.drawCoverThumbFill`)**:
  - The cover bitmap is scaled proportionally to fit entirely inside the 2:3 bounding box.
  - If a cover is narrower than 2:3, it is centered horizontally within the 1px black bounding frame.
  - No cropping of title/cover art; no non-uniform stretching.

### 5.2 1-Bit Contrast Supremacy
- All UI chrome, text, dividers, and progress tracks use 100% black ink (`#000000`) on 100% white paper (`#FFFFFF`).
- Grayscale contrast modes in browser prototypes are visual simulations only; the production firmware renders binary 1-bit primitives with hardware dithering where appropriate.

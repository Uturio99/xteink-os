# XTEInk OS — Home Screen Engineering Specification v1.0

> **Document ID:** XOS-SPEC-HOME-010  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch + Hardware Keys)  
> **Design Authority:** Product Owner Approved Visual North Star (HOME-006)  
> **Role:** Lead Visual Designer  
> **Status:** Production-Ready Engineering Specification for Codex Implementation & Product Owner Sign-off  
> **Upstream Authority:** Visual North Star v1.0 & CrossPoint In-Tree Font Audits  

---

## 1. Architectural Alignment: Centered Editorial Hero

Specification v1.0 marks the formal design contract for the **Product Owner Approved Visual North Star**: **Premium Editorial Minimalism × Apple HIG × Durobo Krono**.

### 1.1 Architectural Shift from v0.5 to v1.0
- **Compositional Pivot:** Supersedes the previous asymmetrical horizontal split (v0.5 side-by-side cover and text block) with a **strong, centered editorial hero unit**.
- **Visual Weight & Immersion:** The book cover (`116 × 174px`) is centered directly above the dominant book title and expressive italic author credit, establishing a commanding, magazine-style focal point.
- **Progress Gauge Refinement:** Replaces bulky progress bars with a slender, centered 3px hairline track (`140 × 3px`) paired with a high-contrast percentage label (`64% COMPLETED`).
- **Ergonomic Action Button:** A centered, high-contrast action button (`210 × 44px`, `Continue Reading →`) with minimum 44px touch height meeting Apple HIG guidelines.
- **Three-Book Secondary Shelf:** A carefully composed horizontal row capped at 3 books (`84 × 126px` covers), preserving ample negative space.
- **Quiet Masthead & Minimal Dual Footer:** Restrained 42px top bar and 56px dual-action footer (`Library` & `Files`), completely free of cluttered 5-item mobile app docks.

---

## 2. Master Layout Coordinate Table (Internally Reconciled)

The target canvas is **exactly `480 × 800` portrait pixels** (`X: 0 .. 479`, `Y: 0 .. 799`). The X4 Pro panel glass is 800×480 landscape; firmware maps this to 480×800 portrait via display rotation and GT911 touch coordinate mapping (`swapXY = true`).

Horizontal safe margin: `24px` left and right (`X: 24` to `X: 456`, usable width = `432px`).

| UI Element | Bounding Box (`X, Y, W, H`) | Typography & Styling | Touch Hit Bounds | Firmware Action / Event Route |
| :--- | :--- | :--- | :--- | :--- |
| **Quiet Masthead Band** | `Rect{0, 0, 480, 42}` | 1px solid black bottom hairline | — | Static top system band |
| • Bottom Hairline | `(0, 41)` to `(479, 41)` | 1px solid black (`#000000`) | — | Visual separation rule |
| • Brandmark ("XTEINK") | `Rect{24, 11, 56, 20}` | Ubuntu 10 Bold (tracking `+0.16em`) | — | Visual brand anchor |
| • Static Clock ("14:45") | `Rect{84, 11, 48, 20}` | Ubuntu 10 Regular (`advanceY: 24px`) | — | Updates on wake / 1-min RTC alarm |
| • Search Action Button | `Rect{278, 0, 44, 42}` | 19×19px magnifying icon (1.7px stroke) | `Rect{278, 0, 44, 42}` | Pushes `LibraryActivity` (search focus) |
| • Settings Action Button | `Rect{324, 0, 44, 42}` | 19×19px gear icon (1.7px stroke) | `Rect{324, 0, 44, 42}` | Pushes `SettingsActivity` |
| • Wi-Fi Indicator | `Rect{374, 14, 16, 14}` | 1-bit vector / bitmap icon | — | System Wi-Fi RSSI |
| • Battery Percent ("86%") | `Rect{394, 11, 32, 20}` | Ubuntu 10 Bold | — | System battery percentage |
| • Battery Icon | `Rect{430, 14, 26, 14}` | 1-bit battery glyph (fill = 86%) | — | Hardware fuel gauge |
| **Top Spacing Buffer** | `Rect{0, 42, 480, 16}` | Pure White (`#FFFFFF`) | — | Whitespace buffer (16px) |
| **Centered Editorial Hero** | `Rect{24, 58, 432, 368}` | — | — | Active reading focal unit |
| • Section Kicker | `Rect{24, 58, 432, 19}` | Ubuntu 8 Bold, uppercase, centered | — | "CURRENTLY READING" label |
| • Hero Cover Slot (2:3) | `Rect{182, 78, 116, 174}` | 1px solid black border + 1px frame | `Rect{182, 78, 116, 174}` | Launches Book Details / Cover Modal |
| • Hero Title (Standard 2 lines) | `Rect{40, 260, 400, 52}` | Serif Bold 21pt (`advanceY: 26px`), centered | — | Dominant focal anchor (max 2 lines) |
| • Hero Title (State E Long Title) | `Rect{40, 260, 400, 76}` | Serif Bold 21pt (`advanceY: 26px`), centered | — | Wraps up to 3 lines (e.g. Vietnamese) |
| • Hero Author Credit | `Rect{40, 314, 400, 20}` | Serif Italic 13pt (`#444444`), centered | — | Author metadata (1-line truncate) |
| • Progress Label ("64% COMPLETED") | `Rect{170, 338, 140, 14}` | Ubuntu 10 Bold (`advanceY: 14px`), centered | — | Omitted gracefully if progress < 0 |
| • Hairline Progress Track | `Rect{170, 354, 140, 3}` | 1px black outline, solid fill | — | Visual progress indicator |
| • Continue Reading Button | `Rect{135, 368, 210, 44}` | Ubuntu 10/12 Bold (`#FFFFFF`), solid black | `Rect{135, 368, 210, 44}` | Launches `EpubReaderActivity` |
| **Section Divider Rule** | `(24, 426)` to `(456, 426)` | 1px hairline rule (`#000000` / `#E2E2E2`) | — | Subtle zone boundary |
| **Carefully Composed Shelf** | `Rect{24, 438, 432, 222}` | — | — | 3-book secondary continuity row |
| • Shelf Section Title | `Rect{24, 438, 180, 24}` | Serif Bold 13pt (`advanceY: 22px`) | — | "Recent Books" |
| • Shelf Header Link | `Rect{330, 428, 126, 44}` | Ubuntu 10 Regular, right-aligned | `Rect{330, 428, 126, 44}` | Pushes `LibraryActivity` |
| • Shelf Column 1 (Cover + Meta) | `Rect{42, 472, 84, 170}` | 84×126px thumb + Serif Title & Author | `Rect{42, 472, 84, 170}` | Launches Column 1 Book |
| • Shelf Column 2 (Cover + Meta) | `Rect{198, 472, 84, 170}` | 84×126px thumb + Serif Title & Author | `Rect{198, 472, 84, 170}` | Launches Column 2 Book |
| • Shelf Column 3 (Cover + Meta) | `Rect{354, 472, 84, 170}` | 84×126px thumb + Serif Title & Author | `Rect{354, 472, 84, 170}` | Launches Column 3 Book |
| **Luxurious Whitespace Buffer** | `Rect{24, 660, 432, 84}` | Pure White (`#FFFFFF`) | — | Intentional negative space (84px) |
| **Minimal Dual Footer** | `Rect{0, 744, 480, 56}` | 1px solid black top rule | — | Bottom utility navigation zone |
| • Top Border Hairline | `(0, 744)` to `(479, 744)` | 1px solid black (`#000000`) | — | Footer boundary rule |
| • Library Action Button | `Rect{24, 750, 208, 44}` | Ubuntu 12 Bold (`advanceY: 29px`) | `Rect{24, 750, 208, 44}` | Pushes `LibraryActivity` |
| • Files Action Button | `Rect{248, 750, 208, 44}` | Ubuntu 12 Bold (`advanceY: 29px`) | `Rect{248, 750, 208, 44}` | Pushes `FileBrowserActivity` |

**Verification of Vertical Geometry:**
- Masthead: `42px` (`0 .. 42`)
- Top Whitespace: `16px` (`42 .. 58`)
- Hero Unit: `368px` (`58 .. 426`)
- Divider Margin: `12px` (`426 .. 438`)
- Shelf Unit: `222px` (`438 .. 660`)
- Negative Space Buffer: `84px` (`660 .. 744`)
- Minimal Footer: `56px` (`744 .. 800`)
- **Total: Exactly 800px.** Zero pixel drift, zero overlapping containers.

---

## 3. Typography & Vietnamese Diacritic Clearance

### 3.1 Font Asset Requirements & In-Tree Metrics
CrossPoint compiles bitmap fonts rasterized at **150 DPI**.

```
[--- Ascender Span ---]   +16px (U8)   / +20px (U10)  / +24px (U12)
=======================   BASELINE (Y = cursor pos)
[--- Descender Span ---]  -4px  (U8)   / -4px  (U10)  / -5px  (U12)
-----------------------
Total Required Slot:      20px  (U8)   / 24px  (U10)  / 29px  (U12)
```

To achieve the approved Visual North Star aesthetic:
1. **In-Tree Verified UI Fonts:**
   - `Ubuntu 8 Regular`: Kicker, micro metadata pills.
   - `Ubuntu 10 Regular / Bold`: Clock, battery, progress stats, search/settings labels.
   - `Ubuntu 12 Bold`: Continue Reading button text, footer button text.
2. **Required High-Fidelity Serif Font Assets (Codex Action):**
   - `Serif Display Bold 21pt` (`advanceY: 26px`, line advance 26px): Hero title.
   - `Serif Accent Italic 13pt` (`advanceY: 20px`): Hero author credit.
   - `Serif Subhead Bold 13pt` (`advanceY: 22px`): Shelf section heading.
   - `Serif Body Bold / Italic 10pt` (`advanceY: 14px`): Shelf thumbnails metadata.
   - *Candidate Families:* `Literata`, `Merriweather`, `Noto Serif`.

### 3.2 Stacked Diacritic Clearance & Truncation Algorithm
- **Vietnamese Diacritic Safeguard:** Vietnamese titles containing upper and lower diacritics (e.g. `ẩ, ễ, ổ, ự, ỹ, đ`) require full vertical clearance. Hero title line-height is set to **`26px`** per line, providing at least `+20px` ascender and `-5px` descender space without clipping.
- **Pixel-Advance Truncation:** Character-count truncation (e.g. `title.substr(0, 32)`) is strictly forbidden. Firmware must measure rendered pixel width using:
  ```cpp
  int maxWidth = 400; // Hero title bounding box width
  int ellipsisWidth = GfxRenderer::getTextWidth(fontSerif21, "...");
  if (GfxRenderer::getTextWidth(fontSerif21, text) > maxWidth) {
      // Truncate glyph by glyph until text + "..." <= maxWidth
  }
  ```

---

## 4. Aspect-Fit Image Scaling & Cover Handling

### 4.1 Target Slots & Aspect Ratio
- **Hero Cover Frame:** `116 × 174px` (2:3 standard e-reader aspect ratio).
- **Shelf Thumbnails:** `84 × 126px` (2:3 standard e-reader aspect ratio).
- **Scaling Rule:** Firmware must use aspect-fit with optical centering:
  - If cover aspect > 2:3 (wider), fit to width and center vertically.
  - If cover aspect < 2:3 (taller), fit to height and center horizontally.
  - Border: 1px solid black frame (`#000000`) enclosing the image canvas.

### 4.2 Typographic Fallback Jacket (`.cover-fallback`)
Rendered when cover artwork is missing, unreadable, or for text documents (`.txt`, `.xtc`):
- **Hero Fallback (`116 × 174px`):**
  - Outer border: 1px solid black.
  - Inset frame: `Rect{188, 84, 104, 162}` (1px hairline rule, 6px inset).
  - Title: Centered serif text (max 3 lines), 10pt Bold.
  - Horizontal divider: 1px solid black hairline.
  - Author: Centered serif italic text, 8pt.
  - Format Hallmark: Bottom micro-caps text `EPUB · COMPLETE` or `TXT · DOCUMENT`.
- **Shelf Fallback (`84 × 126px`):**
  - Outer border: 1px solid black.
  - Centered book title in 9pt serif bold, truncated to 2 lines.
  - Format indicator `EPUB` or `TXT` at bottom.

---

## 5. Exhaustive Content State Specifications (A through F)

### State A: Empty Library
- **Condition:** `LibraryIndexFile::bookCount() == 0` and SD card contains zero recognized book files.
- **Layout Behavior:** Hero and shelf containers are replaced by a centered empty state stage (`Rect{24, 120, 432, 520}`):
  - Emblem: 1-bit line art Book Emblem (`48 × 48px`) centered at `X = 216, Y = 240`.
  - Heading: "Your Library is Waiting" (Serif 18pt Bold), centered at `Y = 312`.
  - Body Copy: "No books found. Connect via USB or Wi-Fi to transfer your EPUB or TXT books, or insert a MicroSD card.", centered at `Y = 348` (max width 340px, line height 22px).
  - Primary Action Button: `Rect{140, 424, 200, 44}`, black fill, white text `Open File Browser`. Action: Pushes `FileBrowserActivity`.
  - Masthead and Minimal Footer remain visible and active.

### State B: Books Available in Library, No Reading History
- **Condition:** `LibraryIndexFile::bookCount() > 0`, but `RecentBooksStore::getBooks().empty()`.
- **Layout Behavior:**
  - Hero Cover Slot displays a generic library welcome book jacket.
  - Title: "Welcome to XTEInk OS" (Serif 21pt Bold).
  - Author / Subtitle: "Select a book to begin reading" (Serif 13pt Italic).
  - Progress Gauge: Gracefully omitted (no track, no label).
  - Hero Action Button: `Rect{135, 368, 210, 44}`, text `Browse Library (24) →`. Action: Pushes `LibraryActivity`.
  - Shelf: Populated with the first 3 indexed books from the library index. Progress badges are omitted.

### State C: Currently Reading (Default Visual North Star)
- **Condition:** `RecentBooksStore::getBooks().size() > 0` with `loadBookProgress(path) >= 0`.
- **Layout Behavior:**
  - Hero Cover Slot: Displays decoded cover art (`116 × 174px`).
  - Title: Displayed in 21pt Serif Bold (e.g. *The Left Hand of Darkness*).
  - Author: Displayed in 13pt Serif Italic (e.g. *Ursula K. Le Guin*).
  - Progress Unit: Shows `64% COMPLETED` with 3px hairline track (`140 × 3px`).
  - Action Button: `Continue Reading →` launches active book in `EpubReaderActivity`.
  - Shelf: Renders 3 most recent secondary books with individual progress badges (`31%`, `88%`, `DONE`).

### State D: Missing or Unavailable Cover Artwork
- **Condition:** Reading history exists, but book lacks embedded cover artwork or cover extraction failed.
- **Layout Behavior:**
  - Active book metadata, progress meter, and Continue Reading button render identically to State C.
  - Cover slot displays the **Typographic Fallback Book Jacket** with ornamental inset border, typeset title, author, and `EPUB · COMPLETE` hallmark.

### State E: Long Title / Vietnamese Diacritics
- **Condition:** Book title exceeds 2 lines of pixel advance, or contains stacked Vietnamese diacritics (e.g. *Nỗi Buồn Chiến Tranh* by *Bảo Ninh*).
- **Layout Behavior:**
  - Hero Title expands from 52px (2 lines) to 76px (3 lines).
  - Vertical spacing adjusts dynamically: Author subtitle shifts downward by 14px to `Y = 328`.
  - Progress unit and Continue Reading button retain their fixed positions (`Y = 338` and `Y = 368`), absorbing the 14px difference from the internal whitespace buffer.
  - Diacritical marks (`ệ, ổ, ử, ỹ, ắ`) render with zero clipping due to `26px` line advance.
  - Titles exceeding 3 full lines truncate with trailing ellipsis `...`.

### State F: Storage / Index Unavailable
- **Condition:** SD card unmounted, I/O read error, or index file corrupted.
- **Layout Behavior:**
  - Hero and shelf containers are replaced by an error stage (`Rect{24, 120, 432, 520}`):
  - Warning Icon: 1-bit triangle warning glyph (`48 × 48px`) centered at `X = 216, Y = 240`.
  - Heading: "Storage Unavailable" (Serif 18pt Bold), centered at `Y = 312`.
  - Body Copy: "Unable to read storage volume. Verify that your MicroSD card is securely inserted and formatted as FAT32.", centered at `Y = 348`.
  - Action Button: `Rect{140, 424, 200, 44}`, black fill, white text `Retry Mount`. Action: Retries SD mount and triggers background index scan.

---

## 6. Input Mapping & Navigation Routing

### 6.1 Touch Targets & Hit Independence
1. **Masthead Search:** `Rect{278, 0, 44, 42}` -> Launches `LibraryActivity` with search field focused.
2. **Masthead Settings:** `Rect{324, 0, 44, 42}` -> Launches `SettingsActivity`.
3. **Hero Cover Slot:** `Rect{182, 78, 116, 174}` -> Launches Book Details Modal.
4. **Continue Reading Button:** `Rect{135, 368, 210, 44}` -> Launches `EpubReaderActivity` at saved reading position.
   - *Spatial Independence:* The cover slot and button are separated by `116px` of vertical space; touch events cannot trigger ambiguous activation.
5. **Shelf Header Link:** `Rect{330, 428, 126, 44}` -> Launches `LibraryActivity`.
6. **Shelf Book Columns:**
   - Column 1: `Rect{42, 472, 84, 170}` -> Launches Column 1 Book.
   - Column 2: `Rect{198, 472, 84, 170}` -> Launches Column 2 Book.
   - Column 3: `Rect{354, 472, 84, 170}` -> Launches Column 3 Book.
7. **Footer Actions:**
   - Library: `Rect{24, 750, 208, 44}` -> Launches `LibraryActivity`.
   - Files: `Rect{248, 750, 208, 44}` -> Launches `FileBrowserActivity`.

### 6.2 Hardware Buttons Integration
- **Capacitive Home Key (`GT911 bit 0x10`):**
  - Governed by user preferences in `HomeButtonSettings.h`. Default: Return to Home Screen.
  - **Do NOT trigger full display refresh** on Home-on-Home press.
- **Physical Left Key (`GPIO0`):**
  - Navigates focus to previous book on shelf.
- **Physical Right Key (`GPIO7`):**
  - Navigates focus to next book on shelf.
- **Power Key (`GPIO3`):**
  - Instant sleep / wake toggle.

---

## 7. Engineering Validation Items for Codex

Codex must review and validate the following architectural points during firmware integration:
1. **Serif Font Implementation Path:**
   - *Option A (Preferred):* Utilize FreeType vector font engine (already embedded in CrossPoint for EPUB reading) to dynamically render UI headings at `21pt` and `13pt`.
   - *Option B (Alternative):* Generate pre-compiled bitmap tables (`literata_21_bold.h`, `literata_13_italic.h`) using CrossPoint's font generation toolchain.
2. **GT911 Touch Edge Clearance:**
   - Verify that touches along the masthead top (`Y: 0 .. 42`) and footer bottom (`Y: 750 .. 794`) register accurately without edge capacitance dead zones on X4 Pro glass.
3. **Cover Thumbnail Caching:**
   - Ensure `HomeCoverCache` extracts and caches thumbnails at `116 × 174px` (Hero) and `84 × 126px` (Shelf) during initial library indexing to avoid on-the-fly JPEG decoding overhead.

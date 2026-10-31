# XTEInk OS — Home Screen Engineering Specification v0.5

> **Document ID:** XOS-SPEC-HOME-005  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch)  
> **Role:** Lead UI/UX Designer  
> **Status:** Production-Ready Engineering Specification for Codex Implementation  
> **Upstream Authority:** HOME-005 Codex Technical Reconciliation & In-Tree Font Metrics

---

## 1. Authoritative Coordinate Grid & Canvas Geometry

- **Target Canvas:** Exactly **`480 × 800` portrait pixels** (`X: 0 .. 479`, `Y: 0 .. 799`).
- **Physical Panel Relationship:** The X4 Pro panel glass is 800×480 landscape. The display driver and GT911 touch driver map this to 480×800 portrait via `swapXY = true`.
- **Horizontal Margins:** Screen edge safe insets are `24px` on left and right (`X: 24` to `X: 456`, usable width = `432px`).
- **No Overlapping Touch Targets:** The hero cover and the resume button have distinct, non-overlapping bounding rects.

---

## 2. Master Layout Coordinate Table (Internally Reconciled)

| UI Element | Bounding Box (`X, Y, W, H`) | Font & Verified Metrics | Touch Hit Bounds | Firmware Action / Event |
| :--- | :--- | :--- | :--- | :--- |
| **Editorial Masthead Band** | `Rect{0, 0, 480, 44}` | — | — | Static header band |
| • Bottom Rule | `(0, 43)` to `(479, 43)` | 1px solid black | — | Screen division |
| • Brandmark ("XTEINK") | `Rect{24, 10, 60, 24}` | Ubuntu 10 Bold (`advanceY: 24px`) | — | Visual context |
| • Static Clock ("14:45") | `Rect{88, 10, 48, 24}` | Ubuntu 10 Regular (`advanceY: 24px`) | — | Updates on wake / 1-min RTC event |
| • Search Button | `Rect{278, 0, 44, 44}` | 19×19px icon (1.7px stroke) | `Rect{278, 0, 44, 44}` | Pushes `LibraryActivity` (search focus) |
| • Settings Button | `Rect{324, 0, 44, 44}` | 19×19px icon (1.7px stroke) | `Rect{324, 0, 44, 44}` | Pushes `SettingsActivity` |
| • Wi-Fi & Battery Cluster | `Rect{372, 12, 84, 20}` | Ubuntu 10 Bold | — | System battery & Wi-Fi level |
| **Currently Reading Hero** | `Rect{24, 58, 432, 274}` | — | — | Active book reading stage |
| • Kicker ("CURRENTLY READING") | `Rect{24, 58, 120, 19}` | Ubuntu 8 Bold (`advanceY: 19px`) | — | Section label |
| • Kicker Hairline | `(148, 67)` to `(456, 67)` | 1px subtle rule | — | Visual separation |
| • Hero Cover Slot (2:3) | `Rect{24, 84, 132, 198}` | 1px solid black border | `Rect{24, 84, 132, 198}` | Opens Book Details / Cover Preview |
| • Hero Title (Standard 2 lines) | `Rect{172, 84, 284, 58}` | 22pt Display (`advanceY: 28px`) | — | Truncates with ellipsis if > 2 lines |
| • Hero Title (Long Title 3 lines)| `Rect{172, 84, 284, 84}` | 22pt Display (`advanceY: 28px`) | — | Truncates with ellipsis if > 3 lines |
| • Hero Author | `Rect{172, 146, 284, 24}` | Ubuntu 10 Bold (`advanceY: 24px`) | — | Author metadata (1-line truncate) |
| • Reading Progress Meter | `Rect{172, 178, 284, 38}` | Ubuntu 10 Bold + 4px track | — | Omitted gracefully if progress < 0 |
| • "Resume Reading →" Button | `Rect{172, 238, 284, 44}` | Ubuntu 12 Bold (`advanceY: 29px`) | `Rect{172, 238, 284, 44}` | Launches `EpubReaderActivity` |
| **Section Divider Rule** | `(24, 340)` to `(456, 340)` | 1px hairline rule | — | Separates hero from shelf |
| **Recent Books Shelf** | `Rect{24, 356, 432, 214}` | — | — | 3-column horizontal shelf |
| • Shelf Header Title | `Rect{24, 358, 160, 24}` | Proposed 14pt / Ubuntu 12 Bold | — | "Recent on Shelf" |
| • Shelf Header Link | `Rect{330, 348, 126, 44}` | Ubuntu 10 Regular (`advanceY: 24px`) | `Rect{330, 348, 126, 44}` | Pushes `LibraryActivity` |
| • Shelf Col 1 (Cover + Meta) | `Rect{40, 396, 84, 170}` | 84×126px thumb + Ubuntu 10/8 | `Rect{40, 396, 84, 170}` | Launches Col 1 Book |
| • Shelf Col 2 (Cover + Meta) | `Rect{198, 396, 84, 170}` | 84×126px thumb + Ubuntu 10/8 | `Rect{198, 396, 84, 170}` | Launches Col 2 Book |
| • Shelf Col 3 (Cover + Meta) | `Rect{356, 396, 84, 170}` | 84×126px thumb + Ubuntu 10/8 | `Rect{356, 396, 84, 170}` | Launches Col 3 Book |
| **Airy Whitespace Buffer** | `Rect{24, 570, 432, 174}` | Pure White (`#FFFFFF`) | — | Contemplative negative space |
| **Bottom Action Ribbon** | `Rect{0, 744, 480, 56}` | — | — | Bottom navigation zone |
| • Top Rule | `(0, 744)` to `(479, 744)` | 1px solid black | — | Zone division |
| • `Library (24)` Button | `Rect{24, 750, 208, 44}` | Ubuntu 12 Bold (`advanceY: 29px`) | `Rect{24, 750, 208, 44}` | Pushes `LibraryActivity` |
| • `Files (SD Card)` Button | `Rect{248, 750, 208, 44}` | Ubuntu 12 Bold (`advanceY: 29px`) | `Rect{248, 750, 208, 44}` | Pushes `FileBrowserActivity` |

---

## 3. Font Metrics & Diacritic Clearance

CrossPoint rasterizes bitmap fonts at **150 DPI**. To guarantee Vietnamese diacritical marks (e.g., `ệ`, `ổ`, `ử`, `ỹ`) and Latin ascenders/descenders are never clipped, layouts must respect the following compiled font boundaries:

```
[--- Ascender Span ---]   +16px (U8)   / +20px (U10)  / +24px (U12)
=======================   BASELINE (Y = cursor pos)
[--- Descender Span ---]  -4px  (U8)   / -4px  (U10)  / -5px  (U12)
-----------------------
Total Required Slot:      20px  (U8)   / 24px  (U10)  / 29px  (U12)
```

- **Truncation Algorithm:** Layout uses `GfxRenderer::getTextWidth(font, text)` to measure pixel advance. If advance exceeds bounding box width minus ellipsis width, the string is truncated glyph-by-glyph until `text + "..."` fits. Character counts (e.g. "32 chars") are superseded.

---

## 4. Content State Specifications (A through F)

### State A: Empty Library
- **Trigger:** `LibraryIndexFile::bookCount() == 0` and SD card contains zero recognized book files.
- **Visuals:** Main content stage replaced by `Rect{24, 80, 432, 600}`:
  - 1-bit Book Emblem (48×48px) at `(216, 260)`.
  - Title: "Your Library is Waiting" (Ubuntu 12 Bold / 18pt), centered at `Y = 328`.
  - Explanatory copy: "No books found. Connect via USB or Wi-Fi to transfer your EPUB or TXT books, or insert a MicroSD card.", centered at `Y = 360`.
  - Action Button: `Rect{140, 430, 200, 44}`, text `Open File Browser` (Ubuntu 12 Bold). Action: Pushes `FileBrowserActivity`.

### State B: Library with Books, No Reading History
- **Trigger:** `LibraryIndexFile::bookCount() > 0`, but `RecentBooksStore::getBooks().empty()`.
- **Visuals:**
  - Hero section renders a welcoming "Start Reading" feature.
  - Right stack displays: "Welcome to XTEInk OS", author "Select a book to begin", and primary button `Browse Library (24 Titles)` (`Rect{172, 238, 284, 44}`).
  - Shelf renders the first 3 indexed books from the library index without progress badges.

### State C: Currently Reading (Active Default)
- **Trigger:** `RecentBooksStore::getBooks().size() > 0` with `loadBookProgress(path) >= 0`.
- **Visuals:** Dominant hero book with decoded cover art, display title, author, progress percentage (`64%`) and 4px solid progress bar. Resume button launches reader directly. 3 recent books on shelf.

### State D: Missing or Unavailable Cover Artwork
- **Trigger:** Book file exists, but no embedded cover art found (or format is `.txt`/`.xtc`).
- **Visuals:** Hero cover slot `Rect{24, 84, 132, 198}` paints `coverFallback`:
  - Outer border: 1px solid black.
  - Inner inset rule: `Rect{32, 92, 116, 182}`.
  - Centered title (Ubuntu 10 Bold), rule, centered author (Ubuntu 10 Regular), format hallmark (`EPUB` / `TXT`).

### State E: Long Book Title
- **Trigger:** Book title exceeds 2 lines of text advance (e.g. *The Left Hand of Darkness: 50th Anniversary Commemorative Expanded Edition with Critical Essays*).
- **Visuals:** Title wraps up to 3 lines (`Rect{172, 84, 284, 84}`). The author slot shifts downward to `Y: 172`, maintaining 10px breathing space above the progress block. The Resume button remains anchored at `Y: 238 .. 282`. Text exceeding 3 lines truncates with trailing ellipsis.

### State F: Storage / Index Unavailable
- **Trigger:** SD card I/O failure, card unmounted, or index rebuild failed.
- **Visuals:**
  - Warning Icon (48×48px triangle) at `(216, 260)`.
  - Title: "SD Card Unavailable" (Ubuntu 12 Bold / 18pt), centered at `Y = 328`.
  - Explanatory copy: "Unable to read storage volume. Verify that your MicroSD card is securely inserted and formatted as FAT32.", centered at `Y = 360`.
  - Action Button: `Rect{140, 430, 200, 44}`, text `Retry Storage Mount`. Action: Retries SD mount and index rebuild.

---

## 5. Input Mapping & Navigation Routing

1. **Touch Navigation:**
   - Single tap on `Resume Reading` (`Rect{172, 238, 284, 44}`): Opens active book in `EpubReaderActivity`.
   - Single tap on `Hero Cover Slot` (`Rect{24, 84, 132, 198}`): Opens Book Details.
   - Single tap on `Search` (`Rect{278, 0, 44, 44}`): Routes directly to `LibraryActivity` with search filter input active.
   - Single tap on `Settings` (`Rect{324, 0, 44, 44}`): Launches `SettingsActivity`.
   - Single tap on `Library (24)` (`Rect{24, 750, 208, 44}`): Launches `LibraryActivity`.
   - Single tap on `Files (SD Card)` (`Rect{248, 750, 208, 44}`): Launches `FileBrowserActivity`.
2. **Hardware Home Button (GT911 Capacitive bit `0x10`):**
   - Retains configurable behavior per `HomeButtonSettings.h`. Does **not** force an automatic full display refresh.
3. **Physical Buttons (`GPIO0` / `GPIO7`):**
   - Left Button (`GPIO0`): Cycles previous book on shelf.
   - Right Button (`GPIO7`): Cycles next book on shelf.

---

## 6. Assumptions Requiring Codex Engineering Verification

1. **FreeType Display Title Support:** Design specifies a `22pt` display title (`advanceY: 28px`). Codex to verify whether FreeInkUI will rasterize this dynamically via FreeType or whether pre-compiled bitmap tables (`ubuntu_22_bold.h`) should be generated.
2. **Cover Downscaling Pipeline:** Cover art is scaled to `132 × 198px` (Hero) and `84 × 126px` (Shelf) via aspect-fit with optical centering. Codex to confirm whether `HomeCoverCache` generates these thumbnails during indexing or scales dynamically at render time.

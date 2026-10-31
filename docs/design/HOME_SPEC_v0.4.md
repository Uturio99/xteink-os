# XTEInk OS — Home Screen Engineering Specification v0.4

> **Document ID:** XOS-SPEC-HOME-004  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas, Capacitive Touch)  
> **Status:** Production Specification for Codex Implementation & Product Owner Review  
> **Author:** Lead Product & UI Designer  

---

## 1. Overview & Canvas Coordinate System

All coordinates, bounds, and layout dimensions are defined in **exact integer pixels** within the portrait UI canvas:
- **Canvas Resolution:** `480 × 800` pixels (`X: 0 .. 479`, `Y: 0 .. 799`).
- **Physical Panel Relationship:** The X4 Pro panel glass is 800×480 landscape. The display driver and GT911 touch driver map this to 480×800 portrait via `swapXY = true`.
- **Origin:** `(X: 0, Y: 0)` is the top-left corner of the portrait screen.
- **Horizontal Safe Inset:** `24px` on the left and right (`X: 24` to `X: 456`, usable content width = `432px`).

---

## 2. Layout Bands & Coordinate Mapping

```
Y = 0   +--------------------------------------------------------+
        | EDITORIAL MASTHEAD (Height: 44px)                       |
        | Brandmark (X:24) | Clock | Search (X:278) | Settings   |
Y = 44  +========================================================+ 1px Solid Black Rule
        | CONTENT STAGE (Y: 44 to 744, Height: 700px)            |
        |                                                        |
        | Section Kicker (Y: 58)                                 |
        |                                                        |
        | CURRENTLY READING HERO (Y: 76 to 280)                  |
        | [Cover 132x198]  Title (22px, 3 lines max)             |
        | (X: 24 to 156)   Author (12px)                         |
        |                  Progress Track & Stats                |
        |                  Resume Reading Button (H: 44px)       |
        |                                                        |
Y = 296 | - - - - - - - - - - - - - - - - - - - - - - - - - - - -| 1px Subtle Rule
        |                                                        |
        | RECENT BOOKS SHELF (Y: 310 to 520)                     |
        | "Recent on Shelf" (X:24)       "All Books (24) ->"     |
        | [Book 1 (80x120)]  [Book 2 (80x120)]  [Book 3 (80x120)] |
        |                                                        |
Y = 744 +========================================================+ 1px Solid Black Rule
        | BOTTOM ACTION RIBBON (Height: 56px)                    |
        | [ Library (24) ] (X:24)      [ Files (SD Card) ](X:248)|
Y = 800 +--------------------------------------------------------+
```

---

## 3. Component Dimensions & Coordinate Specifications

### 3.1 Editorial Masthead
- **Bounding Box:** `Rect{0, 0, 480, 44}`.
- **Background:** Solid White (`#FFFFFF`).
- **Bottom Separator Rule:** Line from `(0, 43)` to `(479, 43)`, 1px solid black.
- **Elements:**
  - **Brandmark ("XTEINK"):** `Rect{24, 12, 60, 20}`, font: Ubuntu 10 Bold (tracking +0.14em, uppercase).
  - **Clock ("14:45"):** `Rect{88, 14, 48, 16}`, font: Ubuntu 10 Regular, color: `#444444`.
    - *Update Policy:* Updated on screen wake, activity entry, or discrete 1-minute RTC alarm interrupt. No continuous timer polling.
  - **Search Button:** Touch hit rect: `Rect{278, 0, 44, 44}`. Center icon: 19×19px magnifying glass (1.7px stroke). Action: Pushes `SearchActivity`.
  - **Settings Button:** Touch hit rect: `Rect{324, 0, 44, 44}`. Center icon: 19×19px gear (1.7px stroke). Action: Pushes `SettingsActivity`.
  - **Status Indicators (Wi-Fi + Battery):** `Rect{372, 12, 84, 20}`.
    - Wi-Fi icon: 13×10px at `(372, 17)`.
    - Battery percentage label: `Rect{390, 14, 28, 16}`, font: Ubuntu 10 Bold.
    - Battery glyph: `Rect{422, 16, 17, 9}`, 1px black outline, solid black interior fill proportional to charge.

### 3.2 Currently Reading Hero
- **Bounding Box:** `Rect{24, 58, 432, 230}`.
- **Section Kicker:** `Rect{24, 58, 100, 12}`, text: `CURRENTLY READING` (Ubuntu 8 Bold, uppercase). Hairline rule extends from `X: 132` to `X: 456` at `Y: 64`.
- **Book Cover Slot:**
  - **Position:** `Rect{24, 78, 132, 198}` (aspect ratio 2:3).
  - **Border:** 1px solid black (`#000000`).
  - **Rendering:** Displays decoded EPUB cover bitmap.
  - **Missing Cover Fallback:** If art is absent, renders `coverFallback` primitive with inner 1px border (`Rect{34, 88, 112, 178}`), centered title (Ubuntu 10 Bold), author (Ubuntu 10 Regular), and `EPUB · COMPLETE` label.
- **Metadata & Action Stack:** `Rect{172, 78, 284, 198}`.
  - **Title:** `Rect{172, 78, 284, 60}`, font: 22px Bold, line height: 26px. Wraps to max 3 lines with ellipsis.
  - **Author:** `Rect{172, 142, 284, 18}`, font: Ubuntu 12 Regular, color: `#444444`.
  - **Reading Statistics:** `Rect{172, 168, 284, 16}`:
    - Percentage: `64%` (15px Bold).
    - Pages remaining: `114 pages left` (Ubuntu 10 Regular, right-aligned or inline).
  - **Progress Bar:** `Rect{172, 188, 284, 4}`:
    - Outer frame: 1px solid black.
    - Fill: Solid black (`width = 284 * percent / 100`).
  - **Chapter Context:** `Rect{172, 198, 284, 14}`, font: Ubuntu 10 Regular, color: `#444444`, text: `Chapter 14: The Escape to the Ice` (truncated to 1 line).
  - **Resume Button:** `Rect{172, 222, 284, 44}` (meets minimum 44px touch height).
    - Background: Solid Black (`#000000`), Text: Solid White (`#FFFFFF`, Ubuntu 12 Bold).
    - Action: Launches `EpubReaderActivity` at saved bookmark position.

### 3.3 Section Divider Rule
- Hairline rule from `(24, 296)` to `(456, 296)`, 1px `#E2E2E2` (or dithered 1-bit on binary controllers).

### 3.4 Recent Books Shelf
- **Bounding Box:** `Rect{24, 310, 432, 200}`.
- **Header Row:** `Rect{24, 310, 432, 28}`.
  - Section Title: `Rect{24, 314, 140, 20}`, text: `Recent on Shelf` (14px Bold).
  - View All Link: `Rect{340, 302, 116, 44}` (padded touch hit target), text: `All Books (24) →` (Ubuntu 10 Regular, underlined). Action: Pushes `LibraryActivity`.
- **Book Thumbnails (Max 3 Books):**
  - **Column 1:** Cover: `Rect{24, 348, 80, 120}`. Title/Author: `Rect{24, 474, 80, 32}`. Hit Target: `Rect{24, 348, 80, 158}`.
  - **Column 2:** Cover: `Rect{200, 348, 80, 120}`. Title/Author: `Rect{200, 474, 80, 32}`. Hit Target: `Rect{200, 348, 80, 158}`.
  - **Column 3:** Cover: `Rect{376, 348, 80, 120}`. Title/Author: `Rect{376, 474, 80, 32}`. Hit Target: `Rect{376, 348, 80, 158}`.
  - **Progress Badge:** Inset within each cover at `(cover.x + cover.w - 32, cover.y + cover.h - 14, 30, 12)`. Solid black pill, white text (`31%`, `88%`, `DONE`).
  - **Metadata Typography:** Title: Ubuntu 10 Bold (1 line truncate); Author: Ubuntu 8 Regular (1 line truncate).

### 3.5 Bottom Action Ribbon
- **Bounding Box:** `Rect{0, 744, 480, 56}`.
- **Top Separator Rule:** Line from `(0, 744)` to `(479, 744)`, 1px solid black.
- **Buttons (2 equal columns):**
  - **Library Button:** `Rect{24, 750, 208, 44}`. 1px black outline, solid white background, 17×17px library icon + label `Library (24)` (Ubuntu 12 Bold). Action: Pushes `LibraryActivity`.
  - **Files Button:** `Rect{248, 750, 208, 44}`. 1px black outline, solid white background, 17×17px folder icon + label `Files (SD Card)` (Ubuntu 12 Bold). Action: Pushes `FileBrowserActivity`.

---

## 4. State Behaviors & Error Handling

| State | Trigger Condition | Visual Behavior | Primary Action |
| :--- | :--- | :--- | :--- |
| **Default** | Library contains active reading session + recent history | Full Editorial Masthead, Hero Book Card, 3-book Recent Shelf, and Bottom Ribbon. | Tap Hero: Open Book; Tap Ribbon: Open Library/Files |
| **Missing Cover** | EPUB archive lacks `cover.jpeg`/`cover.png` or parsing fails | Displays `coverFallback` typographic book jacket with 1px inset frame and centered title/author. | Tap Hero: Open Book |
| **Long Book Title** | Title exceeds 32 characters | Wraps across up to 3 lines (26px line height). Truncates with ellipsis on line 3 if exceeding 90 characters. Layout bounds remain fixed. | Tap Hero: Open Book |
| **Empty Library** | First boot or SD card contains no readable EPUB files | Content stage hides Hero and Shelf. Displays 1-bit book icon, title `Your Library is Waiting`, explanatory copy. | Tap `Open File Browser` button |
| **Storage Error** | SD card unmounted, corrupted, or I/O failure | Content stage displays 1-bit warning triangle icon, title `SD Card Unavailable`, diagnostic copy. | Tap `Retry Storage Mount` button |

---

## 5. Hardware Input Mapping (Xteink X4 Pro)

1. **Capacitive Touch (GT911):**
   - Coordinates match 1:1 with the 480×800 UI layout.
   - All hit targets satisfy the Apple HIG `44 × 44px` minimum.
   - Active tap fires a 1-frame inversion flash (`app.clearTapFlash()`) before dispatching activity transitions.
2. **Capacitive Home Key (GT911 bit `0x10`):**
   - Pressing the Home key while on the Home Screen triggers a clean display refresh (`FULL_UPDATE`).
   - Pressing from any child activity immediately returns to Home.
3. **Physical Buttons:**
   - **Left Button (GPIO0):** Cycles selection through Recent Books shelf (Previous).
   - **Right Button (GPIO7):** Cycles selection through Recent Books shelf (Next).
   - **Power Button (GPIO3):** Enters sleep mode (paints static screensaver or sleep cover).

---

## 6. Assumptions Requiring Codex Verification

To ensure seamless firmware integration, Codex should confirm the following architectural questions during implementation planning:

1. **Display Typography Scaling:** The specification defines a `22px–26px` hero title. Can the current FreeInkUI font pipeline scale Ubuntu via FreeType, or does Codex require a pre-compiled `ubuntu_22_bold` bitmap header table?
2. **Dynamic Cover Thumbnail Generation:** The hero cover is `132 × 198px` and shelf thumbnails are `80 × 120px`. Does `HomeCoverCache` generate these two distinct sizes on demand during library indexing, or should a single downscaled master thumb be scaled at paint time?
3. **Masthead Hit Routing in FreeInkUI:** The Masthead integrates touch buttons (Search and Settings) outside of a standard `UiListActivity`. Confirm that `UiAppHost::routeTouch()` properly registers these header hit rects alongside the body components.

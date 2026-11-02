# EXP-001 Priority Update — Concept B Decision Rationale

> **Document ID:** EXP-001-RATIONALE-B  
> **Experiment Name:** Concept B: Function-Led, Composition-Driven Architecture  
> **Target Hardware:** XTEINK X4 Pro (480×800 Portrait E-Paper Display, Capacitive Touch + Hardware Keys)  
> **Status:** Design Experiment for Review (Concept A Remains Separate & Unmodified)  
> **Deliverable Prototype:** `docs/design/concept-b-prototype.html`  

---

## 1. Executive Summary & Design Pivot

Following the priority update under **EXP-001**, **Concept B** is developed as an independent design experiment that reorders product priorities to solve real-world e-reader ergonomics:

1. **Real Functionality & Efficient User Flows** (Primary driver)
2. **Information Architecture & Excellent Composition** (Proportion, whitespace, grouping)
3. **E-Paper Interaction & Performance Constraints** (1-bit supremacy, minimal redraws, no modal ghosting)
4. **Typography & Visual Polish** (Restrained, verified font scale, zero decorative ornamentation)

While **Concept A** (`home-v1.1.html`) explored a magazine-style "Editorial North Star" with an oversized centered title and hidden menu drawer, it was fundamentally a single-screen cover mockup that sacrificed task speed. **Concept B** designs the complete 5-screen operational ecosystem of CrossPoint Reader around primary user tasks without inventing unsupported features or altering the approved Concept A.

---

## 2. Design of the 5 Screens Around Primary User Tasks

Each screen in Concept B is architected strictly around what the user is trying to accomplish:

### Screen 1: Home Screen (`#screen-home`)
- **Primary User Task:** Resume the active book in 1 tap (<1 second from wake) or pick up recent reading without cognitive friction.
- **Composition & Grouping:**
  - **Reading Continuity Hero:** Grouped as a high-contrast card with an 84×124px book jacket, book title, author, chapter milestone (`Ch. 4: The Semi-Annual`), progress gauge (`64%`), and a prominent 44px high `Continue Reading →` tap slab.
  - **Recent Shelf:** 3-book secondary shelf with individual progress badges (`31%`, `88%`, `12%`), clear titles, and an `All Books (24) →` jump link.
  - **Persistent Bottom Nav Hub:** Ergonomic 4-tab bar (`Home`, `Library`, `Files`, `Settings`) positioned directly in the thumb reach zone (Y: 748..800).
- **Empty State Reorganization:**
  - *Not a label swap:* When empty, the book hero card is completely removed. In its place, an **Ingestion & Onboarding Canvas** appears, providing 1-tap actionable entry points to:
    1. `Wi-Fi Web Server` (HTTP book transfer portal at `http://192.168.1.150:8080`)
    2. `SD Card File Browser` (inspect unindexed directories)
    3. `USB Mass Storage Sideload` (instructions)
    4. Live MicroSD storage capacity & FAT32 health meter.

### Screen 2: Library Screen (`#screen-library`)
- **Primary User Task:** Rapidly search, sort, filter, and pick a book from the local catalog.
- **Composition & Grouping:**
  - **Control Ribbon:** Integrated sort tabs (`[ Recent ]`, `[ Title ]`, `[ Progress ]`) and an instant text filter input (`Filter titles...`).
  - **List Composition:** Two-column visual rhythm: 48×68px mini thumbnail + title/author/metadata on the left, high-contrast completion badge + progress bar on the right. Tapping any book immediately opens it in the Reader.
  - **Pagination Bar:** Tactile 44px touch buttons (`← Prev`, `Next →`) with explicit page indicator (`Page 1 of 4`).
- **Empty State Reorganization:**
  - Shows diagnostic recovery guidance, format support reminder (`.epub`, `.txt`, `.fb2`, `.cbz`), and 1-tap `Rescan SD Card Index` and `Open File Browser` buttons.

### Screen 3: Reader Screen (`#screen-reader`)
- **Primary User Task:** Deep, distraction-free reading with instant access to reading tools when needed.
- **Composition & Grouping:**
  - **Clean Reading Surface:** Zero persistent chrome. Running header with book title and chapter; beautifully kerned justified text with 1.6 line height; running footer with exact progress (`Page 42 of 280 (15%)`) and battery/clock.
  - **In-Book Reader HUD (On-Demand):** Tapping anywhere on the text surface reveals a clean 1-bit control layer:
    - *Top HUD:* Direct return `← Library`, and `📑 TOC` (Table of Contents) trigger.
    - *Bottom HUD:* Chapter scrubber slider, font size stepper (`Font -`, `15pt Serif`, `Font +`), margins toggle, and manual e-paper waveform refresh trigger (`⚡ Refresh`).
  - **TOC Drawer Modal:** Displays the complete chapter list with page numbers and current reading position indicators for instantaneous jumping.

### Screen 4: Files / SD Card Screen (`#screen-files`)
- **Primary User Task:** Direct filesystem exploration, sideload verification, and unindexed document launching.
- **Composition & Grouping:**
  - **Breadcrumb Navigation:** Clear path bar (`/sdcard/books/`) with quick parent folder navigation (`[..] Parent Folder`).
  - **Folder & File Hierarchy:** Clear 1-bit badges (`DIR`, `EPUB`, `TXT`), file sizes in tabular layout, and modification dates.
  - **Storage Bar:** Persistent footer showing exact SD card capacity (`14.2 GB / 32 GB Used · 44% Available`).
- **Empty State Reorganization:**
  - Detects empty directory and gives 1-tap jump back to the root storage directory.

### Screen 5: Settings Screen (`#screen-settings`)
- **Primary User Task:** Fast, legible configuration of display refresh waveforms, reader defaults, and connectivity.
- **Composition & Grouping:**
  - Grouped into CrossPoint's verified operational domains:
    1. *Display & E-Paper Waveforms:* `Full Refresh Waveform` (`Every 10 p.`), `Sunlight Fading Fix` (`OFF`).
    2. *Reader Defaults:* `Default Typography` (`Merriweather Serif`), `Line Spacing` (`1.6`).
    3. *Hardware & Controls:* `Home Button Action` (`Return Home`), `Auto Sleep Timeout` (`10 Minutes`).
    4. *Network & Sync:* `Wi-Fi Connection` (`CONNECTED`), `Web Server` (`IDLE`).
    5. *System Info:* Firmware version `v2.1.0-x4pro` (ESP32-C3 / FreeInk SDK).
  - Tapping any setting opens an interactive dialog to change parameters directly.

---

## 3. Explicit Comparison: Concept B vs. Concept A

| Evaluation Metric | Concept A (Editorial North Star v1.1) | Concept B (Function-Led, Composition-Driven) | Advantage of Concept B |
| :--- | :--- | :--- | :--- |
| **Taps to Continue Reading** | 1 tap (centered pill button) | 1 tap (dedicated 44px touch slab) | Equal, but Concept B adds chapter name and pages-left context. |
| **Taps to Search Library** | **2 taps** (Tap Menu in masthead → Open full modal drawer → Tap Search) | **1 tap** (Direct search bar in Library or 1 tap to Library) | **50% fewer taps**; zero modal drawer blocking. |
| **Taps to Access Files / SD** | **2 taps** (Tap Menu in masthead → Tap Files in drawer) | **1 tap** (Direct `Files` tab in bottom hub) | **50% fewer taps**; immediate sideload workflow. |
| **Taps to Adjust Refresh / Settings** | **2 taps** (Tap Menu → Tap Settings) | **1 tap** (Direct `Settings` tab in bottom hub) | **50% fewer taps**; fast configuration. |
| **Thumb Ergonomics (480×800)** | Primary navigation triggers in upper right bezel (`Y: 0..40`), leading to GT911 edge capacitance misses. | Primary navigation in lower canvas (`Y: 748..800`), resting naturally inside the user's thumb sweep. | **Drastically superior one-handed reachability.** |
| **E-Paper Refresh Footprint** | Every menu open/close flashes a full-screen drawer overlay, leaving partial ghosting artifacts. | Standardized persistent bottom hub; page transitions are discrete and clean without heavy modal backdrops. | **Reduced ghosting and lower display controller power consumption.** |
| **Empty State Treatment** | Decorative: swaps book title with "Your Library is Waiting" and shows a blank cover outline. | Functional: transforms into an **Ingestion Canvas** with Wi-Fi Web Server shortcut and storage health. | **Actively guides the user to load content.** |
| **Firmware Feasibility** | Relies on unverified dynamic 30pt vector serif rendering on ESP32-C3 (380KB RAM). | Uses verified 19pt title and standard bitmap font metrics (`ubuntu_10`, `ubuntu_12`). | **Zero risk of font engine crashes or heap exhaustion.** |

---

## 4. E-Paper & Hardware Discipline

Concept B adheres strictly to the physical realities of the XTEINK X4 Pro:
1. **Zero Glassmorphism / Blur:** No `backdrop-filter: blur()`, semi-transparent gray tints, or soft shadows. All UI is 1-bit black (`#000000`) and pure white (`#FFFFFF`).
2. **Zero Animation:** No CSS transforms, transitions, or smooth scrolling. E-paper displays update in discrete waveform pulses (50–120ms); simulated in the prototype via a 60ms binary refresh pulse.
3. **Apple HIG Touch Target Compliance:** All interactive touch targets (buttons, list rows, tabs, sliders) maintain a minimum bounding box of 44×44px with a minimum 4px gutter to avoid mis-taps.
4. **Preservation of CrossPoint Architecture:** All data models (book progress, recent books, SD card directories, refresh intervals, webserver endpoints) reflect real, existing CrossPoint Reader subsystems.

---

## 5. Review & Verification Instructions

To inspect the Concept B prototype:
1. Open [`docs/design/concept-b-prototype.html`](file:///g:/AI-Setup/XTEInk-X4-Pro/xteink-os/docs/design/concept-b-prototype.html) in any modern web browser.
2. In the top toolbar, use the **Screen Switcher** buttons (`1. Home`, `2. Library`, `3. Reader`, `4. Files`, `5. Settings`) or the bottom navigation hub on the device canvas to navigate between all 5 screens.
3. Toggle between **Populated** and **Empty / Initial** states to observe the meaningful layout reorganization on Home, Library, and Files.
4. Test the interactive task flows:
   - **Flow 1 (Resume):** Tap `Continue Reading →` on Home to launch directly into the Reader.
   - **Flow 2 (Library Filter):** Navigate to Library, type in the filter input (e.g., `Dune`), or click sort chips. Tap any book to launch Reader.
   - **Flow 3 (Reader HUD):** In Reader, tap anywhere on the text to open the Reader HUD. Tap `📑 TOC` to test chapter jumping; test font size adjustment (`Font +` / `Font -`).
   - **Flow 4 (Settings Config):** In Settings, tap `Full Refresh Waveform` to open the configuration dialog and select a new interval.
   - **Flow 5 (1-Bit Invert / Flash):** Tap `🌓 Invert 1-Bit` or `⚡ Refresh Flash` in the top bar to inspect night reading mode and waveform partial updates.

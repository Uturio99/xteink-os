# QA & Verification: EXP-001 Quiet Modular (Concept B) — Refinement Pass 1

> **Document ID:** EXP-001-QA  
> **Status:** Design Prototype Verification  
> **Target:** Browser Interactive Prototype ([`prototype.html`](prototype.html))  
> **Hardware Reference Profile:** XTEINK X4 Pro (480×800 Portrait E-Paper Display)  

---

## 1. Verified Functional Test Outcomes (In Browser Automation & Interactive Test)

The following test suites were executed against [`prototype.html`](prototype.html) using automated browser checks and manual inspection:

| Test ID | Interaction / Flow | Expected Outcome | Actual Verified Outcome | Status |
| :--- | :--- | :--- | :--- | :--- |
| **QA-P0-01** | **Reader 3-Zone Page Turns** | Left zone turns previous page; right zone turns next page; center zone toggles HUD. | Tapping left decrements page (42 &rarr; 41); tapping right increments page (42 &rarr; 43); center toggles HUD overlays. | **PASS** |
| **QA-P0-02** | **Working Scrubber Slider** | Dragging / changing slider updates reading position and total page progress. | Slider inputs dynamically update page tag (`42/280`), reading text stream, and catalog progress. | **PASS** |
| **QA-P0-03** | **Reading Progress Consistency** | Progress made in Reader reflects across Home hero and Library list. | Turning pages updates active book progress (e.g. 42 &rarr; 43) and synchronizes with Home hero percentage gauge and Library list item without discrepancy. | **PASS** |
| **QA-P0-04** | **Library Pagination** | `Prev` / `Next` buttons paginate 3 items per page across collection. | Tapping `Next` advances from Page 1 of 3 to Page 2 of 3; `Prev` returns. First and last pages trigger boundary feedback toast. | **PASS** |
| **QA-P0-05** | **Live Catalog Search** | Typing query dynamically filters catalog and recalibrates pagination. | Typing "Dune" instantly filters list to 1 book and adjusts pagination to Page 1 of 1. Clearing query restores full catalog. | **PASS** |
| **QA-P0-06** | **Genuine Folder Navigation** | Clicking directory rows updates file list to reflect selected folder contents. | Clicking `/sdcard/books/Science Fiction/` lists *Left Hand*, *Dune*, and *Neuromancer*; clicking `.. Parent Folder` returns to `/sdcard/books/`. Breadcrumb string matches current directory. | **PASS** |
| **QA-P0-07** | **Direct Sideload Launch** | Clicking an EPUB in the Files screen launches directly in Reader. | Clicking *Left_Hand_Darkness.epub* inside the file browser mounts Reader with corresponding text stream. | **PASS** |
| **QA-P0-08** | **Mock Hardware Notifications** | Hardware-only actions clearly identified without claiming native execution. | Tapping Web Server or USB triggers a high-visibility toast banner explicitly marked as `Simulated: HTTP Web Server active at :8080` / `Simulated: USB Mass Storage`. | **PASS** |
| **QA-P1-01** | **Touch Hit Targets** | All actionable hit regions &ge; 44×44 CSS px; primary actions 56px. | Primary CTA (`Continue Reading`), bottom hub tabs (`56px`), library rows (`56px`), settings rows (`56px`), and pagination buttons meet touch minimums. | **PASS** |
| **QA-P1-02** | **Monochrome Visual Icons** | No colored emoji in UI chrome; all status and navigation icons use 1-bit vector linework. | Masthead clock, battery gauge, Wi-Fi wave, bottom tabs (`Home`, `Library`, `Files`, `Settings`), and search glyphs render in 1-bit monochrome SVG. | **PASS** |
| **QA-P1-03** | **Vietnamese Diacritic Wrap** | Complex diacritics (*Nỗi Buồn Chiến Tranh* by *Bảo Ninh*) render without clipping. | Multi-line wrap with safe 1.4 line-height and optimized typography ensures no ascender/descender clipping on `ệ, ổ, ử, ỹ, ắ`. | **PASS** |
| **QA-P1-04** | **Empty State Reorganization** | Empty state reorganizes content into an Onboarding Ingestion Canvas. | Toggling to Empty replaces hero with 3 ingest methods (Web Server, SD Browser, USB sideload) and storage metrics instead of an empty book mockup. | **PASS** |

---

## 2. Screenshot Inventory (480×800 Hardware Resolution)

Exact 480×800 pixel viewport captures generated via headless browser automation:

1. [`screenshots/home-populated.png`](screenshots/home-populated.png) — Home screen with active reading continuity hero, recent shelf, and bottom hub.
2. [`screenshots/home-empty.png`](screenshots/home-empty.png) — Home screen reorganized into the First-Run Onboarding & Ingestion Canvas.
3. [`screenshots/library.png`](screenshots/library.png) — Library screen showing filter input, sort tabs, paginated list, and reading progress gauges.
4. [`screenshots/reader.png`](screenshots/reader.png) — Reader screen showing distraction-free reading surface with 3-zone page turn touch layout.
5. [`screenshots/reader-hud.png`](screenshots/reader-hud.png) — Reader screen with active HUD overlays: chapter scrubber, font stepper, margins toggle, and TOC trigger.
6. [`screenshots/files.png`](screenshots/files.png) — Files screen with hierarchical directory navigation, breadcrumb path, and SD storage bar.
7. [`screenshots/settings.png`](screenshots/settings.png) — Settings screen with categorized display waveforms, reader defaults, and network toggles.

---

## 3. Outstanding Defects & Simulated Aspects

The following behaviors remain browser-level simulations and **must not** be treated as verified embedded C++ or hardware results:

1. **Simulated Waveform Refresh:**
   - The 60ms binary flash simulates a partial/full display controller waveform update visually, but does not drive real e-paper controller waveform lookup tables (LUTs) or clear physical pigment ghosting on hardware.
2. **Bottom Navigation Hypothesis:**
   - The persistent bottom navigation hub remains a testable Concept B design hypothesis. While it reduces navigation taps in user flows, its superiority over Concept A's minimal editorial masthead has **not** been measured through physical device user testing.
3. **Hardware Storage Access:**
   - Filesystem navigation runs in a client-side JavaScript dictionary object; it does not measure FAT32 SPI read latencies or handle SD card hot-unplug events.
4. **Touch Controller Calibration:**
   - Touch targets meet software bounding box specs (&ge;44px), but physical GT911 edge capacitance on production glass requires hardware bench validation.

---

## 4. Untested Claims (Excluded from Production Baseline)

- **ESP32-C3 Memory Profile:** Static memory overhead for retaining a 5-screen UI state machine has not been benchmarked against the 380 KB usable RAM constraint.
- **Battery Autonomy:** No power meter measurements exist comparing Concept B's persistent bottom hub redraws against Concept A's drawer overlays.

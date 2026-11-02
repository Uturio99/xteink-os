# EXP-001: Quiet Modular (Concept B)

> **Status:** EXPERIMENTAL / NOT APPROVED  
> **Target Hardware:** XTEINK X4 Pro (480×800 Portrait E-Paper Display)  
> **Visual Direction:** Quiet Modular (Function-Led, Composition-Driven)  
> **Prototype Entry:** [`prototype.html`](prototype.html)  
> **Design Notes:** [`design-notes.md`](design-notes.md)  
> **QA & Verification:** [`QA.md`](QA.md)  
> **Report:** [`report.md`](report.md)  

---

## 1. Experiment Purpose & Status

EXP-001 explores **Concept B ("Quiet Modular")** as an independent design experiment for the XTEINK X4 Pro.

- **Status:** **EXPERIMENTAL / NOT APPROVED**.
- **Relationship to Concept A:** Concept A (`home-v1.1.html` / H1.1A native layout) remains the approved reference and is untouched. Concept B does not replace, alter, or restart Concept A.
- **Intent:** Evaluate a function-led, composition-driven multi-screen information architecture where navigation efficiency, context-aware empty states, and e-paper interaction constraints take precedence over purely editorial poster aesthetics.

---

## 2. Priority Hierarchy

Concept B follows a strict priority order:
1. **Real Functionality & Efficient User Flows:** Core reading, library search, file exploration, and hardware configuration take priority over ornamental framing.
2. **Information Architecture & Excellent Composition:** Clear grouping, deliberate whitespace, and meaningful reorganization between content states.
3. **E-Paper Interaction & Performance Constraints:** Pure 1-bit high contrast, zero animations or glass effects, discrete waveform updates, and touch zones ≥44px.
4. **Typography & Visual Polish:** Restrained typography scale matching verified in-tree bitmap metrics.

---

## 3. Implemented Screens and Interactions

Concept B articulates the complete 5-screen operational model in [`prototype.html`](prototype.html):

1. **Home Screen (`#screen-home`):**
   - *Populated:* Reading continuity hero (jacket, chapter milestone, pages left, progress bar, 44px continue reading slab), 3-book recent shelf, and 4-tab bottom navigation hub.
   - *Empty State:* Meaningfully reorganizes into an Ingestion & Onboarding Canvas offering direct actions to Wi-Fi Web Server, SD card browsing, USB transfer info, and storage health metrics.
2. **Library Screen (`#screen-library`):**
   - *Populated:* Search filter input, sort tabs (`Recent`, `Title`, `Progress`), rich book list with progress bars, and pagination.
   - *Empty State:* Reorganizes into an ingestion troubleshooting guide with a 1-tap SD rescan trigger.
3. **Reader Screen (`#screen-reader`):**
   - Clean reading view with running header/footer. Tapping the surface toggles the Reader HUD (exit to library, chapter scrubber, font size adjuster, margins toggle, and Table of Contents drawer).
4. **Files (SD Card) Screen (`#screen-files`):**
   - Path breadcrumbs (`/sdcard/books/`), directory and file list with type badges (`DIR`, `EPUB`, `TXT`), and persistent storage capacity footer.
5. **Settings Screen (`#screen-settings`):**
   - High-contrast categorized settings (`Display & Waveforms`, `Reader Defaults`, `Hardware & Controls`, `Network & Sync`). Interactive modal allows configuring refresh intervals, toggling sunlight fix, and toggling Wi-Fi.

---

## 4. Key Differences from Concept A

| Dimension | Concept A (Editorial North Star v1.1) | Concept B (Quiet Modular) |
| :--- | :--- | :--- |
| **Primary Driver** | Print editorial/magazine cover aesthetic | Multi-screen task efficiency and reading continuity |
| **Navigation** | Hidden modal drawer accessed via top-right menu trigger | Persistent 4-tab bottom hub in natural thumb reach zone |
| **Scope** | Single screen mockup (Home only) | 5 interconnected screens (Home, Library, Reader, Files, Settings) |
| **Empty State** | Text label swap on static frame ("Your Library is Waiting") | Context-aware layout reorganization into an Onboarding Ingestion Canvas |
| **Reader HUD** | Out of scope / undefined | Interactive in-book HUD with live Table of Contents modal and font adjustment |

---

## 5. Known Limitations

- **Browser Prototype Environment:** Prototype runs in HTML/CSS/JS without FreeType or FreeInk SDK runtime dependencies.
- **Waveform Simulation:** The 60ms flashing overlay is a UI approximation of e-paper waveform flashing, not physical controller timing.
- **Mock Data:** Book titles, chapter text, and filesystem entries are mock objects embedded for task validation; they are not connected to SQLite or FAT32 SPI drivers.
- **Touch Driver Target:** Touch coordinates simulate standard pointer events; GT911 edge capacitance must be verified on physical X4 Pro hardware if implemented.

---

## 6. How to Open the Prototype

Open the standalone prototype file in any standard browser:
- File path: `docs/design/experiments/EXP-001-quiet-modular/prototype.html`
- Use the top control toolbar to switch screens, toggle populated vs. empty states, simulate e-paper refresh flash, or invert display to dark/night mode.
- Alternatively, use the simulated bottom navigation bar on the 480×800 device chassis.

---

## 7. Agent Instructions

- **Codex (C++ / Firmware):** Do NOT implement this experiment in firmware. Concept A remains the approved baseline (`src/components/X4ProHomeLayout.h` and `home-v1.1.html`). Treat this directory as reference design exploration only.
- **DeepSeek / Other Reviewers:** Evaluate task efficiency and information architecture strictly against the criteria in `QA.md` and `design-notes.md`. Do not treat prototype timing as hardware benchmarks.
- **Antigravity:** Preserve this experiment under `docs/design/experiments/EXP-001-quiet-modular/`. Do not overwrite or discard files. Do not create firmware diffs or commit changes without explicit human instruction.

# XTEInk OS — Design Contract Reconciliation Changelog v0.5

> **Document ID:** XOS-LOG-005  
> **Milestone:** HOME-005 (Design Contract Reconciliation)  
> **Author:** Lead UI/UX Designer  
> **Status:** Reconciled Specification for Codex Implementation Planning  

---

## 1. Codex Audit Reconciliation Matrix

The table below maps each technical audit finding raised by Codex on Home v0.4 to its authoritative resolution in Home v0.5:

| # | Codex Technical Audit Finding | Design Contract Resolution in v0.5 | Authoritative Verification Reference |
| :--- | :--- | :--- | :--- |
| **1** | **Font rasterization point sizes vs pixel heights:** Ubuntu 8/10/12 are point sizes rasterized at 150 DPI, not 8/10/12 pixel heights. | Updated all typography tables to reflect true compiled line advances: `ubuntu_8` advanceY = 19px, `ubuntu_10` advanceY = 24px, `ubuntu_12` advanceY = 29px. | Confirmed in `lib/EpdFont/builtinFonts/ubuntu_*_*.h` structs. |
| **2** | **3-Line title overflow in 60px slot:** The three-line title cannot fit into a 60px container. | Expanded the title slot vertical headroom to 84px in Long Title state; standardized standard title to 2 lines (58px) with glyph-width-aware ellipsis truncation. | Tested across all 6 states in `home-v0.5.html`. |
| **3** | **Arbitrary character-count truncation:** Fixed character-count limits do not account for proportional font metrics. | Replaced character counts with a glyph-width-aware truncation rule using `GfxRenderer::getTextWidth()` and `fui::TextStyle.maxLines`. | Reconciled in `HOME_SPEC_v0.5.md` Section 3. |
| **4** | **Diacritic clearance & Vietnamese text:** Vietnamese stacked diacritical marks clip if line height is insufficient. | Established vertical slot heights matching true `advanceY` (24px/29px) and mandated minimum 4px vertical padding on badges. | Reconciled in `DESIGN_SYSTEM_v0.5.md` Section 2.2. |
| **5** | **Layout overlap & budget imbalance:** The hero was cramped while below the shelf sat >200px of empty space. | Authored a single internally consistent master coordinate table: Hero allocated 248px height, shelf allocated 214px, whitespace buffer allocated 174px. | Verified in `HOME_SPEC_v0.5.md` Section 2. |
| **6** | **Touch target collision in Hero:** The entire hero card and the Resume button had overlapping touch regions. | Separated hero cover slot (`Rect{24, 84, 132, 198}`) and Resume button (`Rect{172, 238, 284, 44}`) into distinct, non-overlapping hit rects. | Verified in `home-v0.5.html` and `HOME_SPEC_v0.5.md`. |
| **7** | **Unreliable metadata (chapters, pages remaining):** CrossPoint underlying stores do not guarantee chapter title or whole-book pages remaining together. | Removed unverified chapter and page count fields. Implemented graceful omission: if progress < 0, progress bar is cleanly omitted without breaking layout. | Reconciled with `BookProgress.h` (`loadBookProgress()`). |
| **8** | **Missing content states:** The design lacked explicit states for library with books but no history, and storage unavailable. | Formally defined and previewed all 6 states (A: Empty, B: Books Present No History, C: Active Default, D: Missing Cover, E: Long Title, F: Storage Error). | Fully interactive in `home-v0.5.html` QA switcher. |
| **9** | **Supported non-EPUB books:** Typographic fallback only handled EPUB books. | Extended fallback jacket hallmark to dynamically display the file format (`TXT`, `XTC`, `EPUB`). | Supported in `HOME_SPEC_v0.5.md` Section 4. |
| **10** | **Hardcoded Home button refresh:** Assigning Home-on-Home to full refresh violated user-configurable settings. | Removed hardcoded Home-on-Home full refresh rule. Hardware Home button behavior is governed strictly by `HomeButtonSettings.h`. | Preserved in `DESIGN_SYSTEM_v0.5.md` Section 4.2. |
| **11** | **Search routing architecture:** Proposed global search daemon without in-tree support. | Mapped Masthead Search button directly to `LibraryActivity` with search filter focus, leveraging CrossPoint's verified search. | Documented in `HOME_SPEC_v0.5.md` Section 5. |
| **12** | **Cover scaling & aspect ratio:** Undefined scaling caused potential image distortion. | Formalized Aspect-Fit with optical horizontal centering (`GUI.drawCoverThumbFill`). Covers scale proportionally inside 2:3 slot without cropping. | Reconciled in `DESIGN_SYSTEM_v0.5.md` Section 5.1. |
| **13** | **Extra refresh before navigation:** Flash before navigation risked slow multi-refresh cycles. | Restricted tap confirmation to immediate 1-frame in-memory inversion (`app.clearTapFlash()`) without triggering extra EPD waveform cycles. | Specified in `DESIGN_SYSTEM_v0.5.md` Section 1.1. |

---

## 2. Remaining Design Trade-Offs

1. **Cover Aspect-Fit Side Slivers vs. Cropping:**
   - *Decision:* Maintain aspect-fit with optical centering. If a cover is slightly narrower than standard 2:3, small vertical slivers may be visible at the sides within the 1px black bounding frame.
   - *Trade-off:* Eliminates any risk of cropping book titles, author credits, or publisher typography from non-standard aspect ratio covers.
2. **Exclusion of Chapter Titles from Home Screen:**
   - *Decision:* Omitted chapter names from the Home screen.
   - *Trade-off:* While a chapter name provides granular reading context, CrossPoint's `RecentBooksStore` does not persist chapter names without parsing EPUB TOCs in the background, which would cause noticeable SD card I/O delays during Home screen startup.

---

## 3. Unresolved Technical Questions (For Codex Implementation)

1. **FreeType Display Font Asset vs. Bitmap Header:**
   - *Question:* Does Codex prefer to load FreeType vector fonts dynamically for the 22pt hero title (`advanceY: 28px`), or should an offline bitmap table (`ubuntu_22_bold.h`) be generated and compiled into `lib/EpdFont/builtinFonts/`?
2. **Library Search Filter Activation:**
   - *Question:* Does `LibraryActivity` currently support an intent/argument flag to open directly with the text keyboard focused upon entry from the Masthead Search button?

# XTEInk OS — Home Refinement Changelog v0.4

> **Document ID:** XOS-LOG-004  
> **Milestone:** HOME-004 (Design Refinement v0.4)  
> **Author:** Lead Product & UI Designer  
> **Status:** For Product Owner Review  

---

## 1. Summary of Changes (v0.3 → v0.4)

| Scope Area | Changes Made in v0.4 | Rationale & Architectural Benefit |
| :--- | :--- | :--- |
| **Primary Direction Lock** | Standardized on **Editorial Masthead (Alternative v0.3)** as the definitive single Home Screen foundation. Removed competing multi-theme alternatives. | Delivers a focused, singular production target rather than fragmented UI themes. |
| **Bottom Navigation Overhaul** | **Removed the permanent 5-item bottom dock.** Replaced with a streamlined **Bottom Action Ribbon** containing two dedicated touch buttons: `Library (24)` and `Files (SD Card)`. | Reclaims ~60px of vertical space for generous reading whitespace while keeping core navigation instantly discoverable. |
| **Header & Masthead Integration** | Unified the status bar and utility shortcuts into a compact **44px Editorial Masthead**. Directly integrates `Search` and `Settings` touch targets (min 44×44px hit bounds). | Frees the lower canvas from application clutter while preserving 100% functional access. |
| **De-Skeuomorphing & Cleanliness** | Eliminated all remaining fake drop shadows, spine crease gradients, and gray card backgrounds (`#FAFAFA`, `#F0F0F0`). Surfaces are pure white paper (`#FFFFFF`) with 1px black rules. | Prevents e-paper waveform ghosting and gray-level flashing on X4 Pro controllers. |
| **Integer Coordinates Specification** | Authored [`HOME_SPEC_v0.4.md`](HOME_SPEC_v0.4.md) defining exact integer pixel coordinates across the entire 480×800 portrait UI canvas. | Enables Codex to map design components directly into FreeInkUI `Rect` structs without coordinate estimation. |
| **Typography Categorization & Honesty** | Categorized `Ubuntu 8`, `10`, and `12` as verified in-tree bitmap fonts. Marked sizes >12px (`14px, 18px, 22px`) as proposed display design tokens requiring engineering validation. Explicitly noted that CSS sizes do not claim exact binary equivalence with physical bitmap tables. | Eliminates unverified firmware claims and clarifies font asset requirements for Codex. |
| **Hardware Input Specification** | Documented physical button mappings: Left button (`GPIO0`) for Up/Prev, Right button (`GPIO7`) for Down/Next, Power (`GPIO3`) for sleep/wake, and capacitive Home key (`GT911 bit 0x10`). | Seamless dual-input support (Touch + Hardware Keys) on the X4 Pro. |
| **Clock Update Policy** | Specified a strict, static clock policy: the clock renders statically on wake, screen transitions, or discrete 1-minute RTC interrupts. Continuous JS/firmware polling loops are banned. | Eliminates unnecessary e-paper repaints and battery drain. |
| **State Coverage Expanded** | Created interactive preview toggles in `home-v0.4.html` covering all 5 core states: Default, Missing Cover, Long Book Title (3-line wrap + ellipsis), Empty Library, and Storage Error. | Enables thorough Product Owner visual QA before firmware implementation. |

---

## 2. Remaining Design Trade-Offs

1. **Cover Aspect Ratio vs. Screen Real Estate:**
   - *Decision:* Standardized on an authentic book proportion of `2:3` (`132 × 198px` for Hero, `80 × 120px` for Shelf).
   - *Trade-off:* While a square `1:1` or wide `16:9` tile would fit more metadata horizontally, the `2:3` ratio preserves literary authenticity and aligns with standard published EPUB book jackets.
2. **Masthead Icon Utility vs. Text Labeling:**
   - *Decision:* `Search` and `Settings` are presented as minimal 19×19px line-art icons in the top Masthead with 44×44px touch hit zones, while `Library` and `Files` use text + icon buttons in the bottom ribbon.
   - *Trade-off:* Preserves a quiet, bookish header aesthetic without text congestion, while giving the primary file and library destinations prominent, unambiguous text labels.
3. **Recent Books Shelf Capacity (Maximum 3 Books):**
   - *Decision:* Capped the horizontal shelf at 3 books (rather than 4 to 6).
   - *Trade-off:* Readers with large active queues must tap "All Books (24)" to browse deeper, but the 3-book limit guarantees generous whitespace, legible titles, and zero horizontal visual compression.

---

## 3. Unresolved Technical Questions (For Codex Verification)

1. **Display Font Rendering Pipeline:**
   - *Question:* Does FreeInkUI currently support scaling vector TrueType fonts on the fly via FreeType for the proposed 22px display title, or must Codex generate pre-compiled bitmap tables (e.g. `ubuntu_22_bold.h`)?
2. **Cover Thumbnail Storage & Scaling:**
   - *Question:* Can the X4 Pro ESP32-S3 firmware downscale cover art dynamically from the EPUB container to `132×198px` and `80×120px`, or should `HomeCoverCache` generate and store pre-rendered 1-bit BMP thumbnails on the MicroSD card?
3. **FreeInkUI Masthead Touch Hit Registration:**
   - *Question:* Since the Home screen does not subclass a standard `UiListActivity`, will `UiAppHost::routeTouch()` natively process the `Search` (`Rect{278, 0, 44, 44}`) and `Settings` (`Rect{324, 0, 44, 44}`) hit areas, or should a custom interaction table be declared?

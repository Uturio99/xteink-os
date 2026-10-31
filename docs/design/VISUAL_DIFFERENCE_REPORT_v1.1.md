# XTEInk OS — Visual Difference Report (v1.0 vs. v1.1)

> **Document ID:** XOS-QA-DIFF-011  
> **Milestone:** HOME-007 (Visual Fidelity Correction)  
> **Target Hardware:** XTEINK X4 Pro ONLY (480×800 Portrait UI Canvas)  
> **Author:** Lead Visual Designer  
> **Status:** Visual QA Review & Product Owner Verification  

---

## 1. Executive Summary

During the visual fidelity review of Home v1.0, the Product Owner identified that the layout leaned too heavily toward a conventional e-reader dashboard (dominated by a large 116×174px cover image, crowded top utility shortcuts, and a two-button bottom dock) rather than the approved **Visual North Star** (**Premium Editorial Minimalism × Apple HIG × Durobo Krono**).

In **Home v1.1** (`home-v1.1.html`), the visual hierarchy has been corrected:
1. The **book title** is now the commanding editorial hero, typeset in expressive, oversized serif typography (30px) with an italic author accent (15px).
2. The companion book cover has been reduced to a restrained, elegant accent (`76 × 114px`), preventing it from overpowering the typography.
3. The rectangular button has been replaced with the reference's **minimal black pill CTA** (`border-radius: 9999px`).
4. The crowded utility header and two-button footer have been eliminated in favor of a **quiet minimal masthead** and an on-demand **editorial menu drawer** holding `Library`, `Files`, `Search`, and `Settings`.
5. Recent Books have been moved to a **restrained editorial bookshelf** near the lower portion of the screen, stripped of noisy badges, borders, and extra metadata.
6. All 6 required content states (A through F) are faithfully retained and fully functional.

---

## 2. Side-by-Side Visual Comparison

| Review Dimension | Home v1.0 (Failed Review) | Home v1.1 (Corrected North Star) | Visual QA Impact |
| :--- | :--- | :--- | :--- |
| **Hero Focal Anchor** | Large book cover (`116 × 174px`) centered above a modest 21pt title. Eye was pulled immediately to the cover box. | **Dominant Book Title** (`30px` Serif Bold, `line-height: 36px`) centered with an expressive `15px` italic author accent. | Restores literary print authority; the title commands the screen. |
| **Companion Cover** | Dominated 38% of the upper canvas (`116 × 174px`). | Subordinate, restrained companion jacket (`76 × 114px`, 2:3 ratio). | Rebalances proportions; cover complements rather than overwhelms the title. |
| **Call to Action (CTA)** | Heavy rectangular box (`210 × 44px`, 2px radius, full borders). | **Minimal Black Pill CTA** (`border-radius: 9999px`, 44px touch height, unbordered). | Matches Apple HIG ergonomic elegance and the approved visual reference. |
| **Top Masthead** | Crowded with 6 items: Brand, Clock, Search button, Settings button, Wi-Fi icon, Battery %, Battery glyph. | **Quiet Minimal Masthead** (40px): Time (`14:45`) on left, Battery (`86%`) and quiet `Menu` trigger on right. | Eliminates smartphone status bar clutter; creates serene literary breathing room. |
| **Bottom Navigation** | Two heavy rectangular buttons in a 56px bottom bar: `[ Library (24) ]` and `[ Files (SD Card) ]`. | **Zero Bottom Dock**: Navigation moves to the quiet `Menu` trigger and the bookshelf's `All Books (24) →` link. | Eliminates application dock aesthetic; opens up 56px of luxurious whitespace. |
| **Secondary Shelf** | Heavy card styling, dividing line, and noisy black pill badges (`31%`, `88%`, `DONE`) pinned over covers. | **Restrained Editorial Bookshelf** near lower screen: 3 clean covers, quiet title & author, **zero noisy badges**. | Clean, unadorned presentation; books sit peacefully like physical editions. |
| **Whitespace Distribution** | Fragmented between multiple bordered containers and rules. | **Expansive, Intentional Whitespace**: Fluid negative space surrounding the centered hero. | Contemplative, uncluttered e-paper experience. |

---

## 3. Navigation Accessibility Contract (No Smartphone Navigation)

All 4 core destinations remain instantly discoverable and accessible without smartphone-style UI chrome:
1. **Library Access:**
   - Primary: Tap `All Books (24) →` directly in the bookshelf header.
   - Secondary: Tap `Menu` in masthead -> Select `Library (24 Titles)`.
2. **Search Access:**
   - Tap `Menu` in masthead -> Select `Search Library` (routes to `LibraryActivity` with search focus).
3. **Files (SD Card) Access:**
   - Tap `Menu` in masthead -> Select `Files (SD Card)` (routes to `FileBrowserActivity`).
   - Also directly accessible in State A (Empty Library) via `Open File Browser →` pill.
4. **Settings Access:**
   - Tap `Menu` in masthead -> Select `Settings` (routes to `SettingsActivity`).

---

## 4. Verification of Content States in `home-v1.1.html`

- **State C (Currently Reading — Visual North Star):**
  Dominant 30px title (*The Left Hand of Darkness*), 15px italic author (*Ursula K. Le Guin*), 76×114px companion cover, hairline progress (`64% Completed`), minimal black pill CTA, and restrained 3-book shelf.
- **State D (Missing Cover Fallback):**
  Typographic book jacket with 1px inset rule, centered title/author, and `EPUB · COMPLETE` hallmark.
- **State E (Long Vietnamese Title & Diacritics):**
  Tests stacked diacritics on *Nỗi Buồn Chiến Tranh* by *Bảo Ninh*. 34px line advance ensures zero ascender/descender clipping on `ệ, ổ, ử, ỹ, ắ`.
- **State B (Books Available, No History):**
  Welcome hero with invitation to browse, minimal black pill CTA `Browse Library (24) →`, and shelf populated without progress badges.
- **State A (Empty Library):**
  Peaceful 1-bit emblem, *Your Library is Waiting*, and minimal black pill CTA `Open File Browser →`.
- **State F (Storage / Index Unavailable):**
  1-bit warning triangle, *Storage Unavailable*, FAT32 guidance, and minimal black pill CTA `Retry Mount →`.

---

## 5. Unverified Firmware Requirements for Codex

The following technical requirements are documented for Codex to validate on hardware, without asserting premature certainty:
1. **Oversized Serif Rendering (`30pt` / `advanceY: 36px`):**
   - *Option A:* Utilize FreeType vector font engine (currently compiled in CrossPoint for EPUB reading) to render the Home UI title dynamically.
   - *Option B:* Pre-compile bitmap font tables (`literata_30_bold.h` and `literata_15_italic.h`) via CrossPoint's font toolchain.
2. **Minimal Pill Button Drawing Primitives:**
   - Drawing a rounded pill (`border-radius: 9999px`) on e-paper requires rendering two filled semicircles (radius = 22px) connected by a filled rectangle (`GUI.drawFilledRoundRect` or `GfxRenderer::fillRoundRect`). Codex to verify primitive support in `FreeInkUI`.
3. **Touch Hit Zone for Masthead Menu:**
   - The top-right `Menu` button has a visual size of 16×16px but allocates an ergonomic touch hit bounds of `44 × 40px` (`X: 436 .. 480, Y: 0 .. 40`). Codex to verify GT911 edge sensitivity along the top bezel.

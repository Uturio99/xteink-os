# Art Direction: Calm Editorial Utility (EXP-001 / AD-01)

> **Document ID:** AD-01-RATIONALE  
> **Target Canvas:** 480×800 Portrait E-Paper (XTEINK X4 Pro)  
> **Visual Reference:** Apple Visual Hierarchy × DuRoBo Krono Purposeful Simplicity  
> **Status:** High-Fidelity Direction Study (Concept B)  

---

## 1. Compositional Structure & Visual Hierarchy

AD-01 resolves the tension between literary editorial warmth and utilitarian e-reader efficiency through a unified, 1-bit composition:

1. **Current Reading (Dominant Focus):** An asymmetric editorial hero commands the upper viewport. Title, author, chapter milestone, and progress gauge are typeset on the left, counterbalanced on the right by a high-contrast companion book jacket (`104×152px`). Serif display typography establishes immediate print authority.
2. **Continue Reading (Primary Action):** A full-width, 56px high solid touch slab anchors the reading hero. It provides an unmistakable, high-affordance target that users can trigger reliably with a thumb reach without scanning.
3. **Recent Books (Secondary Content):** A compact three-column shelf displays recent reads with clean title/author typography and discrete completion percentages. Heavy borders, badges, and card outlines are removed, allowing white space to delineate hierarchy.
4. **Direct Utility Actions (Lower Access):** Eliminating the permanent four-tab bottom dock, three discrete utility touch buttons (`Library`, `Add Books`, `Settings`) are integrated into the lower canvas. Each button provides an ergonomic 52px hit target without cluttering the screen with mobile-app chrome.

---

## 2. Dynamic State Reorganization

- **Populated State:** Centers around immediate reading resumption and queue continuity. Generous 32px lateral margins create breathing room around text, while high-contrast hairline rules establish calm structural separation.
- **Empty State:** Rather than displaying dummy covers or empty placeholders, the layout transforms into an actionable **Onboarding & Ingestion Canvas**. It provides 1-tap triggers for HTTP Web Server transfer (`:8080`), direct SD Card exploration, and USB sideload guidance, accompanied by real FAT32 volume health statistics.

---

## 3. E-Paper & Typography Discipline

- **1-Bit Purity:** Pure black (`#000000`) on white (`#FFFFFF`) ensures zero reliance on controller dithering or fragile grayscale.
- **Vietnamese Readability:** Titles use 1.28 line height with explicit diacritic clearance, eliminating clipping on stacked marks (*Nỗi Buồn Chiến Tranh* by *Bảo Ninh*).

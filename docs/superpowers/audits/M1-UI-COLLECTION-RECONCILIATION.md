# M1 UI/UX Collection Reconciliation Matrix & Decision Record

> **Document ID:** M1-UI-COLLECTION-RECONCILIATION  
> **Milestone:** M1 — Unified UI/UX Collection  
> **Framework:** Obra Superpowers (`executing-plans`, `verification-before-completion`)  
> **Repository:** `Uturio99/xteink-os`  
> **Status:** Completed Baseline Reconciliation  
> **Date:** 2026-11-03  

---

## 1. Executive Summary & Provenance Audit

This reconciliation record harmonizes the existing design explorations, checkpoints, and firmware constraints of the XTEInk OS project into a single, cohesive, production-ready specification for the 15-frame UI Collection (Milestone M1).

In accordance with Section 3 and 4 of the M1 Master Execution Contract, all available assets were audited against the source authority hierarchy:
1. **Level 1 — Product Owner Decisions & M1 Master Execution Contract**
2. **Level 2 — Verified CrossPoint Capabilities & Hardware Constraints** (ESP32-C3 380 KB usable SRAM vs X4 Pro ESP32-S3 with 8 MB PSRAM)
3. **Level 3 — Existing Approved Historical Design References** (Concept A / Home v1.1 in `codex/xteink-os-checkpoint-2026-10-31`, H1.1A Native Home in `checkpoint/h1.1a-native-home`)
4. **Level 4 — Proposed XOS-DL v1.0 Design System** (`docs/design/experiments/EXP-001-quiet-modular/design-system/`)
5. **Level 5 — Historical Explorations (DESIGN-002 and Concept B)**

### Asset Audit & Provenance Log

| Asset / Source | Location / Ref | Inspection Method | Status / Finding | Reconciled Role |
| :--- | :--- | :--- | :--- | :--- |
| **Concept A / Home v1.1** | Branch `codex/xteink-os-checkpoint-2026-10-31` (`0861aeaf`) | Git tree inspection (`docs/design/home-v1.1.html`, `DESIGN_SYSTEM_v1.0.md`) | **VERIFIED AVAILABLE** | Protected reference; contributes editorial title hierarchy, typography proportions, and quiet aesthetic. |
| **H1.1A Native Home** | Branch `checkpoint/h1.1a-native-home` (`877461d3`) | Git tree inspection (`src/components/CoverGridHomeUi.cpp`, `X4ProHomeLayout.h`) | **VERIFIED AVAILABLE** | Production baseline; demonstrates zero-heap allocation cover grid layout for embedded C++. |
| **XOS-DL v1.0 Spec & Tokens** | `docs/design/experiments/EXP-001-quiet-modular/design-system/` | Direct filesystem read (`tokens.json`, `FOUNDATIONS.md`, `COMPOSITION.md`, `specimen.html`) | **VERIFIED AVAILABLE** | Primary proposed visual grammar, 32px margin rhythm, 48px/56px hit target contracts, and token architecture. |
| **Concept B Prototype** | `docs/design/experiments/EXP-001-quiet-modular/prototype.html` | Direct filesystem read (93 KB interactive prototype) | **VERIFIED AVAILABLE** | Multi-screen task flows, ingestion canvas empty states, interactive HUD, SD file browser, and settings modal. |
| **DESIGN-002 Git Ref (`c9d12b52`)** | Historical ref noted in `docs/design/BASELINE.md` | Git log & commit query | **UNVERIFIED / MISSING REFERENCE** | Commit `c9d12b52` is not reachable in current repository clones or remote heads. Maintained as UNVERIFIED. |
| **DESIGN-002 Local Zip** | `xteink-os-design-002-review.zip` | Filesystem recursive scan | **UNVERIFIED / MISSING REFERENCE** | Zip archive not present on local disk. Visual composition derived from documented patterns in `EXP-001` notes and `BASELINE.md`. |

### DESIGN-002 Provenance & Historical Comparison Ledger
In strict compliance with Task F of the M1 Remediation Contract:
1. **Provenance Status:** Classified strictly as **UNVERIFIED / HISTORICAL REFERENCE**. No missing files were fabricated or claimed as recovered.
2. **Frames Requiring Later Historical Comparison:** If `xteink-os-design-002-review.zip` or commit `c9d12b52` becomes accessible, the following 7 frames must be compared against the original source assets:
   - `FR-03` (Home — Unstarted / Curated Showcase)
   - `FR-04` (Library — Curated Shelf)
   - `FR-05` (Library — Editorial List)
   - `FR-08` (Reader — Pure Immersive)
   - `FR-11` (Reader — Quick Typography Controls)
   - `FR-14` (Settings — Editorial Categorized Overview)
   - `FR-15` (Settings — Compact Waveform Modal)

---

## 2. Cross-Concept Architectural Conflicts & Resolution

| Architectural Dimension | Concept A / Home v1.1 | Concept B (`prototype.html`) | DESIGN-002 Patterns | Proposed XOS-DL v1.0 | Reconciled M1 Baseline Decision |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Global Navigation Model** | Top masthead menu icon leading to a modal drawer overlay. | Persistent 4-tab bottom navigation dock (`Home`, `Library`, `Files`, `Settings`) across all screens. | Contextual navigation with visible back link and editorial links. | Home one-tap utility links; contextual Back navigation on all sub-screens; **no persistent bottom dock**. | **Adopt XOS-DL v1.0 rule:** Home provides fast one-tap links to Library, Files/Import, and Settings. Sub-screens have top left `← Trở về` (Back) actions. No persistent 4-tab bar taking reading canvas space. |
| **Canvas & Inset Geometry** | 480×800 px, 24px insets, centered cover. | 480×800 px, 20px insets, bottom dock at Y:748..800. | 480×800 px, 32px side margins, editorial asymmetry. | 480×800 px, 32px horizontal insets, 20px top, 24px bottom clearance. Content width: 416px. | **Adopt XOS-DL v1.0:** Logical 480×800 portrait, 32px left/right insets, 20px top, 24px bottom clearance. 416px content width. Strict 1-axis alignment. |
| **Home Hero & Book Jacket** | Central prominent cover card with centered title and play pill. | 84×124px jacket with title, chapter, progress bar, and 44px continue slab. | Asymmetric layout with title prominence over cover art. | Asymmetric 2-column hero: 270px text column + 108×154px book jacket. | **Adopt XOS-DL v1.0 Asymmetry:** Left column carries book title (Display 34px), author, chapter progress; right column carries 108×154px book jacket. Single 56px `Tiếp tục đọc →` primary CTA. |
| **Empty Home Treatment** | Label swap: "Your Library is Waiting" with empty wireframe cover. | Ingestion Canvas: SD card scan, Web Server IP, USB mass storage. | Curated discovery with add books trigger. | Ingest Stage: Headline (Display 34px), descriptive copy, single dominant "Thêm sách" CTA, contextual import methods. | **Adopt XOS-DL Ingest Stage:** Meaningful structural reorganization. No fake progress or blank covers. Dominant `Thêm sách` action, supported by Wi-Fi and SD card import actions. |
| **Library Catalog View** | Cover grid (3 columns). | 2-column list with progress indicators and top sort ribbon. | Curated Shelf (editorial featured) + Editorial List (high density 1-col). | Editorial List (1-col, 48×68 thumb, resilient long titles, trailing progress). | **Support Both Curated Shelf & Editorial List:** FR-04 provides Curated Shelf; FR-05 provides high-density Editorial List. Default view is Editorial List (`tokens.json`). |
| **Touch Hit Target Contract** | 44×44 pt (Apple HIG adaptation). | 44×44 px buttons in bottom dock. | Variable 40–48px buttons. | Minimum 48×48 px; primary actions 56×56 px; preferred 64 px. | **Adopt XOS-DL Touch Contract:** Strict minimum 48×48 px hit area for browser prototype. Primary action ≥56px height. Target separation ≥8px. |
| **Typography Scale & Roles** | Display 30pt serif, 15pt sans. | 19pt bold title, 13pt sans body. | Editorial serif display, high readability. | Display 34/42, Screen Title 30/38, Section 17/23, Body 15/22, Secondary 13/19, Micro 12/17. | **Adopt XOS-DL Typography Scale:** Maximum two typographic families (editorial serif and UI sans-serif). Sentence case primary; Vietnamese NFC-normalized. |
| **Color & 1-Bit Rendering** | Monochrome with subtle card borders. | 1-bit high-contrast black/white. | Pure black/white with light separator lines. | Pure 1-bit information hierarchy: `#FFFFFF` paper, `#111111` ink, `#414141` secondary ink, `#EBEBEB` preview backing. | **Adopt XOS-DL 1-Bit Contract:** Strict monochrome hierarchy. No glassmorphism, blur, soft drop shadows, or colored badges. Must retain 100% legibility under binary thresholding. |

---

## 3. 15-Frame State Mapping & Synthesis

The 15 representative frames are defined to cover all critical product states across the 5 core functional areas:

```
[HOME]
  FR-01: Populated Home (Active reading session hero, recent shelf, utility links)
  FR-02: Empty Home (First-use ingestion stage, no fake progress, Add Books CTA, import options)
  FR-03: Unstarted / Curated Home (Catalog populated but no active session, featured book, discovery)

[LIBRARY]
  FR-04: Curated Shelf (Featured book collections, editorial grouping, cover proportions)
  FR-05: Editorial List (High-density 1-column list, 48×68 covers, long Vietnamese title wrap)
  FR-06: Filtered Search (Active query input, real-time filtered results, clear count, empty handling)
  FR-07: Empty Library (No books in catalog vs. no search results, SD scan prompt, sideload help)

[READER]
  FR-08: Immersive Reader (Zero persistent chrome, editorial serif text, reading status micro-rail)
  FR-09: Reader HUD (Contextual overlay, book progress scrubber, TOC / Settings / Back links)
  FR-10: Table of Contents (Chapter drawer modal, active chapter indicator, direct chapter jumping)
  FR-11: Reader Typography Controls (Font scale stepper, line height, margin selector, live preview)

[FILES]
  FR-12: SD File Browser (Directory tree, path breadcrumb `/sdcard/books/`, folder rows, storage meter)
  FR-13: Direct Sideload Selection (EPUB file preview, metadata card, direct Read launch action)

[SETTINGS]
  FR-14: Settings Overview (Editorial categorized rows: Display, Reading, System, Device info)
  FR-15: Compact Settings Modal (Refresh waveform interval, sunlight fading fix, binary toggles)
```

---

## 4. Hardware & Firmware Feasibility Baseline

| Subsystem | Prototype Implementation | Generic CrossPoint Baseline (ESP32-C3) | Target Hardware (XTEINK X4 Pro) | Firmware Reality Status |
| :--- | :--- | :--- | :--- | :--- |
| **Display Geometry** | 480 × 800 px CSS portrait viewport | 800×480 physical landscape panel mapped 480×800 portrait | 800×480 UC8179/SSD1677 mapped 480×800 portrait | `DESIGNED` / `BROWSER-TESTED` |
| **Touch Interaction** | Mouse click / capacitive touch events (≥48px targets) | FT6336 I2C touch controller (where present) | Goodix GT911 on I2C address 0x5D (SDK profile verified) | `DESIGNED` (Physical edge tuning pending M5) |
| **Font Pipeline** | `Noto Serif` & system sans fallback | Bitmap font engine (`ubuntu_10`, `ubuntu_12`) or compressed FreeType | FreeType vector font engine backed by 8 MB PSRAM | `DESIGNED` (Glyph cache memory allocation pending M4) |
| **Memory Budget** | Browser DOM / CSS (~2 MB RAM in browser) | 380 KB usable SRAM, no PSRAM. 0 dynamic allocations on hot path | ESP32-S3 with 16 MB Flash & 8 MB Octal PSRAM | `DESIGNED` (Must retain zero-heap growth discipline) |
| **Waveform & Refresh** | 60ms binary inverted flash simulation | Fast 1-bit waveform (<300ms) and 4-bit gray waveform (~800ms) | Dedicated e-paper controller partial/full refresh | `SIMULATED` (Hardware waveforms pending M6) |

---

## 5. Summary of Key M1 Design Commitments

1. **Quiet Editorial Utility over Launcher Complexity:** Home is an e-reader launchpad, not an Android widget board.
2. **Reading Continuity:** The single most prominent visual anchor is always the active reading book or primary next task.
3. **Vietnamese Language Priority:** Title wrapping, diacritic rendering, and line height are treated as P0 acceptance gates.
4. **Resilience to 1-Bit E-Paper:** All controls, typography, and states are guaranteed legible without grayscale or color.
5. **Zero Speculative Firmware Claims:** Prototype interactions are strictly labeled as browser simulations until validated on physical hardware in M7.

---

## 6. Final Sprint Drift Resolution & Geometry Reconciliation Record

In accordance with the M1 Final Repair Sprint contract, all 11 specification drift items and 3 geometry items have been reconciled and verified between `UNIFIED_UI_COLLECTION.md`, `unified-ui-prototype.html`, and reference screenshots:

| Frame / Area | Specification Requirement | Previous Prototype State | Reconciled & Verified State | Status |
| :--- | :--- | :--- | :--- | :--- |
| **FR-01 (Geometry)** | Small thumbnail: 40×56px | 44×62px | `.book-thumb-small` updated to exact 40×56px | **RECONCILED** |
| **FR-03 (Unstarted)** | 3 unstarted books on Home | 2 unstarted books | Added 3rd book: *Số Đỏ* — Vũ Trọng Phụng (210 trang) | **RECONCILED** |
| **FR-04 (Geometry)** | Featured cover: 120×170px | 96×136px | `.shelf-featured-cover` updated to exact 120×170px | **RECONCILED** |
| **FR-05 (Geometry & Wrap)** | Row height 84px, 2-line title wrap, 6 books | 76px, 1-line truncate, 5 books | Min-height 84px, 2-line wrap, added *Truyện Kiều*, footer `Hiển thị 1 - 6 trên 24 cuốn` | **RECONCILED** |
| **FR-06 (Relevance)** | Highlighted matches, 2 rows rendered | 0 rendered rows, unhighlighted | Rendered 2 book rows with `<mark class="search-match">`, caption & screenshot match | **RECONCILED** |
| **FR-07 (Hierarchy)** | Screen title `Thư viện sách` (30px), Headline `Chưa tìm thấy sách` (24px) | Missing screen title, single h1 | Two-tier hierarchy implemented: `Thư viện sách` h1 + wireframe + `Chưa tìm thấy sách` h2 | **RECONCILED** |
| **FR-09 (HUD)** | Back link `← Thư viện`, Chapter steppers `← Chương trước / Chương sau →` | `← Trang chủ`, Page steppers | Back link updated to `← Thư viện`, steppers invoke `prevChapter()` and `nextChapter()` | **RECONCILED** |
| **FR-12 (Title & Bar)** | Title `Trình quản lý tệp` (30px), footer `14.2 GB đã dùng / 32.0 GB · FAT32` | `Quản lý tệp SD`, `14.2 GB trống / 32 GB` | Screen title and storage footer updated to exact specification wording | **RECONCILED** |
| **FR-13 (Fallback)** | Unsupported file fallback with `.bin`, notice, disabled CTA | Single valid preview only | Added interactive inspection toggle for invalid `.bin` fallback with disabled CTA and return link | **RECONCILED** |
| **FR-14 (Labels)** | `MÀN HÌNH & E-PAPER`, `19px / Giãn dòng 1.65 →`, `HỆ THỐNG & KẾT NỐI`, `THÔNG TIN THIẾT BỊ [MÔ PHỎNG / THIẾT KẾ ĐỀ XUẤT]` | Alternate wording / `Tùy chỉnh →` | All 4 group titles and row values aligned to exact spec strings | **RECONCILED** |
| **FR-15 (Title & Fix)** | Title `Cài đặt làm mới màn hình` (24px), Sunlight Fading Fix toggle `[ BẬT ]` | `Làm mới màn hình` (26px), missing toggle | Title updated to 24px Serif, Sunlight Fading Fix toggle row added (`min-height: 48px`) | **RECONCILED** |
| **E-Paper Style** | Pure monochrome, no box shadows, inverted active states | Soft box shadow on toast, opacity press | Box shadow removed, solid 2px ink border, inverted active press styling | **RECONCILED** |
| **Provenance** | DESIGN-002 marked as unverified historical reference | Direct reuse claims | Classified as `UNVERIFIED / HISTORICAL REFERENCE (DESIGN-002)` across gallery and docs | **RECONCILED** |
| **Touch Census** | Defensible interaction census, mutation tested | Selector whitelist, 82 targets | 106 discovered, 100 evaluated, 6 documented exclusions, 0 failures, 2 mutations confirmed | **RECONCILED** |

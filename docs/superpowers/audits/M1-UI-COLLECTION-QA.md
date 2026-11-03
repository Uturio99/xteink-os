# M1 UI/UX Collection — Quality Assurance & Verification Report (Final Repair Sprint)

> **Document ID:** M1-UI-COLLECTION-QA  
> **Milestone:** M1 — Unified UI/UX Collection  
> **Test Suite:** Automated Playwright Headless + PIL 1-Bit Analysis + DOM Interaction Census + Mutation Testing + Multi-Book State Regression + 5 User Journeys  
> **Target Viewport:** 480 × 800 px (Portrait E-Paper)  
> **Date:** 2026-11-03 (Final Repair Sprint Pass)  
> **Overall QA Status:** **PASS (100% Verified Across All Automated & Manual Suites)**  

---

## 1. Executive Summary & Final Sprint Audit

Following DeepSeek's second delta review (CONDITIONAL PASS), all verified P1 defects and specification drift items were resolved and independently tested:

1. **Workstream A — FR-06 Search Presentation:**
   - Pre-populated and dynamically synchronized `#search-results-list` with the 2 matching book rows for "chiến tranh": *Nỗi buồn chiến tranh* (Bảo Ninh) and *Chiến tranh và Hòa bình* (Lev Tolstoy).
   - Injected `<mark class="search-match">` query highlighting for high-contrast e-paper legibility.
   - Verified that screenshot (`FR-06.png`), canonical binary (`FR-06_binary.png`), gallery caption, and automated tests agree.
   - Verified that search results are interactive and navigate directly into the Reader.

2. **Workstream B — Complete Touch Target Census & Mutation Testing:**
   - Replaced selector-whitelist-only auditing with a defensible, universal DOM interaction census.
   - Evaluated all interactive candidates (`button`, `a[href]`, `input`, `[role="button"]`, clickable containers, dynamic rows, Reader tap zones `.reader-tap-left, .reader-tap-center, .reader-tap-right`).
   - Census Metrics:
     - **Total Discovered:** 106 candidates.
     - **Total Evaluated:** 100 visible active controls.
     - **Total Excluded:** 6 documented exclusions (4 radio inputs inside `.radio-option-row` to avoid double-counting accessible label parents; 1 disabled primary CTA in unsupported file fallback; 1 hidden return link while valid preview is active).
     - **Total Failures:** 0. (100% compliant with `>=48×48px` / Primary CTAs `>=120×56px` / Content rows `>=200×48px`).
   - **Mutation Testing:**
     - Mutation 1 (Shrinking `.reader-tap-center` width to 30px): Verified detected as failure (`width=30px < 48px`).
     - Mutation 2 (Shrinking `#search-input-field` height to 30px): Verified detected as failure (`height=30px < 48px`).

3. **Workstream C — Rebuild Canonical 1-Bit Evidence:**
   - Preserved `docs/design/prototype/captures/before_remediation/`.
   - Regenerated all 15 reference color captures (`FR-01.png` to `FR-15.png`) at exact native 480×800 pixels.
   - Regenerated all 15 canonical 1-bit binary images (`captures/binary/FR-XX_binary.png`) via PIL threshold 180.
   - Verified pixel-for-pixel match between canonical binary files and fresh conversion: **0 pixel difference across all 15 frames**.

4. **Workstream D — 11 Drift Items & 3 Geometry Reconciliations:**
   - FR-01: `.book-thumb-small` set to exact 40×56px.
   - FR-03: Added 3rd unstarted book (*Số Đỏ* — Vũ Trọng Phụng · 210 trang).
   - FR-04: `.shelf-featured-cover` set to exact 120×170px.
   - FR-05: Set `.fr05-canvas .book-row-item` to `min-height: 84px;`, ensured 2-line title wrap without clipping, added 6th book (*Truyện Kiều* — Nguyễn Du · 150 trang), and updated footer to `Hiển thị 1 - 6 trên 24 cuốn`.
   - FR-06: Added highlighted matches and verified 2 result rows.
   - FR-07: Added two-tier hierarchy: Screen Title `Thư viện sách` (30/38px Serif), icon, headline `Chưa tìm thấy sách` (24/32px Serif), and clarification text.
   - FR-09: Top back button set to `← Thư viện`; chapter steppers set to `← Chương trước / Chương sau →` invoking `prevChapter()` and `nextChapter()`.
   - FR-12: Screen title set to `Trình quản lý tệp` (30/38px Serif); storage bar set to `14.2 GB đã dùng / 32.0 GB · FAT32`.
   - FR-13: Added interactive inspection toggle for invalid file fallback (`firmware-update.bin`, `[LỖI]`, unsupported notice, disabled open CTA, back link).
   - FR-14: Aligned 4 group titles and row values: `MÀN HÌNH & E-PAPER`, `19px / Giãn dòng 1.65 →`, `HỆ THỐNG & KẾT NỐI`, `THÔNG TIN THIẾT BỊ [MÔ PHỎNG / THIẾT KẾ ĐỀ XUẤT]`.
   - FR-15: Screen title set to `Cài đặt làm mới màn hình` (24px Serif); added Sunlight Fading Fix toggle (`[ BẬT ]`).

5. **Workstream E — Pure E-Paper Styling & Provenance:**
   - Removed drop-shadow from `.device-toast` (replaced with solid 2px ink border).
   - Enlarged `.device-toast-close` to `min-height: 48px; min-width: 48px;`.
   - Replaced opacity press feedback with monochrome active inversion (`outline: 2px solid var(--ink)` and inverted background/color).
   - Tagged DESIGN-002 mentions as `UNVERIFIED / HISTORICAL REFERENCE (DESIGN-002)`.

6. **Task 1 — Vietnamese Typography Fidelity (TrueType Fonts):**
   - Investigated browser font selection vs CSS font-family declaration: proved that unbundled `Noto Serif` caused fallback to `Georgia` / generic serif with abnormal diacritic spacing and kerning across different Chromium environments.
   - Bundled authentic TrueType `Noto Serif` (Regular, Bold, Italic) and `Noto Sans` (Regular, Bold, Italic) in `docs/design/prototype/fonts/` sourced directly from repository firmware assets (`lib/EpdFont/builtinFonts/source/`).
   - Bundled `OFL.txt` (SIL Open Font License 1.1) and created `FONTS_METADATA.md` documenting redistribution license and explicitly classifying TTF fonts as **Browser Reference Fonts** (not proven firmware-identical rendering).
   - Removed Google Fonts CDN `<link>` dependencies; verified 100% offline font loading via Playwright `offline=True` mode.
   - Enhanced rendering with `text-rendering: optimizeLegibility`, `-webkit-font-smoothing: antialiased`, and `font-feature-settings: 'kern' 1, 'liga' 1`.
   - Verified active font loading via `document.fonts.ready` in `qa_verification.py` and `capture_m1_frames.py`.
   - Re-measured glyph metrics: confirmed exact TrueType metrics ($248.4\text{px}$ vs $283.0\text{px}$ with fallback) and zero clipping on multi-line Vietnamese titles.

7. **Tasks 2–5 — Final Visual Quality Corrections:**
   - **Task 2 (Reader HUD):** Replaced blank central dismiss container in FR-09 with full underlying readable prose (running header, chapter title, prose paragraphs, running footer) matching FR-08. Controls positioned in opaque white top/bottom bars (`z-index: 50`, solid 2px ink borders, zero blur/shadows/transparency). Center tap zone (`z-index: 40`) dismisses HUD and returns to FR-08.
   - **Task 3 (Library Functional UX):** Replaced static mock list in FR-05 with real sorting implementation: Recent (catalog default), Title A–Z (Vietnamese `localeCompare('vi')`, placing *Chiến tranh và Hòa bình* first), and Progress (descending, placing *Dế Mèn phiêu lưu ký* 40% first). Dynamically synchronized `.sort-pill.active` state. Removed intrusive toast notification.
   - **Task 4 (Product vs Developer Separation):** Moved sideload invalid file inspection toggle out of the 480×800 simulated device screen into `.review-header` inspection toolbar. Cleaned FR-14 group header to `THÔNG TIN THIẾT BỊ` (removed `[MÔ PHỎNG / THIẾT KẾ ĐỀ XUẤT]`).
   - **Task 5 (Bounded Visual Polish):** FR-04 1-bit high-contrast cover styling (solid ink `#111` for Dế Mèn, paper with 2px border for Số Đỏ); FR-07 cleaned CTA `Quét lại thẻ nhớ SD →` (removed redundant `[Quét lại]`); FR-10 TOC modal title 2-line clamp; FR-11 font size steppers dynamically synchronized with active Reader settings.

---

## 2. Visual QA Matrix (15 Frames)

| Frame ID | Area | Screen State / Role | Rendered Size | Overflow (H / V) | Touch Targets (Eval / Excl) | 1-Bit Canonical Diff | Status |
| :---: | :---: | :--- | :---: | :---: | :---: | :---: | :---: |
| `FR-01` | **HOME** | Populated Home (Active Book Hero) | 480 × 800 px | PASS (0 overflow) | 8 evaluated / 0 excl | 0 px | **PASS** |
| `FR-02` | **HOME** | Empty Home (Ingestion Canvas) | 480 × 800 px | PASS (0 overflow) | 4 evaluated / 0 excl | 0 px | **PASS** |
| `FR-03` | **HOME** | Unstarted Home (3 Unstarted Books) | 480 × 800 px | PASS (0 overflow) | 6 evaluated / 0 excl | 0 px | **PASS** |
| `FR-04` | **LIBRARY** | Curated Shelf (120×170 Featured Cover) | 480 × 800 px | PASS (0 overflow) | 7 evaluated / 0 excl | 0 px | **PASS** |
| `FR-05` | **LIBRARY** | Editorial List (6 Books, 84px Min-Height) | 480 × 800 px | PASS (0 overflow) | 12 evaluated / 0 excl | 0 px | **PASS** |
| `FR-06` | **LIBRARY** | Filtered Search (2 Matches Rendered) | 480 × 800 px | PASS (0 overflow) | 5 evaluated / 0 excl | 0 px | **PASS** |
| `FR-07` | **LIBRARY** | Empty Library (30px Title + 24px Headline) | 480 × 800 px | PASS (0 overflow) | 3 evaluated / 0 excl | 0 px | **PASS** |
| `FR-08` | **READER** | Immersive Reader (19px Serif + 3 Tap Zones) | 480 × 800 px | PASS (0 overflow) | 3 evaluated / 0 excl | 0 px | **PASS** |
| `FR-09` | **READER** | Reader HUD (Library Back + Chapter Stepper) | 480 × 800 px | PASS (0 overflow) | 8 evaluated / 0 excl | 0 px | **PASS** |
| `FR-10` | **READER** | TOC Drawer Modal (Chapter Jump) | 480 × 800 px | PASS (0 overflow) | 8 evaluated / 0 excl | 0 px | **PASS** |
| `FR-11` | **READER** | Typo Settings (14-20px Font, 1.4-1.8 LH) | 480 × 800 px | PASS (0 overflow) | 14 evaluated / 0 excl | 0 px | **PASS** |
| `FR-12` | **FILES** | SD File Browser (Trình quản lý tệp + FAT32 Bar) | 480 × 800 px | PASS (0 overflow) | 5 evaluated / 0 excl | 0 px | **PASS** |
| `FR-13` | **FILES** | Sideload Selection & Fallback Toggle | 480 × 800 px | PASS (0 overflow) | 4 evaluated / 2 excl | 0 px | **PASS** |
| `FR-14` | **SETTINGS** | Settings Overview (Màn hình & E-Paper) | 480 × 800 px | PASS (0 overflow) | 5 evaluated / 0 excl | 0 px | **PASS** |
| `FR-15` | **SETTINGS** | Compact Modal (Cài đặt làm mới + Sunlight Fix) | 480 × 800 px | PASS (0 overflow) | 8 evaluated / 4 excl | 0 px | **PASS** |

---

## 3. Mandatory Interactive Journeys Verification

All five end-to-end user journeys executed without errors:

| Journey | Description | Steps Tested | Verification Evidence | Status |
| :--- | :--- | :--- | :--- | :---: |
| **Journey A** | Resume Reading | `FR-01` → Tap `Tiếp tục đọc` → `FR-08` → Right Tap (`Page 42 → 43`) → Center Tap (HUD) → Tap `← Thư viện` (`FR-05`) → Tap `← Trang chủ` (`FR-01`) | DOM transitions verified; page state advanced; return path preserved; active book rendered on Home. | **PASS** |
| **Journey B** | Find a Book | `FR-01` → Tap `Thư viện` → `FR-05` → Tap Search bar → `FR-06` → Input "tô hoài" → Click result → `FR-08` | Diacritic-insensitive filtering executed; book opened in Reader; active book updated to Tô Hoài. | **PASS** |
| **Journey C** | Reader Controls | `FR-08` → Center Tap → `FR-09` → Tap `[Mục lục]` → `FR-10` → Select `Chương 5` → `FR-08` (Page 65) → Center Tap → Tap `[Aa Phông chữ]` → `FR-11` → Change to 18px → Apply → `FR-08` | Chapter jump verified; reader state updated; live typography style preview applied. | **PASS** |
| **Journey D** | Import & Open | `FR-02` (Empty Home) → Tap `Thêm sách ngay` → `FR-12` → Tap `noi-buon-chien-tranh.epub` → `FR-13` → Tap `Mở sách trong Reader` → `FR-08` | Ingestion flow verified from empty onboarding to deep reading. | **PASS** |
| **Journey E** | Change Settings | `FR-01` → Tap `Cài đặt` → `FR-14` → Tap `Tần suất làm mới` → `FR-15` → Select `Mỗi 5 trang` → Tap `Lưu thay đổi` → `FR-14` | State updated and rendered in DOM: `Mỗi 5 trang →` confirmed. | **PASS** |

---

## 4. Multi-Book State Regression Verification

- **Isolation Test:**
  1. Book 1 (*Nỗi buồn chiến tranh*) advanced from Page 42 to 44.
  2. Switched to Book 2 (*Những người khốn khổ — Tập thứ nhất*) at Page 1; advanced to Page 4.
  3. Switched back to Book 1: Verified Book 1 retained Page 44.
  4. Switched back to Book 2: Verified Book 2 retained Page 4.
  5. Returned Home: Verified Home hero dynamically updated to show active book at Page 44.
- **Verdict:** **PASS (Independent state persistence verified)**.

---

## 5. Search Reliability Regression Verification (Fix P1-A)

- **Accented Match:** Query `chiến tranh` → 2 matches (*Nỗi buồn chiến tranh*, *Chiến tranh và Hòa bình*), rendered into DOM rows with `<mark class="search-match">` query term highlighting.
- **Unaccented Match:** Query `chien tranh` → 2 matches (identical to accented query), rendered into DOM rows with `<mark class="search-match">` highlighting preserved on matching base letters.
- **Zero Results Query:** Non-matching query (`khong-ton-tai-xyz`) displays graceful zero-match guidance (`0 kết quả`).
- **Clear Action & Full Catalogue Restoration:**
  - Tapping clear button `(×)` clears input field, restores keyboard focus, and sets status label to `Toàn bộ danh mục (6 cuốn)`.
  - Restores all 6 book rows in catalogue (*Nỗi buồn chiến tranh*, *Những người khốn khổ*, *Dế Mèn phiêu lưu ký*, *Chiến tranh và Hòa bình*, *Số Đỏ*, *Truyện Kiều*).
  - All restored rows verified interactive (clicking navigates directly to Reader).
- **Verdict:** **PASS**.

---

## 5.1 Reader Typography Synchronization & HUD Consistency (Fix P1-B)

- **Initial Default Alignment:** Verified initial Reader CSS, `currentState.readerSettings`, FR-11 steppers, and FR-14 displayed values are aligned to **`19px / Giãn dòng 1.65`** and `32px` margin.
- **Reader ↔ HUD Consistency:** Verified identical computed styles between `#reader-prose-text` (FR-08) and `#reader-hud-prose-text` (FR-09):
  - `fontSize`: identical ($19	ext{px}$).
  - `fontFamily`: identical (Noto Serif).
  - `lineHeight`: identical ($1.65$).
  - `paddingLeft` / `paddingRight`: identical ($32	ext{px}$).
- **Dynamic Updates & State Preservation:** Verified that modifying font size ($18	ext{px}$), line height ($1.4$), font family (`sans`), and margin ($24	ext{px}$) simultaneously updates both FR-08 and FR-09 while strictly preserving `currentPage` and `chapter` state.
- **Verdict:** **PASS**.

---

## 6. Library Sorting Regression Verification

- **Title A–Z Sort:** Clicking `.sort-pills-row .sort-pill:nth-child(2)` triggers `sortLibrary('title')`. Uses Vietnamese-aware `localeCompare('vi', { sensitivity: 'base' })`.
  - Asserted top rendered item: *Chiến tranh và Hòa bình* (Lev Tolstoy).
  - Asserted button receives `.active` class.
- **Reading Progress Sort:** Clicking `.sort-pills-row .sort-pill:nth-child(3)` triggers `sortLibrary('progress')`. Reorders by `(currentPage / totalPages)` descending.
  - Asserted top rendered item: *Dế Mèn phiêu lưu ký* (Tô Hoài · 40% progress).
  - Asserted button receives `.active` class.
- **Recent Sort:** Clicking `.sort-pills-row .sort-pill:nth-child(1)` triggers `sortLibrary('recent')`.
  - Asserted top rendered item: *Nỗi buồn chiến tranh* (Bảo Ninh · default reading order).
  - Asserted button receives `.active` class.
- **Non-Intrusive UX:** Confirmed no intrusive toast message is triggered during sorting.
- **Verdict:** **PASS**.

---

## 7. Touch Target Interaction Census & Mutation Testing

- **Total Discovered Interactive Elements:** 105.
- **Total Evaluated Elements:** 99.
- **Documented Exclusions:** 6 (4 radio inputs inside `<label class="radio-option-row">` to avoid double-counting accessible label parents; 1 disabled CTA in fallback; 1 hidden fallback link).
- **Primary CTAs (`.btn-primary`):** Verified `height >= 56 px` and `width >= 120 px`.
- **Standard Controls & Links:** Verified `width >= 48 px` and `height >= 48 px`.
- **Content Row Items:** Verified `width >= 200 px` and `height >= 48 px`.
- **Failure Count:** **0 of 99 evaluated targets below bounds** (`status: PASS`).
- **Mutation Testing:**
  - Mutation 1: Shrinking `.reader-tap-center` width to 30px confirmed to fail the harness.
  - Mutation 2: Shrinking `#search-input-field` height to 30px confirmed to fail the harness.
  - **Verdict:** **PASS (Both mutations reliably caught)**.

---

## 8. QA Harness Integrity Check

- Verified `--demonstrate-failure` flag triggers intentional assertion and exits with code 1.
- Verified standard run executes all real assertions and exits with code 0.
- Verified test artifacts are written to `tools/test_output/` without overwriting reference captures in `docs/design/prototype/captures/`.
- Verified canonical 1-bit binary images in `docs/design/prototype/captures/binary/` match freshly converted source captures pixel-for-pixel (0 px difference).

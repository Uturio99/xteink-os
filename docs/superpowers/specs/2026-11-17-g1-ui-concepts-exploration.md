# Cổng G1 — Khám phá 3 Concept UI/UX Swiss Editorial × Apple HIG trên CrossPoint

**Trạng thái:** `PROPOSED FOR PO SELECTION (G1)` — Chưa sửa bất kỳ file `.h`/`.cpp` firmware nào.  
**Nhánh / Worktree:** `experiment/antigravity-crosspoint-ui` (`G:/AI-Setup/XTEInk-X4-Pro/xteink-os-antigravity-ui`) từ `ff7a0e02a16c1855d533e8f52abbc9f907a8077c` (`freeink-sdk @ 946e6ac3`).  
**Bản xem trước trực quan 1:1 (480×800 E-Ink):** [g1-concepts-preview.html](g1-concepts-preview.html)

---

## 1. Ngôn ngữ Thiết kế Hợp nhất (Swiss Editorial Grid × Apple HIG Minimalism)

Rút kinh nghiệm từ phản hồi thẩm mỹ ở lần phác thảo trước, toàn bộ 3 concept G1 được tái thiết kế dựa trên **3 mẫu tham chiếu của PO** kết hợp **chuẩn nút & kiểm soát tối giản của Apple HIG**:

1. **Không gian thở & Kiến trúc Lưới (Swiss Editorial Grid):**
   - Duy trì **55–65% khoảng trắng cấu trúc**; lề ngoài `32px` (chỉ số nội dung `416px`), cột phụ bất đối xứng (`x = 136` hoặc `x = 248`), đường kẻ chia mảng `1px` mảnh tuyệt đối.
   - **Loại bỏ hoàn toàn** khung viền dày `2px`, bóng đổ cứng (`box-shadow: 2px 2px 0`), và các khối nút chữ nhật đen đặc choán hết chiều ngang màn hình.
2. **Hệ thống Nút & Điều khiển chuẩn Apple HIG (`FreeInkUI` Mapping):**
   - **Apple Filled Capsule (`height: 44px`, `radius: 255 / 9999px`):** Nút hành động chính nhỏ gọn theo nội dung (`w = 164..176px`), bo tròn hai đầu (stadium pill), nền đặc tương phản cao + nhãn sạch + mũi tên mảnh.
   - **Apple Circular Icon Button (`44×44px`, `radius: 22px`, stroke `1px`):** Dùng cho các nút điều hướng phân trang (`‹` / `›`), nút mở Mục lục, Quick-Sheet, hoặc nút Play/Đọc tiếp (`▶`).
   - **Apple Whisper-Quiet Bottom Nav (`height: 48px`):** Chỉ giữ đường kẻ `1px` phía trên, chữ mảnh sắc nét, trạng thái đang chọn đánh dấu bằng chấm `4px` (`fui::TabBarProps::selectedDotSize = 4`) hoặc gạch chân mảnh.
   - **1px Timeline Progress Scrubber:** Thay thế thanh tiến độ khối dày bằng đường `1px` thanh mảnh kèm vạch đứng `|` tại vị trí đọc và số trang dạng bảng (`tabular-nums`).

---

## 2. Phân tích Khả thi Native của 3 Concept

| Tiêu chí | Concept 1: **Atelier Typographic** *(Ref 1 + Apple HIG)* | Concept 2: **Focus Exhibition Stage** *(Ref 2 + `fui::sheet`)* | Concept 3: **Swiss Architectural Grid 01.1** *(Ref 3 + Circular Pager)* |
| :--- | :--- | :--- | :--- |
| **Đặc trưng thị giác** | Khoảng trắng lớn kiểu Ref 1, nhãn chỉ mục `01.1 / 01.2`, hero bên trái + bìa sách bên phải, Capsule CTA + 2 nút tròn `44×44` + danh sách gần đây tối giản. | Sân khấu bìa sách lớn ở nửa trên (`y=60..506`), tiêu đề toàn chiều rộng, đoạn mô tả lệch cột phải (`x=136`) để trống cột trái, scrubber `1px` + nút tròn `▶` + ngăn kéo đáy `fui::sheet`. | Đường kẻ lưới dọc `1px` lộ thiên (`x=32, 248, 448`), số liệu lớn nét mảnh (`62%`, `142+`), cặp nút phân trang tròn `‹` `›` `44×44`, 2 thẻ ảnh không viền ở đáy. |
| **Component `FreeInkUI` tái sử dụng** | `UiAppHost`, `fui::button` (stadium `radius=255` & circle `radius=22`), `fui::list`, `fui::tabBar` (`selectedDotSize=4`), `ThemeMetrics` + `uiThemeTokens()`. | `fui::coverCarousel`, `fui::button` (circle `▶`), `fui::sheet` (`SheetEdge::Bottom`, backed bởi `storeBwBuffer()`/`restoreBwBuffer()`), `fui::tileGrid`. | `fui::button` (capsule CTA + circular pager `‹`/`›`), `fui::metricCard` (cỡ chữ nhẹ), `fui::tabBar`, vẽ đường lưới dọc qua `DrawTarget::fill`. |
| **Phần mở rộng ở G2 (Đảo ngược được)** | Hàm bố cục `constexpr` Two-Anchor Clamped + presenter tối giản trong `CoverGridHomeUi`. | Trạng thái overlay `sheetOpen` trong `CoverGridHomeUi` + vẽ thanh scrubber `1px` trực tiếp trên `DrawTarget`. | Presenter chia 2 cột lưới Swiss + logic phân trang 2 thẻ sách (`offset += 2`) không cần cấp phát heap. |
| **Số slot `FreeInkApp<24, 6>`** | 1 (heroCover) + 1 (primaryPill) + 2 (circleBtns) + 2 (rows) + 4 (navBar) = **10 slots**. | 3 (carousel) + 2 (swipes) + 1 (btnPlay) + 4 (navBar) = **10 slots**; khi Sheet mở = **6 slots**. | 1 (ctaPill) + 1 (heroCover) + 1 (metric2) + 2 (pagers) + 2 (cards) + 4 (navBar) = **11 slots**. |
| **Tác động ESP32-C3 (`env:default`) vs X4 Pro (`env:x4pro`)** | **0 byte RAM thêm.** Tương thích tuyệt đối mọi bo (C3 & S3). Rủi ro ghosting thấp nhất nhờ tỷ lệ trắng cao. | Cần PSRAM cho bìa cỡ lớn `184×276` trên X4 Pro; khi mở `fui::sheet` dùng `storeBwBuffer()`/`restoreBwBuffer()` giữ nguyên vùng vẽ nền. | Cần font mảnh/chỉ số đẹp ở cỡ lớn (nếu dùng built-in font ở G2 cần đo kích thước thực tế trên G4 trước khi thêm cỡ font mới). |

---

## 3. Tọa độ Đã Kiểm chứng Toán học (`X4 Pro Portrait 480×800`, Safe-Area `{9, 7, 3, 7}` $\rightarrow$ `{7, 9, 466, 788}`)

Đã kiểm chứng bằng Python trên ma trận **30 tổ hợp** (5 cấu hình `contentArea`: X4 Pro Portrait, Zero-Inset Portrait, Portrait Inverted, Landscape CW, Landscape CCW × 2 cỡ chữ `29/24` và `34/26`): **30/30 đạt — 0 chồng lấn, 100% nằm trọn trong `contentArea`, mọi mục tiêu chạm $\ge 44\times 44\text{px}$**.

- **Concept 1 — *Atelier Typographic* (`29/24`):**
  - `topBar`: `(32, 20, 416, 28)` | `indexRow`: `(32, 60, 416, 24)`
  - `heroTitle`: `(32, 108, 272, 87)` | `heroAuthor`: `(32, 203, 272, 24)` | `heroScrubber`: `(32, 239, 272, 34)` | `heroCover`: `(324, 108, 124, 186)`
  - `primaryPill`: `(32, 314, 176, 44)` | `btnToc`: `(352, 314, 44, 44)` | `btnSheet`: `(404, 314, 44, 44)`
  - `shelfHdr`: `(32, 386, 416, 32)` | `row_0`: `(32, 428, 416, 68)` | `row_1`: `(32, 504, 416, 68)` (`bottom=572`)
  - `navBar`: `(32, 740, 416, 48)` (`bottom=788 <= 797`)
- **Concept 2 — *Focus Exhibition Stage* (`29/24` & `34/26`):**
  - `topBar`: `(32, 20, 416, 28)`
  - `stageFrame`: `(32, 60, 416, 446)` ở `29/24` — tự co thành `(32, 60, 416, 432)` ở `34/26` để không chạm phần chữ bên dưới
  - `titleBlk`: `(32, 530, 416, 58)` | `indentedMeta`: `(136, 600, 312, 56)` (chừa trắng `x=32..136`)
  - `btnPlay`: `(32, 676, 44, 44)` | `scrubberTrack`: `(92, 676, 356, 44)`
  - `navBar`: `(32, 740, 416, 48)`
- **Concept 3 — *Swiss Architectural Grid 01.1* (`29/24`):**
  - `topBar`: `(32, 20, 416, 28)`
  - `heroTitle`: `(32, 64, 260, 87)` | `heroSub`: `(32, 159, 260, 24)` | `ctaPill`: `(32, 220, 164, 44)` | `heroCover`: `(312, 64, 136, 200)`
  - `metric1`: `(32, 288, 200, 76)` | `metric2`: `(248, 288, 200, 76)`
  - `carouselLbl`: `(32, 382, 312, 44)` | `btnPrev`: `(352, 382, 44, 44)` | `btnNext`: `(404, 382, 44, 44)`
  - `card1`: `(32, 438, 200, 286)` | `card2`: `(248, 438, 200, 286)` (`bottom=724`)
  - `navBar`: `(32, 740, 416, 48)`

---

## 4. Điểm cần PO Quyết định để Mở Cổng G2

1. **Chọn hướng thẩm mỹ cho PoC G2:** Concept 1 (*Atelier Typographic*), Concept 2 (*Focus Exhibition Stage*), Concept 3 (*Swiss Architectural Grid 01.1*), hay ghép phần tốt nhất của hai concept (ví dụ: hệ thống lưới & chỉ số Concept 3 + `1px` scrubber & `fui::sheet` của Concept 2 + sự thoáng đãng của Concept 1)?
2. **Phạm vi cài đặt C++ ở G2:** Chỉ dựng PoC màn hình Home trên Simulator (`simulator_x4_pro`), hay đồng thời áp dụng token nút Apple (capsule `radius = 255` + hairline `1px`) sang `UIThemeTokens.h` cho toàn bộ danh sách Library/Settings?

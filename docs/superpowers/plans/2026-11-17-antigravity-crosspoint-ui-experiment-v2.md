# Kế hoạch Triển khai V2 — Thí nghiệm Kỹ thuật Giao diện CrossPoint trên Antigravity

> **Historical model-routing instructions:** All role and model-routing instructions in this plan, including the Gemini, no-Claude, single-writer, and Scout instructions, are historical only and preserved for provenance; they are not current operating instructions. Follow `.agents/rules/ui-experiment.md` for current roles and routing. Existing safety, technical verification, and review gates remain binding.

> **For agentic workers:** Áp dụng quy trình phân quyền cứng tại `.agents/rules/ui-experiment.md`. Một Builder duy nhất có quyền ghi tại một thời điểm (chỉ sửa code C++ từ Cổng G2 trở đi, sau khi PO phê duyệt hướng thiết kế ở G1). Tuyệt đối không dùng Claude CLI/subagent hoặc AI co-author trailer.

**Goal:** Khởi tạo môi trường thí nghiệm giao diện CrossPoint cô lập hoàn toàn trên nhánh `experiment/antigravity-crosspoint-ui` (`G:/AI-Setup/XTEInk-X4-Pro/xteink-os-antigravity-ui`), khám phá 3 concept UI/UX thế hệ mới trên nền FreeInkUI/CrossPoint kèm chứng minh hình học không chồng lấn, và phát triển theo các cổng phê duyệt PO từ G0 đến G4.

**Architecture:** Giữ nguyên toàn bộ nền tảng bất biến của CrossPoint (HAL, `HalFile`, `storageMutex`, `RenderLock`, `ActivityManager`, single framebuffer, cache `/.crosspoint/`) và `freeink-sdk @ 946e6ac3`. Kế thừa hạ tầng Simulator X4 Pro (`2dbf8155`), nguồn hợp đồng F01–F44 (`REC-01`), và NAV-001 từng phần (`372ccca1`) từ `ff7a0e02` (`feat/noveos-native`) mà không làm thay đổi worktree NoveOS chuẩn. Toàn bộ bố cục sử dụng mô hình **Two-Anchor Clamped Flow** dẫn xuất từ `UITheme::getContentArea(renderer)` và chiều cao dòng chữ đo thực tế.

**Tech Stack:** C++20 (`-std=c++2a`, `-fno-exceptions`, no RTTI), FreeInkUI (`fui::*`), GoogleTest (CMake/CTest trên WSL), PlatformIO (`env:simulator_x4_pro`, `env:x4pro`, `env:default`, `env:x4c`).

**Authority Chain:**
1. `AGENTS.md`, `.agents/rules/*.md`, và `freeink-sdk @ 946e6ac3` (giới hạn phần cứng & HAL).
2. Quyết định phê duyệt từng cổng của Product Owner (G0 $\rightarrow$ G1 $\rightarrow$ G2 $\rightarrow$ G3 $\rightarrow$ G4).
3. `.agents/rules/ui-experiment.md` (biên cô lập, quy tắc cổng, phân quyền mô hình, 6 tầng xác minh).
4. `docs/EXECUTION_CONTRACT.md` & `docs/superpowers/audits/STEP-03-FEATURE-INVENTORY.md` (danh sách bảo toàn F01–F44).

---

## Chính sách Định tuyến Mô hình & Ngân sách Thực thi

- **Orchestrator (Frontier Model):** Lập kế hoạch, đánh giá đánh đổi kiến trúc, duyệt tiêu chuẩn chuyển cổng G0–G4, giải quyết leo thang.
- **Builder (Capable Lower-Cost Model, Single Writer):** Thực hiện gói việc đã được duyệt trên worktree `xteink-os-antigravity-ui`, chạy kiểm tra, lưu log/ảnh/SHA-256. **Quy tắc 2 lần (2-Strike Rule):** tối đa 2 lần thử sửa trên cùng một giả thuyết thất bại trước khi dừng và trả lại Orchestrator.
- **Scout / Routine Verifier (Fast Lightweight Read-Only):** Quét symbol, kiểm tra tham chiếu F01–F44 và liên kết tài liệu.
- **Reviewers (Read-Only Subagents):**
  - G2–G3: 1 Reviewer đọc-duy-nhất kiểm tra đối chiếu hợp đồng gói việc và chất lượng diff.
  - G4: Đủ 4 trục đọc-duy-nhất độc lập (`review-correctness`, `review-architecture`, `review-embedded`, `review-i18n-docs`) theo `.agents/skills/firmware-handoff/SKILL.md` (tối đa 2 vòng).
- **No Claude Involvement:** Cấm gọi Claude CLI/subagent và cấm thêm trailer `Co-Authored-By: Claude...`.

---

## Sáu Tầng Xác minh Độc lập (Không suy diễn bắc cầu)

| Tầng | Nhãn | Phạm vi kiểm chứng | Tiêu chí đạt |
| :---: | :--- | :--- | :--- |
| 1 | `HOST_TEST_VERIFIED` | CMake + CTest (`GoogleTest`) | 100% test pass; chứng minh không chồng lấn (`!homeRectsOverlap`), lọt vùng an toàn (`homeRectContains`), chạm $\ge 44\text{px}$ ở cả 4 hướng xoay. |
| 2 | `SIMULATOR_BUILD_VERIFIED` | `pio run -e simulator_x4_pro` từ nguồn Git hợp lệ | Exit 0, sinh binary mới, ghi nhận đường dẫn build, trạng thái cache và `SHA-256`. |
| 3 | `SIMULATOR_RENDER_VERIFIED` | Chạy binary khớp SHA-256 ở Tầng 2 với SD fixture cô lập | Ảnh chụp framebuffer thực tế cho các trạng thái: trống, có sách, tiếng Việt NFC/NFD dài, thiếu bìa, xoay màn hình. |
| 4 | `SIMULATOR_INTERACTION_VERIFIED` | Thao tác cảm ứng và phím vật lý trên Simulator | Log + ảnh xác nhận nhấn/nhả, long-press không rò short-release, chuyển Activity và quay về đúng trạng thái. |
| 5 | `FIRMWARE_BUILD_VERIFIED` | `clang-format`, `pio check`, build `x4pro`, `default`, `x4c` | Exit 0 mọi target, 0 defect, bảng delta RAM tĩnh và Flash so với `ff7a0e02`. |
| 6 | `HARDWARE_VERIFIED` | Do PO kiểm thử trên X4 Pro thật | Kiểm tra ghosting, độ nét e-ink, GT911 touch, phím cứng, và serial log `HalMemory` qua nhiều chu kỳ. |

---

## Kế hoạch 5 Cổng (Gates G0 – G4)

### Gate G0 — Cô lập Worktree, Quy tắc Antigravity & Baseline Audit (Không sửa C++ Firmware)
- [x] Khởi tạo `G:/AI-Setup/XTEInk-X4-Pro/xteink-os-antigravity-ui` trên nhánh `experiment/antigravity-crosspoint-ui` tại `ff7a0e02`.
- [x] Khởi tạo cục bộ `freeink-sdk @ 946e6ac3` và `lucide @ c81680e0`.
- [x] Tạo `.agents/rules/ui-experiment.md` (kèm YAML frontmatter `trigger: always_on`) và liên kết trong `AGENTS.md`.
- [ ] Chạy baseline CTest tại `~/.cache/ag-crosspoint-ui/ctest-baseline` để ghi nhận số liệu gốc `HOST_TEST_VERIFIED`.
- **Gate G0 Exit:** Không sửa bất kỳ file `.h`/`.cpp` firmware nào; các worktree khác nguyên vẹn 100%.

### Gate G1 — Khám phá 3 Concept UI/UX Mở & Kiểm chứng Hình học Độc lập (Chưa sửa C++)
- [ ] Xây dựng đặc tả `docs/superpowers/specs/2026-11-17-g1-ui-concepts-exploration.md` phân tích 3 hướng:
  1. **Concept 1 — *Swiss Editorial Index*** (`UiAppHost`, `fui::button`, `fui::list`, `fui::tabBar`, bố cục Two-Anchor Clamped Flow).
  2. **Concept 2 — *Cover Carousel & Quick-Sheet OS*** (`fui::coverCarousel`, `fui::publicationHeader`, `fui::sheet` + `storeBwBuffer()`/`restoreBwBuffer()`).
  3. **Concept 3 — *Reading Workbench & Split-Grid*** (`fui::bookCard`, `fui::metricCard`, `fui::tileGrid`, `fui::readerChrome`, `fui::tapZones`).
- [ ] Dựng mockup trực quan 1:1 chuẩn 480×800 E-Ink (`docs/superpowers/specs/g1-concepts-preview.html`) với trạng thái trống, tiếng Việt NFC/NFD và trạng thái focus.
- [ ] Kiểm chứng toán học toàn bộ tọa độ 3 concept cho font thường (`29/24`), font lớn (`34/26`) và hướng Portrait/Landscape (đảm bảo `overlap == 0` và `content.bottom` không bị tràn như lỗi `788 > 772` của baseline hay `735 > 732` của V1).
- **Gate G1 Exit (Cổng phê duyệt PO):** PO chọn 1 hướng UI/UX (hoặc phương án kết hợp) và chốt phạm vi file C++ được phép sửa cho G2.

### Gate G2 — Cài đặt Native PoC Tối giản & Có thể Đảo ngược (Chỉ chạy sau khi PO duyệt G1)
- [ ] Viết GoogleTest cho hàm tính bố cục Clamped và kiểm tra không chồng lấn trên 4 góc xoay (Red $\rightarrow$ Green, `HOST_TEST_VERIFIED`).
- [ ] Hiện thực hóa PoC trên mã nguồn C++ theo nguyên tắc tối giản Ponytail (tái sử dụng `fui::*` primitives, 0 cấp phát heap khi vẽ, có nhánh/presenter dễ đảo ngược).
- [ ] Biên dịch `simulator_x4_pro` (`SIMULATOR_BUILD_VERIFIED`), chụp framebuffer các trạng thái mẫu (`SIMULATOR_RENDER_VERIFIED`), kiểm tra tương tác touch/button (`SIMULATOR_INTERACTION_VERIFIED`).
- [ ] Gửi diff cho 1 Read-Only Spec/Quality Reviewer.
- **Gate G2 Exit (Cổng phê duyệt PO):** PO nghiệm thu hình ảnh và tương tác từ Simulator trước khi mở rộng sang G3.

### Gate G3 — Mở rộng UI & Bảo toàn Hợp đồng CrossPoint (F01–F44)
- [ ] Đồng bộ ngôn ngữ thị giác đã duyệt sang Library/Lists/Header/Overlays trong phạm vi được cấp phép.
- [ ] Kiểm tra hiển thị tiếng Việt NFC/NFD (`utf8_compose`, `combining_marks`, `library_text`), `thumbHeightFor`, và ma trận input từ `.agents/rules/ui-activities.md`.
- [ ] Xác minh các route F01–F44 (`Home <-> Library <-> Reader`, `Files`, `Settings`, `Transfer`, `Plugins`) và guard bo mạch (`FREEINK_CAP_USB_MSC`, `BoardConfig::hasTouch()`, `supportsCoverGrid()`).
- **Gate G3 Exit (Cổng phê duyệt PO):** Toàn bộ test host và kịch bản tương tác Simulator xanh 100%.

### Gate G4 — Kiểm chứng Đa Bo & Firmware Handoff 4 Trục
- [ ] Chạy `./bin/clang-format-fix`, `pio check`, và build `x4pro` (ESP32-S3), `default` (ESP32-C3), `x4c` (`FIRMWARE_BUILD_VERIFIED`).
- [ ] Lập bảng so sánh RAM tĩnh / Flash so với baseline `ff7a0e02`.
- [ ] Chạy đủ 4 Read-Only Review Subagents theo `firmware-handoff` (tối đa 2 vòng).
- [ ] Bàn giao kịch bản kiểm thử phần cứng cho PO thực hiện `HARDWARE_VERIFIED` trước khi xét commit cục bộ hoặc flash.

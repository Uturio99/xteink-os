---
name: systematic-debugging
description: >
  Dùng khi debug lỗi firmware CrossPoint Reader: crash, HardFault, lỗi
  render e-ink, memory corruption, hoặc hành vi sai. Truy nguyên nhân
  gốc trước khi sửa — cấm sửa theo phán đoán.
license: MIT
---

# Debug có hệ thống

> Chuyển thể từ obra/superpowers/skills/systematic-debugging (MIT).

## Nguyên tắc cốt lõi

**Hiểu trước khi sửa.** Mỗi lần "thử một fix xem sao" mà không hiểu
nguyên nhân là một lần thêm rác vào codebase. Đặc biệt nguy hiểm trên
firmware, nơi bug có thể corrupt memory hoặc brick thiết bị.

## Quy trình 4 pha

### Pha 1: Tái hiện

- Tìm bước tái hiện tối thiểu, đáng tin cậy.
- Nếu không tái hiện được: ghi chính xác điều kiện (build nào,
  hardware hay simulator, input gì, trạng thái gì trước đó).
- Ghi lại: lệnh build, input, output mong đợi so với thực tế.

### Pha 2: Thu hẹp

- **Kiểm tra thay đổi gần nhất trước tiên**: `git log`, `git diff` —
  bug mới hầu như luôn do thay đổi gần đây.
- Đọc thông báo lỗi nguyên văn. Không đoán nghĩa — tra tài liệu.
- Chia đôi: tách dần cho đến khi phạm vi lỗi hẹp nhất.
- Với simulator: thêm log thẳng vào chỗ nghi vấn.
- Với hardware: uart log, hoặc bỏ về simulator nếu tái hiện được.

### Pha 3: Giả thuyết nguyên nhân gốc

- Nêu giả thuyết cụ thể: X xảy ra vì Y, kiểm chứng bằng Z.
- Kiểm chứng bằng instrumentation (log/assert), không phải bằng fix.
- Nếu giả thuyết sai: giả thuyết mới, không sửa đại.
- Nguyên nhân gốc = thay đổi nào khiến hành vi sai, không phải chỗ
  hiện triệu chứng.

### Pha 4: Sửa và chứng minh

- Sửa nguyên nhân gốc, không che triệu chứng.
- Chứng minh: reproducer FAIL trước fix, PASS sau fix (xem skill
  verification-before-completion).
- Tìm cùng lớp lỗi ở chỗ khác trong codebase.

## Kịch bản embedded thường gặp

| Triệu chứng | Kiểm tra đầu tiên |
|---|---|
| HardFault / crash | Stack overflow, con trỏ null/hạng rác, ghi ngoài vùng buffer |
| Render sai / nhòe e-ink | Dirty-region tracking, waveform mode, framebuffer overlap |
| Leak / hết heap | Log alloc/free từng cặp; xem skill heap-discipline |
| Treo ngẫu nhiên | Race condition, watchdog reset (xem SilentRestart.h), interrupt stack |
| Sai sau refactor | Interface đổi không đồng bộ giữa HAL và caller |

## Cờ đỏ — DỪNG LẠI

- "Thử sửa nhanh xem sao"
- Sửa mà không tái hiện được lỗi
- Không đọc thông báo lỗi nguyên văn
- Đổ lỗi cho hardware/toolchain khi chưa loại trừ code
- Thêm return sớm hoặc bỏ call để "cho chạy được"

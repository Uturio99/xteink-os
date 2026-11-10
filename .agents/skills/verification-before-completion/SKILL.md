---
name: verification-before-completion
description: >
  Dùng trước khi tuyên bố bất kỳ task nào hoàn thành, đã sửa, hoặc pass
  trong firmware CrossPoint Reader. Yêu cầu bằng chứng xác minh tươi
  (build, test, static checks) trước khi tuyên bố thành công.
license: MIT
---

# Xác minh trước khi hoàn thành

> Chuyển thể từ obra/superpowers/skills/verification-before-completion (MIT).

## Nguyên tắc cốt lõi

**Bằng chứng trước lời nói.** Không được nói task đã xong, bug đã sửa,
build đã pass, hoặc test đã pass cho đến khi bạn đã chạy lệnh xác minh
trong phiên này và đọc output của nó.

## Cổng xác minh

Trước mọi tuyên bố hoàn thành:

1. **Xác định** lệnh chứng minh tuyên bố.
2. **Chạy** toàn bộ lệnh đó (fresh).
3. **Đọc** output và exit code.
4. **Xác minh** output khớp với tuyên bố.
5. **Sau đó** mới tuyên bố, trích dẫn bằng chứng.

## Áp dụng cho dự án này

| Tuyên bố | Bằng chứng bắt buộc |
|---|---|
| Build simulator | `pio run -e simulator_x4_pro` exit 0 |
| Build hardware | `pio run -e xteink_x4_pro` exit 0 |
| Test pass | `pio test -e native` pass |
| Format sạch | `pio run -t clangformat -e simulator_x4_pro` hoặc check thủ công |
| Bug đã sửa | Reproducer FAIL trước fix, PASS sau fix |
| An toàn memory | Heap trong ngân sách; không alloc static mới không có lý do |

## Cờ đỏ — DỪNG LẠI

- "Giờ thì chắc được rồi"
- "Trông đúng rồi"
- "Có lẽ pass"
- Tin vào lần chạy trước
- Tin báo cáo của agent mà không kiểm tra diff/output

## Format output

```
[XÁC MINH]
- Lệnh: <lệnh chính xác>
- Exit code: <N>
- Kết quả: PASS / FAIL / PARTIAL
- Bằng chứng: <tóm tắt output một dòng>
```

Nếu FAIL, nêu rõ gì bị hỏng và bước tiếp theo. Không tuyên bố hoàn thành.

---
name: writing-plans
description: >
  Dùng khi người dùng đưa ra task hoặc spec nhiều bước cho CrossPoint
  Reader. Viết plan triển khai ngắn gọn trước khi chạm code.
license: MIT
---

# Viết plan triển khai

> Chuyển thể từ obra/superpowers/skills/writing-plans (MIT).

## Khi nào dùng

- Feature mới chạm hơn 2 file
- Refactor xuyên module
- Tối ưu memory/performance
- Bất kỳ task nào người dùng nói "plan first" hoặc dùng nhiều harness

## Output

Lưu plan vào:
`docs/plans/YYYY-MM-DD-<ten-ngan>.md`

Nếu `docs/plans/` chưa có, tạo mới.

## Header plan

```markdown
# Plan triển khai <Tinh nang>

**Muc tieu:** mot cau
**Kien truc:** 2-3 cau
**Spec / tham quyen:** duong dan den design doc hoac yeu cau nguoi dung
**Rang buoc toan cuc:**
- ESP32-C3/S3, khoang 380 KB RAM, khong PSRAM
- Tuan thu AGENTS.md va cac skill rule hien co
- Uu tien static allocation; bien minh moi heap alloc moi
- Duy tri build PlatformIO simulator + hardware
```

## Cấu trúc task

Chia thành các task, mỗi task:

- Chạm một nhóm file nhỏ, liên quan
- Có deliverable kiểm tra được
- Có thể xác minh độc lập
- Kết thúc bằng một commit

Mỗi task:

```markdown
### Task N: <Ten>

**File:**
- Tao: `path`
- Sua: `path:line-range`
- Test: `path`

**Interface:**
- Consume: ...
- Produce: ...

- [ ] Buoc 1: <hanh dong cu the>
- [ ] Buoc 2: <lenh xac minh>
```

## Quy tắc

- DRY, YAGNI
- Không plan feature không được yêu cầu
- Tôn trọng pattern hiện có trong `src/` va `lib/`
- Ghi chú tác động memory/heap cho thay đổi embedded
- Nếu task dành cho harness khác (Codex/Antigravity), dùng skill
  orchestrator-handoff để viết brief thay vì tự làm.

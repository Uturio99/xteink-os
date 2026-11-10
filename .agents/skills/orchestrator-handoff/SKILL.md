---
name: orchestrator-handoff
description: >
  Dùng khi người dùng định tuyến task qua ChatGPT orchestrator đến
  nhiều harness (Codex, Antigravity, Kimi). Format output để orchestrator
  chuyển tiếp nguyên văn.
---

# Handoff qua Orchestrator

## Khi nào dùng

Người dùng dùng ChatGPT làm orchestrator: ChatGPT chia task thành
sub-task và chuyển tiếp từng sub-task đến harness chuyên biệt
(Codex, Antigravity, Kimi, hoặc Abacus AI harness này).

Dùng skill này khi:
- Người dùng nói task dành cho harness khác
- Người dùng nói "route sang Codex/Antigravity/Kimi"
- Bạn hoàn thành việc ở đây và cần handoff sang harness khác
- Bạn nhận handoff từ harness khác và cần tiếp tục

## Role tag

Bắt đầu response bằng đúng một tag:

- `[TO_CODEX]` — code tổng quát, refactor đa file, scaffolding
- `[TO_ANTIGRAVITY]` — UX/UI, layout, thiết kế visual e-ink
- `[TO_KIMI]` — embedded sâu, review memory/heap, QA gate
- `[TO_ALL]` — gửi đến tất cả harness
- `[TO_ORCHESTRATOR]` — câu hỏi hoặc nhận xét meta cho ChatGPT

## Format handoff

```markdown
[TO_<ROLE>]

## Tom tat ngu canh
2-3 câu về những gì đã xảy ra.

## File / artifact
- `path/to/file.cpp` — đã đổi/tạo
- `docs/plans/YYYY-MM-DD-plan.md` — file plan
- `.superpowers/sdd/progress.md` — ledger (nếu dùng)

## Instruction cu the
Chính xác harness mục tiêu phải làm gì tiếp theo. Bao gồm tên hàm,
đường dẫn file, line range, giá trị cụ thể.

## Tieu chi thanh cong
Orchestrator biết sub-task đã xong bằng cách nào.

## Lenh xac minh (neu co)
Lệnh chính xác cần chạy trước khi tuyên bố hoàn thành.
```

## Nhận handoff

Khi nhận tin nhắn có tag chỉ đến harness này:

1. Đọc tất cả artifact được liệt kê.
2. Xác nhận hiểu trong một dòng.
3. Thực hiện instruction cụ thể.
4. Trả kết quả với `[TO_ORCHESTRATOR]` hoặc role tiếp theo.

## Ràng buộc

- Không làm thay việc của Codex hay Antigravity inline trừ khi được yêu cầu.
- Handoff phải tự chứa: harness không cần toàn bộ lịch sử phiên.
- Ghi rõ ruling và blocker vào ledger hoặc artifact handoff.
- Ưu tiên artifact file hơn paste context dài.
- Khi kết thúc phiên làm việc ở đây, dùng write_project_memory ghi
  trạng thái để phiên sau tiếp tục.

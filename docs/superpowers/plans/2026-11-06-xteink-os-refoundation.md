# XTEInk OS X4 Pro: product contract and recovery plan

> Status: PROPOSED for Product Owner review, 2026-11-06. Architecture choice A was selected in conversation. This document does not accept WP2.4, authorize flashing, or replace the firmware handoff gates.

## Product contract

XTEInk OS is the user-facing operating experience for one first user on the X4 Pro. It must retain every working CrossPoint function and book format while offering a distinct, modern, restrained reading flow. The current product target is the X4 Pro; support for other devices is a later decision. Existing shared code must continue to build and behave correctly for supported boards.

The Product Owner chose a separate XTEInk UI layer inside the current repository, using the existing CrossPoint activities, FreeInkUI, input manager, storage, Reader, parsers, network, USB transfer, OPDS, plugins, settings, recovery and SDK. Keep upstream-sensitive code unchanged unless a demonstrated requirement needs a narrow, separately reviewed exception. A modular app surface should expose existing capabilities through stable activity/navigation contracts; it is not a second runtime or plugin engine.

Visual direction: modern, luxurious, minimal, monochrome e-paper UI informed by the supplied editorial reference. Prior M1 frames and Folio/Index explorations are reference material for the redesign, not automatic authority for final screens. Optimize the experience while retaining the best practical image quality. The requested 90% floor needs an agreed comparison method and real panel observation before it can be marked VERIFIED.

Vietnamese must render clearly, with complete diacritics and consistent text metrics. Touch, swipe and physical Home/Back actions must mean the same thing across Home, Library and Reader where those actions apply. Prefer fast partial refresh for small UI changes and high-quality full refresh for reading when the device and controller evidence support that policy. PSRAM may cache bounded images/glyphs on X4 Pro, with explicit failure/fallback paths and separate internal-heap accounting. None of these performance or panel outcomes is assumed from a prototype or simulator.

## Recovery snapshot and authority

The immutable reference copy is `G:/AI-Setup/XTEInk-X4-Pro-backup-20261106-preinventory/`: 40,122 source files matched their copied paths and SHA-256 hashes; all 40,123 backup files, including the manifest, have the read-only attribute. It shares drive G: with the source, so it is a recovery checkpoint rather than an off-drive disaster backup.

| Asset | Current role |
| --- | --- |
| `xteink-os` at `feat/m2-wp2-home@19f46d45` | Current checked-out firmware history; four untracked local skills are preserved. |
| `xteink-os-wp24-fixes` at `feat/wp24-fixes@f6c9151e` | Preserved parser/Home/progress/tests and proposed plan WIP; do not merge wholesale. |
| `xteink-os-wp24-home-lean` at `feat/wp24-home-lean@8a36e75e` | Preserved isolated Home candidate with uncommitted source/tests; not accepted as the new OS. |
| M0/M1 documents, 15 frames, fonts, and browser prototype | Historical design and verification evidence; use as reference and traceability. |
| `.qa-review-tmp` screenshots/build logs and H1.1B launcher | Historical evidence; neither proves the final firmware or hardware behavior. |
| `freeink-sdk` at `946e6ac3` | Pinned hardware dependency; no automatic update. |

Previous documents that call M1 frozen or direct execution to WP2.4 describe the earlier program. This contract has Product Owner authority over the new direction once accepted. Preserve those documents and record any supersession explicitly; do not silently rewrite history.

## Architecture and file ownership

| Boundary | Existing entry points | Planned responsibility |
| --- | --- | --- |
| Core and hardware | `lib/Epub/`, `lib/Txt/`, `lib/Xtc/`, `lib/hal/`, `freeink-sdk/`, `src/activities/reader/` | Keep reading, formats, storage, display and input behavior. Change only through a separately scoped exception. |
| Navigation and data adaptation | `src/activities/ActivityManager.*`, `src/activities/home/HomeActivity.*`, `src/RecentBooksStore.h`, `src/util/BookProgress.*` | Route existing capabilities and adapt existing state to UI, without a second catalog or progress database. |
| XTEInk presentation | `src/components/UITheme.*`, `src/components/CoverGridHomeUi.*`, `src/components/X4ProHomeLayout.h`, existing FreeInkUI controls | Own X4 Pro composition, hierarchy, typography, hit areas and visual states. Isolate additional UI files only when a concrete screen needs them. |
| Existing apps and settings | `src/activities/home/`, `src/activities/settings/`, `src/network/`, existing plugin and transfer activities | Keep all functions reachable; move secondary presentation into a simple extension/settings surface if needed. |

## Delivery order and acceptance

1. **Feature parity map and navigation contract.** Enumerate every currently reachable CrossPoint function, its existing activity/setting, and its destination in XTEInk UI. Define Home, Library, Reader, Files/transfer, Network, Settings and extensions access. The acceptance check is no lost function and a clear return path to the previous reading position. This is the first bounded work package; it changes documentation and, only if needed, isolated UI routing.
2. **Visual contract.** Produce 480×800 monochrome Home, Library, Reader/HUD and Settings/extension flows with empty, populated, error and long Vietnamese text states. The Product Owner reviews the visible result before C++ screen work. Define the image-quality comparison and the meaning of the 90% floor here.
3. **Home and Library shell.** Implement the approved UI through existing `HomeActivity`, `UITheme`, `CoverGridHomeUi`, library/file activities and shared controls. Preserve progress semantics and non-destructive TXT/MD cache reads; XTC Home must not load chapter tables. Deliver working navigation and relevant host tests before another screen.
4. **Reader presentation and input.** Keep the Reader engine and reading state. Adapt HUD, typography controls, Home/Back and gesture consistency through existing owners. Verify remapped buttons, touch, swipe and resume on the X4 Pro.
5. **Typography, caching and refresh.** Add only measured, bounded PSRAM caches and font assets needed by the approved screens. Record glyph coverage, internal/PSRAM heap, flash reserve, latency, ghosting and image quality per panel/controller. Use the existing display HAL and SDK contract; driver policy changes need a separate evidence-based decision.
6. **System parity and release.** Confirm Network, USB transfer, OPDS, plugins, settings, recovery and all original formats remain reachable and functional. Run affected host tests, firmware builds, mandatory firmware handoff and physical-device QA. Release readiness requires explicit PO acceptance and observed hardware evidence.

Each work package starts from a named revision and diff, owns one user-visible outcome, records its tests and resource impact, and stops when an upstream-sensitive exception appears. Preserve both WP2.4 WIP worktrees as evidence; reuse only changes proven relevant to an approved package. Do not equate a commit, browser capture, simulator screenshot or build success with physical X4 Pro verification.

## First implementation gate

The first executable slice is a read-only parity and navigation map grounded in `HomeActivity`, `ActivityManager`, Settings, Network, transfer and plugin entry points. Its output identifies any missing route before UI changes. The PO then reviews the visual contract and this plan. Major firmware work starts only with a bounded screen contract, named worktree, applicable checks and the repository's firmware handoff process.

### Current route map (source inspection, not device QA)

| User function | Current owner and route | Proposed XTEInk location |
| --- | --- | --- |
| Resume / recent book | `HomeActivity` and `CoverGridHomeUi` through `RecentBooksStore` and `BookProgress` | Home primary action and recent shelf |
| Library and local files | `HomeActivity::onLibraryOpen`, `ActivityManager::goToLibrary`; `goToFileBrowser` | Library primary destination; Files/import entry |
| Wi-Fi file transfer | `HomeActivity::onFileTransferOpen`, `ActivityManager::goToFileTransfer` and `CrossPointWebServerActivity` | Visible import/transfer action plus System access |
| USB mass storage | `ActivityManager::goToUsbDrive`, guarded by `FREEINK_CAP_USB_MSC`; `UsbDriveActivity` owns safe SD-card handoff | Transfer options when the board supports it |
| Network and OPDS | `SettingsActivity` Network section; `ActivityManager::goToBrowser`; stored OPDS servers | Library discover and Settings / Network |
| Plugins | `SettingsActivity` Network section and `ActivityManager::goToPlugins`; opt-in via `SETTINGS.pluginsEnabled` | Extensions and Settings / Network |
| Reader controls | Existing Reader activities, mapped input and Settings Reader section | Reader HUD and Settings / Reading |
| Language, keys, clock, updates, about | `SettingsActivity` General/Controls groups with board capability guards | Settings / System and Controls |

The route map is a starting ledger, not yet proof that every CrossPoint setting, recovery path and conditional feature has a corresponding XTEInk destination. Complete that census against `SettingsList`, `SettingsActivity`, feature flags and activity registration before replacing navigation. Preserve board capability guards such as USB MSC, touch and RTC instead of showing actions a board cannot execute.

## Sổ tay triển khai dành cho PO và người thực hiện

**Quyết định đã chốt** là kiến trúc A và X4 Pro là máy đầu tiên. **Phần đề xuất để duyệt** gồm thứ tự bàn giao dưới đây, cách phân công model và phép đánh giá chất lượng. Đầu ra nhìn thấy đầu tiên là prototype 480 × 800 cho phép chính PO mở sách, trở về Home và tìm đường chép sách bằng USB hoặc Wi-Fi. Chỉ khi PO duyệt luồng này mới triển khai màn hình tương ứng trong C++.

### 1. Quy tắc vận hành trong mọi phiên

1. Mở một gói có đầu ra cụ thể. Đọc AGENTS.md, hợp đồng phía trên, quyết định PO mới nhất và phần mã liên quan. Mỗi phiếu giao việc ghi **Superpowers + Ponytail full**. Người nhận việc và reviewer đều áp dụng hai nguyên tắc này.
2. Dùng **Superpowers brainstorming** cho quyết định sản phẩm còn mở và **writing-plans** để đóng phạm vi trước khi sửa. Dùng **using-git-worktrees** khi bắt đầu công việc cần cách ly và **executing-plans** cho người thực hiện chính. Khi phát hiện lỗi tái hiện được, dùng **systematic-debugging**. Dùng **verification-before-completion** trước khi báo kết quả. Quy trình firmware-handoff của repo quyết định số vòng review và thời điểm build.
3. Dùng **Ponytail full** để chọn thay đổi nhỏ nhất đáp ứng trọn vẹn hành vi đã duyệt. Trước mỗi thay đổi, chỉ ra API hoặc activity đang sở hữu hành vi đó, caller bị ảnh hưởng, dữ liệu người dùng có thể mất và phép kiểm tra phát hiện được mất mát. Mỗi phụ thuộc hoặc cache mới phải giải quyết một thiếu hụt có bằng chứng.
4. Một người thực hiện giữ quyền ghi trong một gói. Cho phép đọc song song ở các phần độc lập. Dùng Beads cho trạng thái công việc khi repo có workspace hoạt động. Tại lần kiểm kê vừa qua repo chưa có workspace Beads. Dùng chính phiếu bàn giao trong phiên, tránh tạo thêm backlog song song.
5. Cuối gói ghi base SHA, SDK pin, danh sách thay đổi kể cả tệp mới, lệnh và kết quả thật, artifact để PO mở, việc chưa kiểm và điều kiện làm mất hiệu lực kết quả. Tách rõ SOURCE-INSPECTED, HOST-TESTED, SIMULATOR-TESTED, HARDWARE-VERIFIED và PO-ACCEPTED.

### 2. Model, effort và chế độ

Bảng dưới là **phân công đề xuất**, dựa trên các tên và mức effort có trong danh mục Codex cục bộ lúc lập kế hoạch. Effort là mức suy luận chọn cho một phiên. Số giờ ở phần sau là dự toán công sức chủ động, bao gồm đọc mã và xử lý kết quả. Chúng không phải thời gian phản hồi của model. Nếu máy thực thi thiếu model đã chỉ định, giữ vai trò, mức effort và tính độc lập của reviewer, dùng model khả dụng rồi ghi lựa chọn thực tế vào phiếu.

| Mã | Người chịu trách nhiệm và cấu hình đề xuất | Chế độ và quyền |
| --- | --- | --- |
| K | Kiến trúc sư chính, gpt-6-astra, high | Plan để phân tích ranh giới hoặc thay đổi tiêu chí. Default để ghi đặc tả đã thống nhất. |
| I | Kỹ sư kiểm kê, gpt-6-luna, medium | Default, chỉ đọc mã. Được ghi bảng chức năng vào tài liệu gói. Mọi mục còn nghi vấn đưa về K. |
| U | Thiết kế luồng, gpt-6-astra, high khi chốt luồng, medium khi dựng frame | Default trong thư mục prototype. Dùng Superpowers brainstorming và các hướng dẫn UI UX Pro Max, Impeccable đã đối chiếu với yêu cầu e-paper. |
| F | Kỹ sư firmware chính, gpt-6-sol, high | Default trong worktree riêng, thực hiện từng gói bằng executing-plans. Sở hữu diff, host tests, build và xử lý lỗi. |
| Q | QA simulator, gpt-6-luna, medium | Default với filesystem thử riêng. Ghi revision, profile, ảnh và kết quả. F chịu trách nhiệm xác minh kết quả cuối. |
| R | Bốn reviewer độc lập, mỗi người một ngữ cảnh | Correctness dùng gpt-6-sol high. Architecture dùng gpt-6-astra high. Embedded dùng gpt-6-sol high. i18n/docs dùng gpt-6-luna medium. Cả bốn chỉ đọc. |
| PO | Chủ sản phẩm và người dùng X4 Pro đầu tiên | Duyệt trải nghiệm bằng prototype và thao tác thật. PO điều khiển thiết bị và quyết định flash, chấp nhận đánh đổi hoặc phát hành. |

Mặc định dùng một phiên thực hiện chính. Chỉ nâng một câu hỏi lên K high khi có xung đột quyền sở hữu, nguy cơ đổi cache/progress hoặc kết quả phần cứng không giải thích được. Với tác vụ đếm, chụp ảnh và đối chiếu danh sách, giữ mức medium. Các quyết định thị giác thuộc PO dù model thiết kế hay reviewer đánh giá đạt.

### 3. Mốc đầu tiên — từ chức năng gốc đến prototype có thể duyệt

Thực hiện tuần tự 01–09. Bước 03 và 04 có thể đọc song song sau khi khóa base. Dự toán **12–22 giờ làm việc chủ động** cho mốc này, chưa tính thời gian PO xem hoặc sửa lớn sau phản hồi.

| Bước | Owner, chế độ, công sức | Thao tác và bằng chứng để chuyển bước |
| --- | --- | --- |
| 01. Khóa đầu vào | K, Plan, 0,5–1 giờ | Đọc hợp đồng và bảng WIP. Chạy git worktree list --porcelain, git status --short --branch, git rev-parse HEAD và git ls-tree HEAD freeink-sdk ở cây thích hợp. Ghi SHA của fork develop làm chuẩn tính năng và SHA của bản đối chiếu. Ghi WIP riêng. Mọi so sánh về sau chỉ rõ đối tượng nào được dùng. |
| 02. Khoanh vùng khác biệt | I, chỉ đọc, 0,5–1 giờ | Từ hai SHA ở 01, dùng git diff --name-status trên src, lib, platformio.ini và freeink-sdk. So các tệp chưa commit riêng với HEAD của từng worktree. Đánh dấu phần upstream, phần UI thử nghiệm và test có thể tái dùng. Giữ thử nghiệm parser ở cây fixes. |
| 03. Lập danh sách chức năng | I, chỉ đọc và ghi tài liệu, 2–3 giờ | Lấy USER_GUIDE.md làm danh sách khởi đầu. Tra SettingsList.cpp, SettingsActivity, các thư mục activities và hành động Reader, plugin, recovery. Với mỗi mục ghi tên người dùng thấy, điểm vào, symbol xử lý, điều kiện board/cờ/cấu hình, đích mới, cách quay lại. Kể cả mục ẩn đến khi bật plugin, có OPDS hoặc gắn SD. Ghi UNKNOWN kèm nơi cần kiểm tra cho từng mục thiếu chứng cứ. |
| 04. Truy đường vào cho chức năng thiết yếu | I, chỉ đọc, 1–2 giờ | Theo nút ở HomeActivity/CoverGridHomeUi đến ActivityManager rồi activity đích. Phân biệt Wi-Fi quản lý mạng, truyền tệp qua web và USB MSC. Với USB ghi rõ ai sở hữu thẻ khi host đang mount, cách eject và đường về. Với đọc sách ghi ai ghi progress và ai chỉ hiển thị. Đầu ra là đường đi và đường về cho mỗi chức năng thiết yếu. |
| 05. Đóng bảng đối chiếu | K, Plan, 1–2 giờ | Đối chiếu hai chiều từ chức năng gốc sang UI mới và từ từng nút mới về handler thật. Mỗi hàng phải có đích hoặc trạng thái phụ thuộc thiết bị rõ ràng. Thiếu đường vào cho tính năng gốc là lỗi chặn thay điều hướng. Đưa các hàng còn thiếu ra PO nếu liên quan lựa chọn trải nghiệm. |
| 06. Vẽ luồng và điều khiển | U, thiết kế, 1–2 giờ | Dựng luồng Đọc tiếp → Reader → Home, Thư viện → sách → quay lại đúng vị trí, Home trống → Nhập sách → USB/Wi-Fi, Cài đặt → Network/Extensions → Back. Ghi rõ focus sau khi quay lại, hành vi chạm/vuốt và phím. Kiểm tra một hành động chỉ được xử lý một lần. |
| 07. Dựng prototype | U, Default, 3–5 giờ | Tạo prototype riêng cho thiết kế mới, dùng lại font và công cụ capture đã có. Dựng Home trống/1/2/3+ sách, Library, Reader/HUD và đường vào cài đặt/chuyển tệp. Dùng tên sách tiếng Việt dài, thiếu bìa và progress chưa biết. Mọi control trong bốn luồng ở 06 phải thao tác được. Tách dữ liệu mẫu khỏi nội dung sản phẩm. |
| 08. Kiểm tra và trình mẫu | Q, Default, 2–3 giờ | Mở đúng prototype ở 480 × 800, thao tác bốn luồng bằng pointer và phím. Kiểm bounds, dấu tiếng Việt, tương phản trắng đen, phần chữ bị cắt và trạng thái chọn. Lưu bộ ảnh nhỏ gồm Home trống, Home có sách, trang Reader và lựa chọn truyền tệp. Mỗi lỗi ghi màn hình, thao tác, mong đợi và thực tế. |
| 09. PO duyệt | PO và U, phiên xem mẫu, 1–2 giờ | PO tự thử đọc tiếp, tìm sách và tìm USB/Network. Ghi phiên bản mẫu, điều chấp thuận, lỗi chặn và điều cần kiểm trên máy. Đề xuất so cùng nội dung với bản render chất lượng cao nhất trên cùng máy, gồm chữ có dấu, bìa ảnh và trang chữ nhỏ. PO chốt cách cho điểm và ngưỡng 90% trước khi dùng số đó để nghiệm thu. |

**Đầu ra để bàn giao** gồm một bảng chức năng có nguồn, prototype mở được, bốn hành trình, ảnh kèm revision và quyết định của PO. Nếu prototype cho thấy USB khó tìm, chỉnh vị trí lối vào rồi kiểm lại hành trình nhập sách. Các hành trình độc lập đã có bằng chứng hợp lệ giữ nguyên kết quả.

### 4. Thứ tự triển khai firmware sau khi duyệt mẫu

Dự toán dưới đây dành cho một người thực hiện chính có sẵn toolchain. Mỗi hàng là một gói độc lập về nghiệm thu. Khi thiết kế thay đổi hoặc phải đổi Core, K cập nhật ước lượng và trình tác động trước khi F viết tiếp. Các giờ là khoảng lập kế hoạch, chưa bao gồm chờ phần cứng, thời gian chạy build hoặc xử lý lỗi chưa biết.

| Gói và điều kiện bắt đầu | Các bước thực hiện theo thứ tự | Model, effort, mode và công sức | Điều kiện bàn giao |
| --- | --- | --- | --- |
| 10. Nền tích hợp, sau 09 | ① Chọn commit CrossPoint sạch có đủ chức năng đã kiểm kê ở 01. ② Tạo worktree mới từ SHA đã chọn, xác nhận gitlink SDK và trạng thái sạch. ③ So các hunk WIP có thể dùng với hành vi đã duyệt. ④ Chạy baseline X4 Pro và lưu map/size, build identity, cấu hình simulator. Chỉ mang sang thay đổi có owner và test tương ứng. | K high Plan, F high Default, 2–4 giờ | Tái tạo được baseline. Tách được tác động của mỗi gói khỏi khác biệt do đổi upstream. |
| 11. Home và đọc tiếp | ① Nối dữ liệu RecentBooksStore qua HomeActivity. ② Đặt bố cục X4 Pro ở lớp trình bày. ③ Đọc mốc EPUB khi hợp lệ, XTC dùng %, TXT/MD giữ tiến trình hợp lệ bằng đường đọc an toàn, báo PO nếu cần thay hành vi. ④ Qua test hình học, progress, cache stale/missing và ảnh simulator cho 0/1/2/3+ sách. | F high Default, Q medium, R theo §5, 6–12 giờ | Đọc tiếp về đúng sách. Không có tải bảng chương XTC hay ghi/xóa cache TXT/MD từ việc xem Home. |
| 12. Library, Files và nhập sách | ① Nối danh mục hiện có qua LibraryListActivity và FileBrowserActivity. ② Giữ tìm kiếm/sắp xếp, đường dẫn và chọn sách. ③ Nối hai đích nhập sách qua goToFileTransfer và goToUsbDrive theo capability. ④ Kiểm thẻ thiếu, sách lỗi, Back và quay lại danh sách cũ. | F high Default, Q medium, R, 5–10 giờ | Tìm được sách, mở được sách và tìm được cả hai cách chuyển tệp. PO kiểm eject/khôi phục truy cập SD bằng thiết bị thật. |
| 13. Reader, HUD và input | ① Gắn HUD đã duyệt lên hoạt động đọc hiện có. ② Tái dùng mapped input và vòng đời activity. ③ Theo một gesture xuyên qua overlay, page turn, Home và Back. ④ Kiểm EPUB, TXT/MD, XTC, đổi font, mở TOC, mở lại sách và giữ vị trí. | F high Default, Q medium, R, 6–12 giờ | Một lần chạm chỉ kích hoạt một hành động. Phím remap, vuốt và resume đúng trên thiết bị trong trạng thái đã duyệt. |
| 14. Settings, ứng dụng và mở rộng | ① Ánh xạ toàn bộ hàng ở 03–05 vào Settings hoặc Extensions. ② Dùng plugin system và activity hiện có. ③ Kiểm đường vào/ra với plugin bật, tắt, catalog trống, OPDS có/không có server. ④ So lại tất cả hàng điều kiện Network, Bluetooth, cập nhật, ngôn ngữ và recovery. | F high Default, I/Q medium, R, 4–9 giờ | Mỗi chức năng của baseline có đường truy cập hoặc điều kiện phần cứng được chứng minh. Mở/đóng ứng dụng giữ đúng quyền sở hữu input và storage. |
| 15. Font và chất lượng ảnh | ① So font tích hợp và font SD trên cùng corpus tiếng Việt, chữ tổ hợp và tên dài. ② Đo phần thiếu về dấu, độ nét, hinting, flash và thời gian vẽ. ③ Chỉ bổ sung fallback, hinting hoặc cache glyph khi phép đo chỉ ra nhu cầu. ④ So ảnh với chuẩn PO chọn ở 09 trên đúng panel. | U high cho tiêu chí, F high Default, PO trên máy, R, 4–10 giờ | Có mẫu trước/sau, chi phí byte và kết quả chấm của PO. Hiển thị đúng dấu là yêu cầu bắt buộc riêng với điểm chất lượng ảnh. |
| 16. Tốc độ, PSRAM và refresh | ① Đo cold/warm Home và page turn với cùng nội dung, panel và điều kiện. ② Ghi thời gian, largest free block của internal heap và PSRAM, ghosting sau chuỗi thao tác cố định. ③ Sửa từng biến riêng: cache có giới hạn và fallback khi thiếu bộ nhớ, rồi chính sách partial/full qua HAL nếu cần. ④ Lặp cùng chuỗi và so với baseline. | F high Default, K high nếu vượt ranh giới, R, PO, 5–12 giờ | Có cải thiện đo được, không mất trạng thái, không tích lũy bóng mờ vượt chuẩn PO. Nếu giải pháp cần sửa driver hoặc SDK, tách quyết định cho PO trước khi sửa. |
| 17. QA tổng và bàn giao | ① Khóa SHA + diff + SDK + binary digest. ② Đóng các hàng còn hở trong bảng chức năng. ③ Chạy simulator và build các target bị ảnh hưởng. ④ PO thực hiện QA vật lý sau khi cho phép flash riêng. ⑤ Gắn từng kết quả vào đúng binary, ghi known issues và cách khôi phục. | F high Default, Q medium, PO, 3–7 giờ | PO duyệt hành vi, chi phí bảo trì và trải nghiệm trên máy. Nếu thiếu kết quả bắt buộc, báo đúng trạng thái và hàng còn thiếu. |

### 5. Cách đóng một gói và tiếp tục khi có lỗi

1. **F chuẩn bị candidate.** Ghi yêu cầu và phạm vi, tự review diff, chạy test có liên quan và formatter theo firmware-handoff. Với Home, nhắm BookProgressTest và X4ProHomeLayoutTest nếu các target tồn tại ở candidate. Chạy git diff --check và kiểm riêng tệp mới. Với thay đổi chỉ có tài liệu, kiểm đường dẫn, tính nhất quán và diff.
2. **R review cùng một diff.** Bốn reviewer kiểm riêng correctness, architecture, embedded và i18n/docs. Gửi base, nhận dạng diff, yêu cầu, log và phạm vi. Reviewer chỉ đọc, F thu đủ bốn kết quả trước khi sửa. Dự trù 1–3 giờ tổng công sức review mỗi gói firmware, ngoài bảng §4.
3. **F xử lý một lượt.** Xác nhận từng phát hiện theo đường chạy thật, gộp lỗi cùng nguyên nhân, ghi lý do cho finding không áp dụng. Chạy lại đúng check bị ảnh hưởng. Chỉ có tối đa một lượt follow-up có mục tiêu khi sửa làm mất hiệu lực kết luận trước đó. Nếu vẫn thiếu coverage, đánh dấu handoff incomplete.
4. **Build và simulator.** F chạy pio run -e x4pro sau thay đổi firmware cuối, thêm target chia sẻ theo policy. Q dùng native WSL simulator_x4_pro với cấu hình tạm, FREEINK_DEVICE_X4PRO và filesystem riêng. Lưu profile, SHA/diff, flash, static RAM, IRAM/map, ảnh và trạng thái PASS/FAIL/NOT TESTED. Sai danh tính executable thì loại kết quả.
5. **Khi có lỗi.** Giữ candidate và dữ liệu tái hiện, tìm lỗi đầu tiên, phân loại toolchain, cấu hình, dữ liệu hoặc hành vi. Ví dụ, IRAM báo 100% cần so linker region/map với baseline tương ứng trước khi đề nghị flash. Dừng gói nếu còn nguy cơ boot hoặc mất dữ liệu. Trở về baseline trong worktree cách ly khi cần đối chứng.
6. **Khi chuyển gói.** F ghi phiên bản, bằng chứng và phần chưa kiểm. PO duyệt thay đổi hành vi và đánh đổi cụ thể. Quyền commit, push, PR và flash tuân theo AGENTS.md và chỉ thị riêng của PO. Người thực hiện kế tiếp bắt đầu từ phiên bản đã bàn giao.

### 6. Mẫu giao việc đầu tiên

> Thực hiện 01–05 cho X4 Pro bằng Superpowers và Ponytail full. Người phụ trách kiểm kê dùng gpt-6-luna medium trong Default và chỉ đọc mã. Ghi base, SDK và điều kiện xuất hiện của từng chức năng. Hoàn thiện bảng đối chiếu từ USER_GUIDE, settings, Reader, Home/Back, plugins, Network, USB và recovery. Mỗi hàng phải có đường vào, handler, đích XTEInk và đường về. Báo riêng mục UNKNOWN, ai có thể xác nhận và bằng chứng còn thiếu. Bàn giao bảng cho thiết kế bước 06 khi các chức năng thiết yếu đã truy được cả hai chiều.

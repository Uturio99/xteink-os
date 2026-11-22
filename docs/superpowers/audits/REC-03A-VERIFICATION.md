# REC-03/A — Library → Reader → Back verification

Date: 2026-11-22. Base: `d30e6a12` (`fix/ux02-touch-targets`), `simulator_x4_pro`, WSL clone `~/noveos-rec02b`. Read-only: no source changes, no commit.

PO decision (G2): a short logical Back from a Library-opened book always returns to Library regardless of `backShortToFileBrowser` — accepts the `372ccca1` `returnToCallerOnShortBack` path as intended.

Fixture: `_ux02/sd5` — 15 valid EPUBs (4 authors, shared "Chronicle of …" / "Silent …" prefixes); `_ux02/sd6` — 3 books for the corrupt-EPUB case. Scripts use the sim input-script DSL (TAP/DOWN/CONFIRM/BACK:ms/HOME:80/WAIT/SHOT/QUIT). Artifacts under `\\wsl$\Ubuntu\home\uturio\noveos-rec02b\_ux02\`.

## Case matrix

| # | Case | Expected | Actual | Evidence |
|---|---|---|---|---|
| 1 | Title tab, scrolled (top=4, rows 5–10 of 15), CONFIRM open "Silent Hills" → `BACK:80` | same tab/sort/viewport/focus | `Popped` → `page tab=1 top=4`, first title unchanged; BMP diff before/after = clock rows 30–38 only | **SIMULATOR_BEHAVIOR_VERIFIED** — `c1.log`, `c1-before.png`, `c1-after.png` |
| 2 | Search query `"chron"` (4 rows), open row 03 "Chronicle of Dusk" → `BACK:80` | query still applied, same focus | Pop → header still `02.1 — "chron"`, `04 BOOKS`, focus bar still row 03 | **SIMULATOR_BEHAVIOR_VERIFIED** — `c2.log`, `c2-before.png`, `c2-after.png` |
| 3 | Author tab: long-CONFIRM → "04 GROUPS" collapsed view; expand `Novelist, D.`; open "The Long Road" → `BACK:80` | group state + focus preserved | Pop → `tab=2 top=4`, expanded view restored, focus on row 05 "The Long Road" | **SIMULATOR_BEHAVIOR_VERIFIED** — `c3.log`, `c3-collapsed.png`, `c3-after.png` |
| 4 | Recent tab: open row 02 "Quartz Mountain" → `BACK:80` | re-sorted: book now first, focus follows | Pop → `page first=0 title=Quartz Mountain`, focus bar on row 01 | **SIMULATOR_BEHAVIOR_VERIFIED** — `c4.log`, `c4-recent0.png`, `c4-after.png` |
| 5 | `backShortToFileBrowser` ON → short Back → Library; long Back in both states | G2: short → Library always | Short → **Library (pop)** ✓. **Setting unreachable on X4 Pro**: `SettingsList.cpp:190-191` emits the toggle only `if (!BoardConfig::hasTouch())` — not persisted and not in the UI on touch boards; `SETTINGS.backShortToFileBrowser` is always 0. Long Back observed → **FileBrowser** (`Exiting EpubReader`+`Exiting Library`+`Entering FileBrowser`), matching `ReaderUtils.h:279` (`1 != 0` → `goToFileBrowser`). The short-Back caller branch (ReaderUtils.h:273-276) runs before the setting check, so G2 holds structurally for any value of the flag | **SIMULATOR_BEHAVIOR_VERIFIED** (short/long paths); **SOURCE_VERIFIED** (setting unreachable on touch) — `c5short.log`, `c5longon.log`, `c5-short.png`, `c5-long-on.png` |
| 6 | `HOME:80` from reader → Home; reopen Library | Home; fresh visit | `Exiting EpubReader`+`Exiting Library`+`Entering Home` (HOME replaces the whole stack). Re-entered Library = new instance: Recent tab, viewport top, focus row 01 = just-read book (from recents) | **SIMULATOR_BEHAVIOR_VERIFIED** — `c6.log`, `c6-home.png`, `c6-lib.png` |
| 7 | Truncated (0-byte) EPUB still in index → open from Library | error presented; usable Library | Reader pushed, `Failed to load EPUB` (ZIP/content.opf ERRs), then **self-exits** → `Popped` → Library, focus on the book's row. **Observation:** no user-visible error screen — the reader silently returns to Library within ~4 ms (only log lines). Back afterwards leaves a usable Library | **SIMULATOR_BEHAVIOR_VERIFIED** (mechanics); UX presentation gap noted — `c7.log`, `c7-list.png`, `c7-open.png`, `c7-back.png` |
| 8 | `python3 test/test_library_reader_return.py` | pass | exit 0 (silent success) | **HOST PASS** |

## Mechanism notes

- Short Back from a Library-opened reader pops the pushed activity → `Handling result for popped activity` → `resumeAfterReader(path)`; the Library object itself is never destroyed, so tab/sort/query/groups/viewport persist by construction; `selectBookPath`/`resolvePinned` repair focus when the index changed.
- Long Back from a Library-opened reader bypasses the caller (`longPress` → `goToFileBrowser(filePath)` on X4 Pro, where the setting is always 0) — verified FileBrowser root-replace (`Exiting activity: Library`).
- HOME likewise root-replaces (`Exiting EpubReader` + `Exiting Library` + `Entering Home`).
- One-page fixture books surface the end-of-book suggestion menu on CONFIRM — an extra CONFIRM while in the reader re-opened a different book as root (expected menu behavior, not a defect).

## Observations (not blocking)

- **O1:** Corrupt-EPUB open shows no user-visible error — silent auto-return to Library (log-only). Possible UX gap worth a PO question; per the brief this is recorded, not fixed.
- **O2:** `backShortToFileBrowser` is dead code on touch boards (SettingsList.cpp:190 gate). Long Back from a Library-opened book goes to FileBrowser on X4 Pro — consistent with the accepted mapping, worth stating in the G2 record.

## NOT TESTED (REC-03/B / hardware)

Same-book sync/loan relaunch (F28), memory/SD-handle measurements, touch-only upper-left Back binding, hardware verification, multi-level group collapse inside nested folders, `XtcReader` return path (EPUB only tested).

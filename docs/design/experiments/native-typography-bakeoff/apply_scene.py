#!/usr/bin/env python3
import json
import unicodedata
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[3]
SOURCE = ROOT / "src/activities/home/HomeActivity.cpp"
MARKER = "FONT_BAKEOFF_FIXED_SCENE"
ANCHOR = "  loadRecentBooks(coverGridUi ? CoverGridHomeUi::MAX_BOOKS : metrics.homeRecentBooksCount);\n  hasContinueReading = !recentBooks.empty();"

def main():
    source = SOURCE.read_text(encoding="utf-8")
    scene = json.loads((HERE / "scene-content.json").read_text(encoding="utf-8"))
    books = list(scene["recent_books_order"])
    books[-1] = {"title_nfc": unicodedata.normalize("NFD", books[0]["title_nfc"]), "author": books[0]["author"]}
    rows = ["  // " + MARKER + ": fixed records only; layout and drawing path stay unchanged.",
            "  recentBooks.clear();", "  recentBooks.reserve(5);"]
    for i, book in enumerate(books, 1):
        title = book.get("title_nfc")
        rows.append(f"  recentBooks.push_back({{\"/font-bakeoff/book-{i}.epub\", {json.dumps(title, ensure_ascii=False)}, {json.dumps(book['author'], ensure_ascii=False)}, \"\"}});")
    block = "\n".join(rows)
    if MARKER in source:
        if block not in source:
            raise SystemExit("Unrecognized HomeActivity bake-off marker; refusing to overwrite WIP")
        return
    if source.count(ANCHOR) != 1:
        raise SystemExit("HomeActivity source anchor changed; refusing to apply scene overlay")
    replacement = ANCHOR.split("\n")[0] + "\n" + block + "\n  hasContinueReading = !recentBooks.empty();"
    SOURCE.write_text(source.replace(ANCHOR, replacement), encoding="utf-8", newline="\n")

if __name__ == "__main__":
    main()

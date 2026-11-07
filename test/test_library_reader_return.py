from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def source(path):
    return (ROOT / path).read_text(encoding="utf-8")


library = source("src/activities/library/LibraryListActivity.cpp")
reader = source("src/activities/reader/ReaderActivity.cpp")
reader_header = source("src/activities/reader/ReaderActivity.h")
reader_utils = source("src/activities/reader/ReaderUtils.h")
manager = source("src/activities/ActivityManager.cpp")
activity = source("src/activities/Activity.cpp")

# The Library must allocate Reader before releasing its index, and stack it so
# an allocation failure leaves the existing parent and its open index intact.
open_book = library.split("void LibraryListActivity::openBookByPath", 1)[1].split(
    "void LibraryListActivity::activateIndex", 1
)[0]
assert open_book.index("ReaderActivity::create") < open_book.index("index.close()")
assert "startActivityForResult" in open_book
assert "activityManager.pushActivity" in activity

# Resume through the result callback, after Reader has exited; preserve parent
# navigation state by not calling Library onEnter or recreating the activity.
assert "[this, path](const ActivityResult&)" in open_book
assert "resumeAfterReader" in open_book
assert "currentActivity = std::move(stackActivities.back())" in manager
assert "handler(pendingResult)" in manager
assert "void ActivityManager::goHome" in manager
assert "replaceActivity(std::make_unique<HomeActivity>" in manager
assert "while (!stackActivities.empty())" in manager
assert "void LibraryListActivity::onExit()" in library
assert "index.close()" in library.split("void LibraryListActivity::onExit()", 1)[1].split("\n}", 1)[0]

resume = library.split("void LibraryListActivity::resumeAfterReader", 1)[1].split(
    "void LibraryListActivity::activateIndex", 1
)[0]
assert "Storage.exists(path.c_str())" in resume
assert "library::isLibraryIndexDirty()" in resume
assert "index.open(library::libraryIndexPath())" in resume
assert "rebuildIndex()" in resume
assert 'LOG_ERR("LIB", "library index unavailable after reader return")' in resume
assert "swallowHeldReleases()" in resume
assert "selectBookPath(path)" in resume
assert "groupsWereCollapsed" in resume
assert "readPathHash(record, rowHash)" in library
assert "if (!reader)" in open_book
assert open_book.index("if (!reader)") < open_book.index("index.close()")

# Reader promotes the opened book to RecentBooksStore[0]. Reproduce the pinned
# row reorder and assert that restoring by path survives it while retaining the
# row offset used by the tab-list ring.
def ring_selection(rows, selected_path):
    return rows.index(selected_path) + 1


before = ["/books/other.epub", "/books/opened.epub", "/books/third.epub"]
after_reader = ["/books/opened.epub", "/books/other.epub", "/books/third.epub"]
old_numeric_selection = ring_selection(before, "/books/opened.epub")
assert after_reader[old_numeric_selection - 1] != "/books/opened.epub"
assert ring_selection(after_reader, "/books/opened.epub") == 1
assert ring_selection(before, "/books/other.epub") == 1  # unchanged-order case
title_rows_before = list(before)
title_rows_after = list(title_rows_before)  # Recent promotion does not reorder Title/Author views.
title_selection = ring_selection(title_rows_before, "/books/opened.epub")
assert title_rows_after[title_selection - 1] == "/books/opened.epub"

def recent_pin_lookup_enabled(tab, query, descending):
    return tab == "Recent" and not query and descending


assert recent_pin_lookup_enabled("Recent", "", True)
assert not recent_pin_lookup_enabled("Recent", "needle", True)
assert not recent_pin_lookup_enabled("Recent", "", False)
assert not recent_pin_lookup_enabled("Title", "", True)
assert not recent_pin_lookup_enabled("Author", "", True)
resolve_pinned = library.split("void LibraryListActivity::resolvePinned()", 1)[1].split(
    "void LibraryListActivity::refreshOverlap", 1
)[0]
assert resolve_pinned.index("if (activeTabIndex != RECENT_TAB") < resolve_pinned.index("recentRowsFor")

# Recent pin ordering is inactive on search and Title/Author views. Verify the
# production remap is path-gated and runs only after pinned rows are refreshed.
assert "recentBooks.front().path == path" in resume
assert resume.index("resolvePinned()") < resume.index("recentBooks.front().path == path")
assert "pinnedCount() > 0" in resume
assert "activeTabIndex == RECENT_TAB && query.empty() && isDescending(sortOrder)" in resume
assert "nav.selected = 1" in resume
assert "nav.followOnBuild = true" in resume

# Context changes only the short logical Back route. Existing long Back routing,
# release handling and gesture exclusion remain in the shared handler.
assert "returnToCallerOnShortBack" in reader_header
assert "finish()" in reader
assert "if (!longPress && returnToCaller.fn)" in reader_utils
assert "wasLongPressed(MappedInputManager::Button::Back" in reader_utils
assert "wasReleased(MappedInputManager::Button::Back)" in reader_utils
assert "mappedInput.wasBackGesture()" in reader_utils
loop = reader.split("void ReaderActivity::loop()", 1)[1].split("void ReaderActivity::render", 1)[0]
assert loop.index("handleEndOfBookMenu()") < loop.index("handleFormatInput()")
assert loop.index("handleFormatInput()") < loop.index("handleBackNavigation()")

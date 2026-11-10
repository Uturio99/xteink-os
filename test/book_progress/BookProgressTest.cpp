#include <BookProgress.h>
#include <Utf8.h>
#include <gtest/gtest.h>

#include "Epub.h"
#include "HalStorage.h"

static void epub(const std::string& name, std::initializer_list<uint8_t> bytes) {
  fake::files["/cache/" + name] = {};
  fake::files["/cache/" + name + "/progress.bin"] = bytes;
}
static void xtc(const std::string& name, uint32_t page) {
  fake::files["/cache/" + name + "/progress.bin"] = {uint8_t(page), uint8_t(page >> 8), uint8_t(page >> 16),
                                                     uint8_t(page >> 24)};
}

TEST(BookProgress, AcceptsFourSixAndTenByteEpubFormats) {
  epub("a.epub", {1, 0, 2, 0});
  EXPECT_EQ(loadBookProgress("a.epub"), 33);
  epub("b.epub", {1, 0, 2, 0, 5, 0});
  EXPECT_EQ(loadBookProgress("b.epub"), 47);
  epub("c.epub", {1, 0, 2, 0, 5, 0, 9, 9, 9, 9});
  EXPECT_EQ(loadBookProgress("c.epub"), 47);
}

TEST(BookProgress, RejectsMissingCorruptStaleAndInvalidEpubMetadata) {
  EXPECT_EQ(loadBookProgress("missing.epub"), -1);
  epub("short.epub", {1, 0, 2});
  EXPECT_EQ(loadBookProgress("short.epub"), -1);
  epub("stale.epub", {4, 0, 0, 0});
  EXPECT_EQ(loadBookProgress("stale.epub"), -1);
  epub("done.epub", {3, 0, 0, 0});
  EXPECT_EQ(loadBookProgress("done.epub"), 100);
}

TEST(BookProgress, PreservesPercentageCompatibilityAndDetails) {
  Epub::loadRequests.clear();
  epub("details.epub", {1, 0, 2, 0, 5, 0});
  const auto m = loadBookMilestone("details.epub");
  EXPECT_EQ(m.percentage, 47);
  EXPECT_EQ(m.page, 3);
  EXPECT_EQ(m.pageCount, 5);
  EXPECT_EQ(m.chapterNumber, 2);
  EXPECT_EQ(m.chapterTitle, "Chapter");
  ASSERT_EQ(Epub::loadRequests.size(), 1u);
  EXPECT_EQ(Epub::loadRequests.front(), (std::pair<bool, bool>{false, true}));
}

TEST(BookProgress, BoundsUtf8ChapterTitleWithoutSplittingVietnamese) {
  epub("vietnamese.epub", {1, 0, 2, 0, 5, 0});
  const auto m = loadBookMilestone("vietnamese.epub");
  EXPECT_LE(m.chapterTitle.size(), 64u);
  EXPECT_EQ(utf8SafeTruncateBuffer(m.chapterTitle.c_str(), static_cast<int>(m.chapterTitle.size())),
            static_cast<int>(m.chapterTitle.size()));
}

TEST(BookProgress, HandlesXtcBoundariesAndNoChapterFallback) {
  xtc("chapters.xtc", 3);
  auto m = loadBookMilestone("chapters.xtc");
  EXPECT_EQ(m.percentage, 50);
  EXPECT_EQ(m.chapterNumber, 2);
  EXPECT_EQ(m.page, 1);
  EXPECT_EQ(m.pageCount, 3);
  xtc("plain.xtc", 2);
  m = loadBookMilestone("plain.xtc");
  EXPECT_EQ(m.chapterNumber, -1);
  EXPECT_EQ(m.page, 3);
  xtc("end.xtc", 6);
  EXPECT_EQ(loadBookProgress("end.xtc"), 100);
}

TEST(BookProgress, TxtAndMarkdownDoNotExposeStaleProgress) {
  fake::files["book.txt"] = {'x'};
  fake::files["book.md"] = {'x'};
  fake::files["/cache/book.txt/valid"] = {'1'};
  fake::files["/cache/book.md/valid"] = {'1'};
  fake::files["/cache/book.txt/progress.bin"] = {1, 0, 0, 0};
  fake::files["/cache/book.md/progress.bin"] = {1, 0, 0, 0};
  EXPECT_EQ(loadBookProgress("book.txt"), 33);
  EXPECT_EQ(loadBookProgress("book.md"), 33);
  fake::files.erase("/cache/book.txt/valid");
  fake::files.erase("/cache/book.md/valid");
  EXPECT_EQ(loadBookProgress("book.txt"), -1);
  EXPECT_EQ(loadBookProgress("book.md"), -1);
  fake::files["/cache/book.txt/valid"] = {'1'};
  fake::files["/cache/book.md/valid"] = {'1'};
  fake::files["book.txt"] = {'x', 'y'};
  fake::files["book.md"] = {'x', 'y'};
  EXPECT_EQ(loadBookProgress("book.txt"), -1);
  EXPECT_EQ(loadBookProgress("book.md"), -1);
}

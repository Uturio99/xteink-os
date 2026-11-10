#include <gtest/gtest.h>

#include <BookProgress.h>

#include "Epub.h"
#include "HalStorage.h"

static void putProgress(const std::string& path, std::initializer_list<uint8_t> bytes) {
  fake::files["/cache/" + path + "/progress.bin"] = bytes;
}

TEST(BookProgress, KeepsExistingEpubPercentageFormats) {
  putProgress("book.epub", {1, 0, 2, 0, 5, 0});
  EXPECT_EQ(loadBookProgress("book.epub"), 47);
  EXPECT_EQ(loadBookMilestone("book.epub").percentage, 47);
}

TEST(BookProgress, ReadsNumericEpubMilestoneWithoutTitles) {
  putProgress("book.epub", {1, 0, 2, 0, 5, 0});
  const auto milestone = loadBookMilestone("book.epub");
  EXPECT_EQ(milestone.chapterNumber, 2);
  EXPECT_EQ(milestone.page, 3);
  EXPECT_EQ(milestone.pageCount, 5);
  EXPECT_EQ(Epub::loadRequests.back(), (std::pair<bool, bool>{false, true}));
}

TEST(BookProgress, RejectsMissingMalformedAndUnknownProgress) {
  EXPECT_EQ(loadBookProgress("missing.epub"), -1);
  putProgress("short.epub", {1, 0, 2});
  EXPECT_EQ(loadBookProgress("short.epub"), -1);
  putProgress("unknown.epub", {1, 0, 255, 255});
  const auto milestone = loadBookMilestone("unknown.epub");
  EXPECT_EQ(milestone.page, -1);
  EXPECT_EQ(milestone.pageCount, -1);
  EXPECT_EQ(milestone.percentage, 33);
}

TEST(BookProgress, XtcUsesPercentageOnlyAndDoesNotReadChapters) {
  putProgress("book.xtc", {2, 0, 0, 0});
  const auto milestone = loadBookMilestone("book.xtc");
  EXPECT_EQ(milestone.percentage, 50);
  EXPECT_EQ(milestone.chapterNumber, -1);
  EXPECT_EQ(milestone.page, -1);
  EXPECT_EQ(milestone.pageCount, -1);
}

TEST(BookProgress, HomeMilestoneDoesNotValidateTxtOrMarkdownCaches) {
  fake::files["book.txt"] = {'x'};
  fake::files["book.md"] = {'x'};
  Epub::loadRequests.clear();
  EXPECT_EQ(loadBookMilestone("book.txt").percentage, -1);
  EXPECT_EQ(loadBookMilestone("book.md").percentage, -1);
  EXPECT_TRUE(Epub::loadRequests.empty());
}

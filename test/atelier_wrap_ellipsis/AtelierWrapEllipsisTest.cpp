#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include "components/AtelierWrap.h"

namespace {
constexpr char ELLIPSIS[] = "\xE2\x80\xA6";

int measure(const char* text, const void*) {
  int px = 0;
  for (const auto* p = reinterpret_cast<const unsigned char*>(text); *p; ++p) {
    if ((*p & 0xC0u) != 0x80u) px += 10;
  }
  return px;
}

void expectTail(const char* text, int width, int lines, const char* expected) {
  auto wrapped = wrapAtelierLines(measure, nullptr, text, width, lines);
  ASSERT_GT(wrapped.count, 0);
  ellipsizeAtelierTail(measure, nullptr, wrapped, width);
  EXPECT_STREQ(wrapped.lines[wrapped.count - 1], expected);
  EXPECT_LE(measure(wrapped.lines[wrapped.count - 1], nullptr), width);
}
}  // namespace

TEST(AtelierWrapEllipsis, ExactFitAndTrailingSpaces) {
  for (const auto* text : {"abc de", "abc de  "}) {
    auto wrapped = wrapAtelierLines(measure, nullptr, text, 30, 2);
    EXPECT_FALSE(wrapped.truncated);
    ellipsizeAtelierTail(measure, nullptr, wrapped, 30);
    ASSERT_EQ(wrapped.count, 2);
    EXPECT_STREQ(wrapped.lines[0], "abc");
    EXPECT_EQ(std::string(wrapped.lines[1]).find("de"), 0u);
    EXPECT_LE(measure(wrapped.lines[1], nullptr), 30);
  }
}

TEST(AtelierWrapEllipsis, WordAndCharacterOverflow) {
  expectTail("abc de fg", 30, 2, "de…");
  expectTail("abcdef", 30, 1, "ab…");
  expectTail("abc def ghi jkl mno", 30, 3, "gh…");
}

TEST(AtelierWrapEllipsis, Utf8TailUsesWholeCodepoints) {
  expectTail("cááá", 20, 1, "c…");
  expectTail("日本語", 20, 1, "日…");
  expectTail("😀😀😀", 20, 1, "😀…");
}

TEST(AtelierWrapEllipsis, LineCapacityReservesWholeEllipsis) {
  const std::string text(200, 'a');
  auto wrapped = wrapAtelierLines(measure, nullptr, text.c_str(), 2000, 1);
  ASSERT_TRUE(wrapped.truncated);
  ellipsizeAtelierTail(measure, nullptr, wrapped, 2000);
  EXPECT_EQ(std::strlen(wrapped.lines[0]), 127u);
  EXPECT_STREQ(wrapped.lines[0] + 124, ELLIPSIS);
}

TEST(AtelierWrapEllipsis, TooNarrowForEllipsisIsEmpty) {
  auto wrapped = wrapAtelierLines(measure, nullptr, "abc", 10, 1);
  ellipsizeAtelierTail(measure, nullptr, wrapped, 5);
  EXPECT_STREQ(wrapped.lines[0], "");
  wrapped = wrapAtelierLines(measure, nullptr, "abc", 10, 1);
  ellipsizeAtelierTail(measure, nullptr, wrapped, 0);
  EXPECT_STREQ(wrapped.lines[0], "");
}

TEST(AtelierWrapEllipsis, EmptyAndLineLimit) {
  for (const auto* text : {"", "   "}) {
    auto wrapped = wrapAtelierLines(measure, nullptr, text, 100, 4);
    EXPECT_EQ(wrapped.count, 0);
    EXPECT_FALSE(wrapped.truncated);
    ellipsizeAtelierTail(measure, nullptr, wrapped, 100);
  }
  EXPECT_EQ(wrapAtelierLines(measure, nullptr, nullptr, 100, 4).count, 0);
  EXPECT_EQ(wrapAtelierLines(measure, nullptr, "abc", 100, 0).count, 0);
  auto wrapped = wrapAtelierLines(measure, nullptr, "a b c d e", 10, 8);
  EXPECT_EQ(wrapped.count, 4);
  EXPECT_TRUE(wrapped.truncated);
}

TEST(AtelierWrapEllipsis, PreservesWholeStringFallbackChoice) {
  auto wrapped = wrapAtelierLines(measure, nullptr, "abc def", 30, 1);
  wrapped.useFallback = true;
  ellipsizeAtelierTail(measure, nullptr, wrapped, 30);
  EXPECT_TRUE(wrapped.useFallback);
  EXPECT_STREQ(wrapped.lines[0], "ab…");
}

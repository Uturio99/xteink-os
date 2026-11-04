#include <gtest/gtest.h>

#include "components/X4ProHomeLayout.h"

TEST(X4ProHomeLayout, MatchesPortraitEditorialContract) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.masthead, (HomeLayoutRect{0, 0, 480, 40}));
  EXPECT_EQ(layout.content, (HomeLayoutRect{32, 52, 416, 720}));
  EXPECT_EQ(layout.heroCover, (HomeLayoutRect{340, 88, 108, 154}));
  EXPECT_EQ(layout.heroTitle, (HomeLayoutRect{32, 88, 270, 87}));
  EXPECT_EQ(layout.heroAuthor.x, 32);
  EXPECT_EQ(layout.heroAuthor.width, 270);
  EXPECT_EQ(layout.progressLabel.x, 32);
  EXPECT_EQ(layout.progressLabel.width, 270);
  EXPECT_LE(layout.heroTitle.right(), layout.heroCover.x);
  EXPECT_EQ(layout.heroCover.right(), layout.content.right());
  EXPECT_EQ(layout.heroTitle.height, 3 * 29);
  EXPECT_EQ(layout.cta.height, 56);
  EXPECT_EQ(layout.progressTrack.x, 32);
  EXPECT_EQ(layout.progressTrack.width, 120);
  EXPECT_EQ(layout.progressTrack.height, 2);
  EXPECT_LE(layout.cta.bottom(), layout.shelfHeader.y);
  EXPECT_LE(layout.shelfRows[1].bottom(), layout.utilityLinks[0].y);

  const auto insideContent = [&layout](const HomeLayoutRect rect) {
    return rect.x >= layout.content.x && rect.right() <= layout.content.right() && rect.y >= layout.content.y &&
           rect.bottom() <= layout.content.bottom();
  };
  EXPECT_TRUE(insideContent(layout.heroCover));
  EXPECT_TRUE(insideContent(layout.heroTitle));
  EXPECT_TRUE(insideContent(layout.heroAuthor));
  EXPECT_TRUE(insideContent(layout.progressLabel));
  EXPECT_TRUE(insideContent(layout.progressTrack));
  EXPECT_TRUE(insideContent(layout.cta));
  EXPECT_TRUE(insideContent(layout.shelfHeader));
  for (const auto& rect : layout.shelfRows) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.shelfThumbs) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.shelfTitles) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.shelfAuthors) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.shelfProgress) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.utilityLinks) EXPECT_TRUE(insideContent(rect));
  EXPECT_GE(layout.utilityLinks[0].height, 48);
  EXPECT_GE(layout.utilityLinks[1].y - layout.utilityLinks[0].bottom(), 12);
  EXPECT_GE(layout.utilityLinks[2].y - layout.utilityLinks[1].bottom(), 12);
  EXPECT_LE(layout.utilityLinks[2].bottom(), layout.content.bottom());
  EXPECT_GE(layout.shelfRows[0].height, 72);
  EXPECT_EQ(layout.shelfThumbs[0].width, 40);
  EXPECT_EQ(layout.shelfThumbs[0].height, 56);
  EXPECT_EQ(layout.shelfThumbs[0].y - layout.shelfRows[0].y, (layout.shelfRows[0].height - 56) / 2);
  EXPECT_EQ(layout.shelfProgress[0].right(), layout.content.right());
  EXPECT_LE(layout.shelfTitles[0].right(), layout.shelfProgress[0].x);
  EXPECT_LE(layout.shelfThumbs[0].right(), layout.shelfTitles[0].x);
}

TEST(X4ProHomeLayout, FitsTwoShelfRowsBeforeNavigation) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);
  EXPECT_EQ(layout.shelfRows.size(), 2u);
  EXPECT_EQ(layout.shelfRows[0], (HomeLayoutRect{32, 387, 416, 84}));
  EXPECT_EQ(layout.shelfRows[1], (HomeLayoutRect{32, 485, 416, 84}));
  EXPECT_EQ(layout.shelfThumbs[0], (HomeLayoutRect{32, 401, 40, 56}));
  EXPECT_EQ(layout.shelfThumbs[1], (HomeLayoutRect{32, 499, 40, 56}));
  EXPECT_GT(layout.shelfRows[1].y, layout.shelfRows[0].bottom());
  EXPECT_LE(layout.shelfAuthors[1].bottom(), layout.utilityLinks[0].y);
  EXPECT_GE(layout.utilityLinks[0].height, 48);
  EXPECT_EQ(layout.utilityLinks[0], (HomeLayoutRect{32, 600, 416, 48}));
  EXPECT_EQ(layout.utilityLinks[1], (HomeLayoutRect{32, 660, 416, 48}));
  EXPECT_EQ(layout.utilityLinks[2], (HomeLayoutRect{32, 720, 416, 48}));
}

TEST(X4ProHomeLayout, BoundsRecentRowsByAvailableBooks) {
  EXPECT_EQ(x4ProVisibleBookCount(0), 0);
  EXPECT_EQ(x4ProVisibleBookCount(1), 1);
  EXPECT_EQ(x4ProVisibleBookCount(2), 2);
  EXPECT_EQ(x4ProVisibleBookCount(3), 3);
  EXPECT_EQ(x4ProVisibleBookCount(7), 3);
  EXPECT_EQ(x4ProShelfRowCount(0), 0);
  EXPECT_EQ(x4ProShelfRowCount(1), 0);
  EXPECT_EQ(x4ProShelfRowCount(2), 1);
  EXPECT_EQ(x4ProShelfRowCount(3), 2);
  EXPECT_EQ(x4ProShelfRowCount(7), 2);
  EXPECT_EQ(x4ProShelfBookIndex(0), 1);
  EXPECT_EQ(x4ProShelfBookIndex(1), 2);
  EXPECT_EQ(x4ProHeroCtaAction(true, 7), 0);
  EXPECT_EQ(x4ProHeroCtaAction(false, 7), 8);
}

TEST(X4ProHomeLayout, RowsHoldUnderLargerTitleFont) {
  constexpr auto layout = makeX4ProHomeLayout(34, 24);
  const auto insideContent = [&layout](const HomeLayoutRect rect) {
    return rect.x >= layout.content.x && rect.right() <= layout.content.right() && rect.y >= layout.content.y &&
           rect.bottom() <= layout.content.bottom();
  };
  EXPECT_TRUE(insideContent(layout.shelfRows[0]));
  EXPECT_TRUE(insideContent(layout.shelfRows[1]));
  EXPECT_TRUE(insideContent(layout.shelfTitles[1]));
  EXPECT_TRUE(insideContent(layout.shelfAuthors[1]));
  EXPECT_TRUE(insideContent(layout.shelfProgress[1]));
  EXPECT_LE(layout.shelfAuthors[1].bottom(), layout.shelfRows[1].bottom());
  EXPECT_LE(layout.shelfRows[1].bottom(), layout.utilityLinks[0].y);
  EXPECT_TRUE(insideContent(layout.utilityLinks[2]));
  EXPECT_LE(layout.heroTitle.bottom(), layout.heroCover.y + 154);
}

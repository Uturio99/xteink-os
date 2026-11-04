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
  EXPECT_LE(layout.shelfAuthors[2].bottom(), layout.tabs.y);

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
  EXPECT_TRUE(insideContent(layout.tabs));
  for (const auto& rect : layout.shelfCovers) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.shelfTitles) EXPECT_TRUE(insideContent(rect));
  for (const auto& rect : layout.shelfAuthors) EXPECT_TRUE(insideContent(rect));
}

TEST(X4ProHomeLayout, FitsThreeShelfBooksBeforeNavigation) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.shelfCovers.size(), 3u);
  EXPECT_EQ(layout.shelfCovers[0], (HomeLayoutRect{32, 387, 125, 188}));
  EXPECT_EQ(layout.shelfCovers[1], (HomeLayoutRect{177, 387, 125, 188}));
  EXPECT_EQ(layout.shelfCovers[2], (HomeLayoutRect{322, 387, 125, 188}));
  EXPECT_LE(layout.shelfAuthors[2].bottom(), layout.tabs.y);
  EXPECT_EQ(layout.tabs.height, 44);
}

TEST(X4ProHomeLayout, BoundsPortraitBooksWithoutChangingStoredCapacity) {
  EXPECT_EQ(x4ProVisibleBookCount(7), 4);
  EXPECT_EQ(x4ProVisibleBookCount(3), 3);
  EXPECT_EQ(x4ProHeroCtaAction(true, 7), 0);
  EXPECT_EQ(x4ProHeroCtaAction(false, 7), 8);
}

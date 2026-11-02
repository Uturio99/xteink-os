#include <gtest/gtest.h>

#include "components/X4ProHomeLayout.h"

TEST(X4ProHomeLayout, MatchesPortraitEditorialContract) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.masthead, (HomeLayoutRect{0, 0, 480, 40}));
  EXPECT_EQ(layout.content, (HomeLayoutRect{32, 52, 416, 720}));
  EXPECT_EQ(layout.heroCover, (HomeLayoutRect{202, 62, 76, 114}));
  EXPECT_EQ(layout.heroTitle.height, 3 * 29);
  EXPECT_EQ(layout.cta.height, 44);
  EXPECT_EQ(layout.progressTrack.width, 120);
  EXPECT_EQ(layout.progressTrack.height, 2);
}

TEST(X4ProHomeLayout, FitsThreeShelfBooksBeforeNavigation) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.shelfCovers.size(), 3u);
  EXPECT_EQ(layout.shelfCovers[0], (HomeLayoutRect{32, 483, 125, 188}));
  EXPECT_EQ(layout.shelfCovers[1], (HomeLayoutRect{177, 483, 125, 188}));
  EXPECT_EQ(layout.shelfCovers[2], (HomeLayoutRect{322, 483, 125, 188}));
  EXPECT_LE(layout.shelfAuthors[2].bottom(), layout.tabs.y);
  EXPECT_EQ(layout.tabs.height, 44);
}

TEST(X4ProHomeLayout, BoundsPortraitBooksWithoutChangingStoredCapacity) {
  EXPECT_EQ(x4ProVisibleBookCount(7), 4);
  EXPECT_EQ(x4ProVisibleBookCount(3), 3);
  EXPECT_EQ(x4ProHeroCtaAction(true, 7), 0);
  EXPECT_EQ(x4ProHeroCtaAction(false, 7), 8);
}

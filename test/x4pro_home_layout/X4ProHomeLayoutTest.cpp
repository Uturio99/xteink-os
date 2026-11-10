#include <gtest/gtest.h>

#include "components/X4ProHomeLayout.h"

TEST(X4ProHomeLayout, MatchesHybridEditorialHomeComposition) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.masthead, (HomeLayoutRect{31, 29, 418, 27}));
  EXPECT_EQ(layout.heading, (HomeLayoutRect{31, 99, 418, 45}));
  EXPECT_EQ(layout.importAction, (HomeLayoutRect{340, 99, 109, 44}));
  EXPECT_EQ(layout.heroCover, (HomeLayoutRect{31, 194, 145, 210}));
  EXPECT_EQ(layout.heroDetails.x, 195);
  EXPECT_EQ(layout.heroDetails.width, 254);
  EXPECT_EQ(layout.heroTitle.height, 2 * 29);
  EXPECT_LE(layout.progressTrack.bottom(), layout.heroCover.bottom());
  EXPECT_EQ(layout.cta, (HomeLayoutRect{31, 428, 418, 50}));
  EXPECT_EQ(layout.shelfHeader, (HomeLayoutRect{31, 507, 418, 44}));
  EXPECT_EQ(layout.recentRows[0].y, 567);
  EXPECT_EQ(layout.recentRows[1].bottom(), 731);
  EXPECT_EQ(layout.tabs, (HomeLayoutRect{31, 744, 418, 44}));
}

TEST(X4ProHomeLayout, EmptyStateActionsAreVisibleTouchTargets) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.emptyIcon, (HomeLayoutRect{208, 247, 64, 64}));
  EXPECT_GE(layout.emptyTitle.y - layout.emptyIcon.bottom(), 8);
  EXPECT_LE(layout.emptyTitle.y - layout.emptyIcon.bottom(), 16);
  EXPECT_GE(layout.emptyImport.height, 44);
  EXPECT_GE(layout.emptyTransfer.height, 44);
  EXPECT_LE(layout.emptyImport.bottom(), layout.emptyTransfer.y);
  EXPECT_LE(layout.emptyTransfer.bottom(), layout.tabs.y);
  EXPECT_LE(layout.emptyCatalog.bottom(), layout.tabs.y);
}

TEST(X4ProHomeLayout, ShowsTwoRecentRowsBeforeRootNavigation) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);

  EXPECT_EQ(layout.recentRows.size(), 2u);
  EXPECT_EQ(layout.recentCovers[0], (HomeLayoutRect{31, 575, 42, 61}));
  EXPECT_EQ(layout.recentCovers[1], (HomeLayoutRect{31, 661, 42, 61}));
  EXPECT_EQ(layout.recentRows[1].y - layout.recentRows[0].bottom(), 8);
  EXPECT_LE(layout.recentRows[1].bottom(), layout.tabs.y);
  EXPECT_LE(layout.recentTitles[0].bottom(), layout.recentAuthors[0].y);
  EXPECT_LE(layout.recentAuthors[0].bottom(), layout.recentRows[0].bottom());
}

TEST(X4ProHomeLayout, BoundsPortraitBooksWithoutChangingStoredCapacity) {
  EXPECT_EQ(x4ProVisibleBookCount(7), 3);
  EXPECT_EQ(x4ProVisibleBookCount(3), 3);
  EXPECT_EQ(x4ProHeroCtaAction(true, 7), 0);
  EXPECT_EQ(x4ProHeroCtaAction(false, 7), 8);
}

TEST(X4ProHomeLayout, KeepsContextActionsOnExistingMenuSlots) {
  EXPECT_EQ(x4ProTransferMenuIndex(false), 2);
  EXPECT_EQ(x4ProSettingsMenuIndex(false), 3);
  EXPECT_EQ(x4ProTransferMenuIndex(true), 3);
  EXPECT_EQ(x4ProSettingsMenuIndex(true), 4);
}

TEST(X4ProHomeLayout, RootRailKeepsTextAndUnderlineClear) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);
  constexpr auto home = x4ProRootTabRect(0);
  constexpr auto library = x4ProRootTabRect(1);
  constexpr auto settings = x4ProRootTabRect(2);
  EXPECT_EQ(home.height, 44);
  EXPECT_GE(library.x - home.right(), 8);
  EXPECT_GE(settings.x - library.right(), 8);
  EXPECT_LE(settings.right(), layout.tabs.right());
  EXPECT_GT(layout.tabs.y - 6, layout.recentRows[1].bottom());
  EXPECT_LT(home.y + 20, home.bottom() - 3);
}

TEST(X4ProHomeLayout, RootActionsUseExistingHomeMenuIndices) {
  EXPECT_EQ(x4ProRootTabAction(0, 0, false), -1);
  EXPECT_EQ(x4ProRootTabAction(1, 3, false), 4);
  EXPECT_EQ(x4ProRootTabAction(2, 3, false), 6);
  EXPECT_EQ(x4ProRootTabAction(2, 3, true), 7);
}

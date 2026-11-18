#include <gtest/gtest.h>

#include <array>

#include "../../src/components/AtelierSelection.h"
#include "../../src/images/NoveOsLogo.h"
#include "components/X4ProHomeLayout.h"

namespace {
constexpr HomeLayoutRect X4PRO_SAFE_AREA{7, 9, 466, 788};

void expectValidLayoutGeometry(const X4ProHomeLayout& layout) {
  EXPECT_TRUE(homeRectContains(X4PRO_SAFE_AREA, layout.masthead));
  EXPECT_TRUE(homeRectContains(X4PRO_SAFE_AREA, layout.content));
  EXPECT_FALSE(homeRectsOverlap(layout.masthead, layout.content));

  const std::array<HomeLayoutRect, 13> siblings{
      layout.indexRow,         layout.heroCover,  layout.heroTitle,     layout.heroAuthor,    layout.progressTrack,
      layout.progressLabel,    layout.cta,        layout.btnSecondaryLeft, layout.btnSecondaryRight, layout.shelfHeader,
      layout.shelfRows[0],     layout.shelfRows[1], layout.tabs,
  };

  for (size_t i = 0; i < siblings.size(); ++i) {
    EXPECT_TRUE(homeRectContains(layout.content, siblings[i])) << "Sibling " << i << " out of content bounds";
    EXPECT_FALSE(homeRectsOverlap(layout.masthead, siblings[i])) << "Sibling " << i << " overlaps masthead";
    for (size_t j = i + 1; j < siblings.size(); ++j) {
      EXPECT_FALSE(homeRectsOverlap(siblings[i], siblings[j])) << "Overlap between " << i << " and " << j;
    }
  }

  for (const auto& touchTarget :
       {layout.cta, layout.btnSecondaryLeft, layout.btnSecondaryRight, layout.shelfRows[0], layout.shelfRows[1],
        layout.tabs}) {
    EXPECT_GE(touchTarget.width, 44);
    EXPECT_GE(touchTarget.height, 44);
  }
}
}  // namespace

TEST(X4ProHomeLayout, MatchesAtelierEditorialContract) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);
  expectValidLayoutGeometry(layout);

  EXPECT_EQ(layout.masthead, (HomeLayoutRect{32, 20, 416, 28}));
  EXPECT_EQ(layout.content, (HomeLayoutRect{32, 52, 416, 736}));
  EXPECT_EQ(layout.indexRow, (HomeLayoutRect{32, 60, 416, 24}));
  EXPECT_EQ(layout.heroCover, (HomeLayoutRect{324, 108, 124, 186}));
  EXPECT_EQ(layout.heroTitle, (HomeLayoutRect{32, 108, 272, 87}));
  EXPECT_EQ(layout.heroAuthor, (HomeLayoutRect{32, 203, 272, 20}));
  EXPECT_EQ(layout.progressTrack, (HomeLayoutRect{32, 231, 272, 11}));
  EXPECT_EQ(layout.progressLabel, (HomeLayoutRect{32, 246, 272, 18}));
  EXPECT_EQ(layout.cta, (HomeLayoutRect{32, 314, 176, 44}));
  EXPECT_EQ(layout.btnSecondaryLeft, (HomeLayoutRect{352, 314, 44, 44}));
  EXPECT_EQ(layout.btnSecondaryRight, (HomeLayoutRect{404, 314, 44, 44}));
  EXPECT_EQ(layout.shelfHeader, (HomeLayoutRect{32, 416, 416, 26}));
  EXPECT_EQ(layout.shelfRows[0], (HomeLayoutRect{32, 442, 416, 72}));
  EXPECT_EQ(layout.shelfRows[1], (HomeLayoutRect{32, 514, 416, 72}));
  EXPECT_EQ(layout.tabs, (HomeLayoutRect{32, 740, 416, 48}));
}

TEST(X4ProHomeLayout, HoldsUnderLargerFontScale) {
  constexpr auto layout = makeX4ProHomeLayout(34, 26);
  expectValidLayoutGeometry(layout);
  EXPECT_GE(layout.heroTitle.height, 34 * 3);
  EXPECT_LT(layout.shelfRows[1].bottom(), layout.tabs.y);
}

TEST(X4ProHomeLayout, BoundsRecentBooksAndScrubberTick) {
  EXPECT_EQ(x4ProVisibleBookCount(-2), 0);
  EXPECT_EQ(x4ProVisibleBookCount(0), 0);
  EXPECT_EQ(x4ProVisibleBookCount(1), 1);
  EXPECT_EQ(x4ProVisibleBookCount(3), 3);
  EXPECT_EQ(x4ProVisibleBookCount(7), 3);

  EXPECT_EQ(x4ProShelfRowCount(0), 0);
  EXPECT_EQ(x4ProShelfRowCount(1), 0);
  EXPECT_EQ(x4ProShelfRowCount(2), 1);
  EXPECT_EQ(x4ProShelfRowCount(3), 2);
  EXPECT_EQ(x4ProShelfRowCount(7), 2);

  EXPECT_EQ(x4ProHeroCtaAction(true, 7), 0);
  EXPECT_EQ(x4ProHeroCtaAction(false, 7), 8);
  EXPECT_EQ(x4ProHeroCtaAction(false, 0), 1);

  constexpr auto layout = makeX4ProHomeLayout(29, 24);
  constexpr auto track = layout.progressTrack;
  EXPECT_EQ(x4ProScrubberTickX(track, 0), track.x);
  EXPECT_EQ(x4ProScrubberTickX(track, -5), track.x);
  EXPECT_EQ(x4ProScrubberTickX(track, 50), 167);
  EXPECT_EQ(x4ProScrubberTickX(track, 100), track.right() - 2);
  EXPECT_EQ(x4ProScrubberTickX(track, 150), track.right() - 2);

  for (const int pct : {-5, 0, 50, 100, 150}) {
    const int16_t tickX = x4ProScrubberTickX(track, pct);
    const HomeLayoutRect tick{tickX, track.y, 2, track.height};
    EXPECT_TRUE(homeRectContains(track, tick)) << "Scrubber tick out of bounds at pct=" << pct;
  }
}

TEST(X4ProHomeLayout, ComputesAtelierRowSelectionLayout) {
  constexpr auto unsel = makeAtelierRowSelectionLayout(32, 300, 416, 48, false, 72);
  EXPECT_FALSE(unsel.selected);
  EXPECT_EQ(unsel.barWidth, 0);
  EXPECT_EQ(unsel.barHeight, 0);
  EXPECT_EQ(unsel.indexX, 32);
  EXPECT_EQ(unsel.textX, 76);
  EXPECT_EQ(unsel.textWidth, 300);

  constexpr auto sel = makeAtelierRowSelectionLayout(32, 300, 416, 48, true, 72);
  EXPECT_TRUE(sel.selected);
  EXPECT_EQ(sel.barX, 32);
  EXPECT_EQ(sel.barY, 310);
  EXPECT_EQ(sel.barWidth, 5);
  EXPECT_EQ(sel.barHeight, 28);
  EXPECT_EQ(sel.indexX, 53);
  EXPECT_EQ(sel.textX, 97);
  EXPECT_EQ(sel.textWidth, 279);
  EXPECT_EQ(sel.indexX - (sel.barX + sel.barWidth), 16);
}

TEST(X4ProHomeLayout, PlansBwFastFocusAndPeriodicCleanup) {
  // Step 0 (initial / forced full) -> 3-pass grayscale, counter = 0
  constexpr auto initClean = planAtelierFocusRefresh(0, true, true);
  EXPECT_FALSE(initClean.bwFastOnly);
  EXPECT_TRUE(initClean.useHalfRefresh);
  EXPECT_EQ(initClean.nextCounter, 0);

  constexpr auto initFast = planAtelierFocusRefresh(5, true, false);
  EXPECT_FALSE(initFast.bwFastOnly);
  EXPECT_FALSE(initFast.useHalfRefresh);
  EXPECT_EQ(initFast.nextCounter, 0);

  // Steps 1..7 -> BW-only FAST_REFRESH, counter advances 1..7
  uint8_t counter = 0;
  for (uint8_t step = 1; step < ATELIER_FOCUS_CLEAN_INTERVAL; ++step) {
    const auto plan = planAtelierFocusRefresh(counter, false, false);
    EXPECT_TRUE(plan.bwFastOnly) << "step=" << static_cast<int>(step);
    EXPECT_FALSE(plan.useHalfRefresh);
    EXPECT_EQ(plan.nextCounter, step);
    counter = plan.nextCounter;
  }

  // Step 8 -> full 3-pass HALF_REFRESH cleanup and counter reset to 0
  const auto cleanup = planAtelierFocusRefresh(counter, false, false);
  EXPECT_FALSE(cleanup.bwFastOnly);
  EXPECT_TRUE(cleanup.useHalfRefresh);
  EXPECT_EQ(cleanup.nextCounter, 0);

  // Counter >= 8 also triggers cleanup and resets
  constexpr auto overflow = planAtelierFocusRefresh(ATELIER_FOCUS_CLEAN_INTERVAL, false, false);
  EXPECT_FALSE(overflow.bwFastOnly);
  EXPECT_TRUE(overflow.useHalfRefresh);
  EXPECT_EQ(overflow.nextCounter, 0);
}

TEST(X4ProHomeLayout, NoveOsBrandingAssetsAndSplashBoundsAreValid) {
  EXPECT_STREQ(NoveOsBranding::OS_NAME, "NoveOS");
  EXPECT_EQ(NoveOsBranding::LOCKUP_WIDTH, 220);
  EXPECT_EQ(NoveOsBranding::LOCKUP_HEIGHT, 226);
  EXPECT_EQ(sizeof(NoveOsBranding::NOVEOS_LOCKUP_2BPP),
            static_cast<size_t>((NoveOsBranding::LOCKUP_WIDTH * NoveOsBranding::LOCKUP_HEIGHT + 3) / 4));
  EXPECT_EQ(NoveOsBranding::LOGO_WIDTH, 120);
  EXPECT_EQ(NoveOsBranding::LOGO_HEIGHT, 120);
  EXPECT_EQ(sizeof(NoveOsBranding::NOVEOS_LOGO_1BPP),
            static_cast<size_t>((NoveOsBranding::LOGO_WIDTH * NoveOsBranding::LOGO_HEIGHT) / 8));

  bool hasLockupInk = false;
  for (const uint8_t b : NoveOsBranding::NOVEOS_LOCKUP_2BPP) {
    if (b != 0) {
      hasLockupInk = true;
      break;
    }
  }
  EXPECT_TRUE(hasLockupInk);

  constexpr int logoX = (480 - NoveOsBranding::LOCKUP_WIDTH) / 2;
  constexpr int logoY = 196;
  constexpr int ruleY = logoY + NoveOsBranding::LOCKUP_HEIGHT + 28;
  EXPECT_GE(logoX, 32);
  EXPECT_LE(logoX + NoveOsBranding::LOCKUP_WIDTH, 480 - 32);
  EXPECT_GT(logoY, 61);
  EXPECT_LT(ruleY + 46 + 24, 736);
}

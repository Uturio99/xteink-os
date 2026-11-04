#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

struct HomeLayoutRect {
  int16_t x;
  int16_t y;
  int16_t width;
  int16_t height;

  constexpr int16_t right() const { return static_cast<int16_t>(x + width); }
  constexpr int16_t bottom() const { return static_cast<int16_t>(y + height); }
  constexpr bool operator==(const HomeLayoutRect& other) const {
    return x == other.x && y == other.y && width == other.width && height == other.height;
  }
};

struct X4ProHomeLayout {
  HomeLayoutRect masthead;
  HomeLayoutRect content;
  HomeLayoutRect heroCover;
  HomeLayoutRect heroTitle;
  HomeLayoutRect heroAuthor;
  HomeLayoutRect progressLabel;
  HomeLayoutRect progressTrack;
  HomeLayoutRect cta;
  HomeLayoutRect shelfHeader;
  std::array<HomeLayoutRect, 3> shelfCovers;
  std::array<HomeLayoutRect, 3> shelfTitles;
  std::array<HomeLayoutRect, 3> shelfAuthors;
  HomeLayoutRect tabs;
};

constexpr int x4ProVisibleBookCount(const int totalBooks) { return std::min(4, std::max(0, totalBooks)); }

constexpr int x4ProHeroCtaAction(const bool hasContinueReading, const int totalBooks) {
  return hasContinueReading ? 0 : std::max(0, totalBooks) + 1;
}

constexpr X4ProHomeLayout makeX4ProHomeLayout(const int16_t titleLineHeight, const int16_t smallLineHeight,
                                               const int16_t requestedCtaWidth = 180) {
  constexpr int16_t CONTENT_X = 32;
  constexpr int16_t CONTENT_WIDTH = 416;
  constexpr int16_t HERO_COVER_X = 340;
  constexpr int16_t HERO_COVER_Y = 88;
  constexpr int16_t HERO_COVER_WIDTH = 108;
  constexpr int16_t HERO_COVER_HEIGHT = 154;
  constexpr int16_t HERO_TEXT_WIDTH = 270;
  constexpr int16_t SHELF_COVER_WIDTH = 125;
  constexpr int16_t SHELF_COVER_HEIGHT = 188;
  constexpr int16_t SHELF_COLUMN_STEP = 145;

  const int16_t titleY = HERO_COVER_Y;
  const int16_t titleHeight = static_cast<int16_t>(titleLineHeight * 3);
  const int16_t authorY = static_cast<int16_t>(titleY + titleHeight + 8);
  const int16_t progressY = static_cast<int16_t>(authorY + smallLineHeight + 16);
  const int16_t trackY = static_cast<int16_t>(progressY + smallLineHeight + 6);
  const int16_t ctaY = static_cast<int16_t>(trackY + 2 + 22);
  const int16_t ctaWidth = std::min<int16_t>(CONTENT_WIDTH, std::max<int16_t>(48, requestedCtaWidth));
  const int16_t shelfHeaderY = static_cast<int16_t>(ctaY + 56 + 18);
  const int16_t shelfCoverY = static_cast<int16_t>(shelfHeaderY + smallLineHeight + 12);
  const int16_t shelfTitleY = static_cast<int16_t>(shelfCoverY + SHELF_COVER_HEIGHT + 6);
  const int16_t shelfAuthorY = static_cast<int16_t>(shelfTitleY + smallLineHeight + 1);

  X4ProHomeLayout layout{{0, 0, 480, 40},
                         {CONTENT_X, 52, CONTENT_WIDTH, 720},
                         {HERO_COVER_X, HERO_COVER_Y, HERO_COVER_WIDTH, HERO_COVER_HEIGHT},
                         {CONTENT_X, titleY, HERO_TEXT_WIDTH, titleHeight},
                         {CONTENT_X, authorY, HERO_TEXT_WIDTH, smallLineHeight},
                         {CONTENT_X, progressY, HERO_TEXT_WIDTH, smallLineHeight},
                         {CONTENT_X, trackY, 120, 2},
                         {static_cast<int16_t>((480 - ctaWidth) / 2), ctaY, ctaWidth, 56},
                         {CONTENT_X, shelfHeaderY, CONTENT_WIDTH, smallLineHeight},
                         {},
                         {},
                         {},
                         {CONTENT_X, 744, CONTENT_WIDTH, 44}};
  for (size_t i = 0; i < layout.shelfCovers.size(); ++i) {
    const int16_t x = static_cast<int16_t>(CONTENT_X + i * SHELF_COLUMN_STEP);
    layout.shelfCovers[i] = {x, shelfCoverY, SHELF_COVER_WIDTH, SHELF_COVER_HEIGHT};
    layout.shelfTitles[i] = {x, shelfTitleY, SHELF_COVER_WIDTH, smallLineHeight};
    layout.shelfAuthors[i] = {x, shelfAuthorY, SHELF_COVER_WIDTH, smallLineHeight};
  }
  return layout;
}

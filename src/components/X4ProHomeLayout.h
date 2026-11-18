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

constexpr bool homeRectContains(const HomeLayoutRect& outer, const HomeLayoutRect& inner) {
  return outer.width >= 0 && outer.height >= 0 && inner.width >= 0 && inner.height >= 0 && inner.x >= outer.x &&
         inner.y >= outer.y && inner.right() <= outer.right() && inner.bottom() <= outer.bottom();
}

constexpr bool homeRectsOverlap(const HomeLayoutRect& a, const HomeLayoutRect& b) {
  if (a.width <= 0 || a.height <= 0 || b.width <= 0 || b.height <= 0) return false;
  return a.x < b.right() && b.x < a.right() && a.y < b.bottom() && b.y < a.bottom();
}

struct X4ProHomeLayout {
  static constexpr int16_t HERO_COVER_WIDTH = 124;
  static constexpr int16_t HERO_COVER_HEIGHT = 186;
  static constexpr size_t SHELF_ROW_COUNT = 2;

  HomeLayoutRect masthead;
  HomeLayoutRect content;
  HomeLayoutRect indexRow;
  HomeLayoutRect heroCover;
  HomeLayoutRect heroTitle;
  HomeLayoutRect heroAuthor;
  HomeLayoutRect progressTrack;
  HomeLayoutRect progressLabel;
  HomeLayoutRect cta;
  HomeLayoutRect btnSecondaryLeft;
  HomeLayoutRect btnSecondaryRight;
  HomeLayoutRect shelfHeader;
  std::array<HomeLayoutRect, SHELF_ROW_COUNT> shelfRows;
  HomeLayoutRect tabs;
};

constexpr int x4ProShelfRowCount(const int totalBooks) {
  return std::min<int>(static_cast<int>(X4ProHomeLayout::SHELF_ROW_COUNT), std::max(0, totalBooks - 1));
}

constexpr int x4ProVisibleBookCount(const int totalBooks) {
  return std::min<int>(1 + static_cast<int>(X4ProHomeLayout::SHELF_ROW_COUNT), std::max(0, totalBooks));
}

constexpr int x4ProHeroCtaAction(const bool hasContinueReading, const int totalBooks) {
  return hasContinueReading ? 0 : std::max(0, totalBooks) + 1;
}

constexpr int16_t x4ProScrubberTickX(const HomeLayoutRect& track, const int progressPct) {
  constexpr int16_t tickWidth = 2;
  if (track.width <= tickWidth) return track.x;
  const int clamped = std::min(100, std::max(0, progressPct));
  const int travel = track.width - tickWidth;
  return static_cast<int16_t>(track.x + (travel * clamped + 50) / 100);
}

// Concept 01 (Atelier Typographic #s1) 1:1 portrait layout for X4 Pro (480x800).
constexpr X4ProHomeLayout makeX4ProHomeLayout(const int16_t titleLineHeight, const int16_t smallLineHeight) {
  constexpr int16_t CONTENT_X = 32;
  constexpr int16_t CONTENT_Y = 52;
  constexpr int16_t CONTENT_WIDTH = 416;
  constexpr int16_t CONTENT_HEIGHT = 736;
  constexpr int16_t HERO_LEFT_WIDTH = 272;
  constexpr int16_t HERO_COVER_X = 324;
  constexpr int16_t TOUCH_MIN = 44;
  constexpr int16_t ACTION_GAP = 8;
  constexpr int16_t TABS_Y = 740;
  constexpr int16_t TABS_HEIGHT = 48;
  constexpr int16_t SHELF_BOTTOM_LIMIT = TABS_Y - 16;

  const int16_t safeTitleLh = std::max<int16_t>(1, titleLineHeight);
  const int16_t safeSmallLh = std::max<int16_t>(1, smallLineHeight);

  constexpr int16_t INDEX_Y = 60;
  const int16_t indexHeight = std::max<int16_t>(24, safeSmallLh);
  const int16_t heroY = static_cast<int16_t>(INDEX_Y + indexHeight + 24);

  const HomeLayoutRect heroCover{HERO_COVER_X, heroY, X4ProHomeLayout::HERO_COVER_WIDTH,
                                 X4ProHomeLayout::HERO_COVER_HEIGHT};
  const HomeLayoutRect heroTitle{CONTENT_X, heroY, HERO_LEFT_WIDTH, static_cast<int16_t>(safeTitleLh * 3)};
  const int16_t authorHeight = safeSmallLh > 24 ? safeSmallLh : static_cast<int16_t>(20);
  const HomeLayoutRect heroAuthor{CONTENT_X, static_cast<int16_t>(heroTitle.bottom() + 8), HERO_LEFT_WIDTH,
                                  authorHeight};
  const HomeLayoutRect progressTrack{CONTENT_X, static_cast<int16_t>(heroAuthor.bottom() + 8), HERO_LEFT_WIDTH, 11};
  const int16_t progressLabelHeight = safeSmallLh > 24 ? safeSmallLh : static_cast<int16_t>(18);
  const HomeLayoutRect progressLabel{CONTENT_X, static_cast<int16_t>(progressTrack.bottom() + 4), HERO_LEFT_WIDTH,
                                     progressLabelHeight};

  const int16_t heroBottom = std::max(heroCover.bottom(), progressLabel.bottom());
  const int16_t ctaY = static_cast<int16_t>(heroBottom + 20);
  const int16_t rightBtnX = static_cast<int16_t>(CONTENT_X + CONTENT_WIDTH - TOUCH_MIN);
  const int16_t leftBtnX = static_cast<int16_t>(rightBtnX - ACTION_GAP - TOUCH_MIN);
  const int16_t maxCtaWidth = static_cast<int16_t>(leftBtnX - 16 - CONTENT_X);
  const int16_t ctaWidth = std::clamp<int16_t>(176, 120, maxCtaWidth);

  const int16_t shelfHeaderY = static_cast<int16_t>(ctaY + TOUCH_MIN + 58);
  const int16_t shelfHeaderHeight = std::max<int16_t>(26, static_cast<int16_t>(safeSmallLh + 2));
  const int16_t row0Y = static_cast<int16_t>(shelfHeaderY + shelfHeaderHeight);
  const int16_t idealRowHeight = std::max<int16_t>(72, static_cast<int16_t>(safeSmallLh * 2 + 20));
  const int16_t maxRowHeight =
      std::max<int16_t>(TOUCH_MIN, static_cast<int16_t>((SHELF_BOTTOM_LIMIT - row0Y) / 2));
  const int16_t rowHeight = std::min(idealRowHeight, maxRowHeight);
  const int16_t row1Y = static_cast<int16_t>(row0Y + rowHeight);

  return X4ProHomeLayout{
      {CONTENT_X, 20, CONTENT_WIDTH, 28},
      {CONTENT_X, CONTENT_Y, CONTENT_WIDTH, CONTENT_HEIGHT},
      {CONTENT_X, INDEX_Y, CONTENT_WIDTH, indexHeight},
      heroCover,
      heroTitle,
      heroAuthor,
      progressTrack,
      progressLabel,
      {CONTENT_X, ctaY, ctaWidth, TOUCH_MIN},
      {leftBtnX, ctaY, TOUCH_MIN, TOUCH_MIN},
      {rightBtnX, ctaY, TOUCH_MIN, TOUCH_MIN},
      {CONTENT_X, shelfHeaderY, CONTENT_WIDTH, shelfHeaderHeight},
      {{{CONTENT_X, row0Y, CONTENT_WIDTH, rowHeight}, {CONTENT_X, row1Y, CONTENT_WIDTH, rowHeight}}},
      {CONTENT_X, TABS_Y, CONTENT_WIDTH, TABS_HEIGHT},
  };
}

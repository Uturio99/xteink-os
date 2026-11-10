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
  HomeLayoutRect milestoneLabel;
  HomeLayoutRect progressLabel;
  HomeLayoutRect progressTrack;
  HomeLayoutRect cta;
  HomeLayoutRect shelfHeader;
  std::array<HomeLayoutRect, 2> shelfRows;
  std::array<HomeLayoutRect, 2> shelfThumbs;
  std::array<HomeLayoutRect, 2> shelfTitles;
  std::array<HomeLayoutRect, 2> shelfAuthors;
  std::array<HomeLayoutRect, 2> shelfProgress;
  std::array<HomeLayoutRect, 3> utilityLinks;
};

constexpr int x4ProShelfRowCount(const int totalBooks) { return std::max(0, std::min(2, totalBooks - 1)); }

// Row 0 shows books[1], row 1 shows books[2]; hero is books[0].
constexpr int x4ProShelfBookIndex(const int row) { return row + 1; }

constexpr int x4ProVisibleBookCount(const int totalBooks) {
  return totalBooks > 0 ? 1 + x4ProShelfRowCount(totalBooks) : 0;
}

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
  constexpr int16_t SHELF_ROW_HEIGHT = 84;
  constexpr int16_t SHELF_ROW_GAP = 14;
  constexpr int16_t SHELF_THUMB_WIDTH = 40;
  constexpr int16_t SHELF_THUMB_HEIGHT = 56;
  constexpr int16_t SHELF_TEXT_GAP = 16;
  constexpr int16_t SHELF_LINE_GAP = 4;
  constexpr int16_t SHELF_TAG_WIDTH = 56;
  constexpr int16_t UTILITY_LINK_Y = 600;
  constexpr int16_t UTILITY_LINK_HEIGHT = 48;

  const int16_t titleY = HERO_COVER_Y;
  const int16_t titleHeight = static_cast<int16_t>(titleLineHeight * 3);
  const int16_t authorY = static_cast<int16_t>(titleY + titleHeight + 8);
  const int16_t milestoneY = static_cast<int16_t>(authorY + smallLineHeight + 4);
  const int16_t progressY = static_cast<int16_t>(milestoneY + smallLineHeight + 8);
  const int16_t trackY = static_cast<int16_t>(progressY + smallLineHeight + 6);
  const int16_t ctaY = static_cast<int16_t>(trackY + 2 + 22);
  const int16_t ctaWidth = std::min<int16_t>(CONTENT_WIDTH, std::max<int16_t>(48, requestedCtaWidth));
  const int16_t shelfHeaderY = static_cast<int16_t>(ctaY + 56 + 18);
  const int16_t shelfRowY =
      std::min<int16_t>(static_cast<int16_t>(shelfHeaderY + smallLineHeight + 12),
                        static_cast<int16_t>(UTILITY_LINK_Y - 2 * SHELF_ROW_HEIGHT - SHELF_ROW_GAP));

  X4ProHomeLayout layout{{0, 0, 480, 40},
                         {CONTENT_X, 52, CONTENT_WIDTH, 720},
                         {HERO_COVER_X, HERO_COVER_Y, HERO_COVER_WIDTH, HERO_COVER_HEIGHT},
                         {CONTENT_X, titleY, HERO_TEXT_WIDTH, titleHeight},
                         {CONTENT_X, authorY, HERO_TEXT_WIDTH, smallLineHeight},
                         {CONTENT_X, milestoneY, HERO_TEXT_WIDTH, smallLineHeight},
                         {CONTENT_X, progressY, HERO_TEXT_WIDTH, smallLineHeight},
                         {CONTENT_X, trackY, 120, 2},
                         {static_cast<int16_t>((480 - ctaWidth) / 2), ctaY, ctaWidth, 56},
                         {CONTENT_X, shelfHeaderY, CONTENT_WIDTH, smallLineHeight},
                         {},
                         {},
                         {},
                         {},
                         {},
                         {}};
  for (size_t i = 0; i < layout.shelfRows.size(); ++i) {
    const int16_t rowY = static_cast<int16_t>(shelfRowY + i * (SHELF_ROW_HEIGHT + SHELF_ROW_GAP));
    const int16_t thumbY = static_cast<int16_t>(rowY + (SHELF_ROW_HEIGHT - SHELF_THUMB_HEIGHT) / 2);
    const int16_t textX = static_cast<int16_t>(CONTENT_X + SHELF_THUMB_WIDTH + SHELF_TEXT_GAP);
    const int16_t tagX = static_cast<int16_t>(CONTENT_X + CONTENT_WIDTH - SHELF_TAG_WIDTH);
    const int16_t textWidth = static_cast<int16_t>(tagX - SHELF_TEXT_GAP - textX);
    const int16_t textTop = static_cast<int16_t>(
        rowY +
        std::max<int16_t>(0, static_cast<int16_t>((SHELF_ROW_HEIGHT - 2 * smallLineHeight - SHELF_LINE_GAP) / 2)));
    layout.shelfRows[i] = {CONTENT_X, rowY, CONTENT_WIDTH, SHELF_ROW_HEIGHT};
    layout.shelfThumbs[i] = {CONTENT_X, thumbY, SHELF_THUMB_WIDTH, SHELF_THUMB_HEIGHT};
    layout.shelfTitles[i] = {textX, textTop, textWidth, smallLineHeight};
    layout.shelfAuthors[i] = {textX, static_cast<int16_t>(textTop + smallLineHeight + SHELF_LINE_GAP), textWidth,
                              smallLineHeight};
    layout.shelfProgress[i] = {tagX, static_cast<int16_t>(rowY + (SHELF_ROW_HEIGHT - smallLineHeight) / 2),
                               SHELF_TAG_WIDTH, smallLineHeight};
  }
  // Three equal text links, in the Home band's drawn order.
  constexpr int16_t UTILITY_LINK_GAP = 12;
  for (size_t i = 0; i < layout.utilityLinks.size(); ++i) {
    layout.utilityLinks[i] = {CONTENT_X,
                              static_cast<int16_t>(UTILITY_LINK_Y + i * (UTILITY_LINK_HEIGHT + UTILITY_LINK_GAP)),
                              CONTENT_WIDTH, UTILITY_LINK_HEIGHT};
  }
  return layout;
}

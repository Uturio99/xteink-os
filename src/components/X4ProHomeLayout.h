#pragma once

#include <algorithm>
#include <array>
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
  HomeLayoutRect heading;
  HomeLayoutRect subline;
  HomeLayoutRect importAction;
  HomeLayoutRect emptyIcon;
  HomeLayoutRect emptyTitle;
  HomeLayoutRect emptyMessage;
  HomeLayoutRect emptyImport;
  HomeLayoutRect emptyTransfer;
  HomeLayoutRect emptyCatalog;
  HomeLayoutRect heroCover;
  HomeLayoutRect heroDetails;
  HomeLayoutRect heroEyebrow;
  HomeLayoutRect heroTitle;
  HomeLayoutRect heroAuthor;
  HomeLayoutRect progressLabel;
  HomeLayoutRect progressTrack;
  HomeLayoutRect cta;
  HomeLayoutRect shelfHeader;
  HomeLayoutRect shelfRule;
  std::array<HomeLayoutRect, 2> recentRows;
  std::array<HomeLayoutRect, 2> recentCovers;
  std::array<HomeLayoutRect, 2> recentTitles;
  std::array<HomeLayoutRect, 2> recentAuthors;
  HomeLayoutRect tabs;
};

constexpr int x4ProVisibleBookCount(const int totalBooks) { return std::min(3, std::max(0, totalBooks)); }

constexpr int x4ProHeroCtaAction(const bool hasContinueReading, const int totalBooks) {
  return hasContinueReading ? 0 : std::max(0, totalBooks) + 1;
}

constexpr int x4ProTransferMenuIndex(const bool hasCatalog) { return hasCatalog ? 3 : 2; }
constexpr int x4ProSettingsMenuIndex(const bool hasCatalog) { return hasCatalog ? 4 : 3; }

constexpr HomeLayoutRect x4ProRootTabRect(const int index) {
  return {static_cast<int16_t>(31 + index * 142), 744, 134, 44};
}

constexpr int x4ProRootTabAction(const int index, const int bookCount, const bool hasCatalog) {
  return index == 0 ? -1 : bookCount + (index == 1 ? 1 : x4ProSettingsMenuIndex(hasCatalog));
}

constexpr X4ProHomeLayout makeX4ProHomeLayout(const int16_t titleLineHeight, const int16_t smallLineHeight) {
  constexpr int16_t CONTENT_X = 31;
  constexpr int16_t CONTENT_WIDTH = 418;
  constexpr int16_t HERO_COVER_WIDTH = 145;
  constexpr int16_t HERO_COVER_HEIGHT = 210;
  constexpr int16_t HERO_GAP = 19;
  constexpr int16_t RECENT_ROW_GAP = 8;
  constexpr int16_t RECENT_ROW_HEIGHT = 78;
  constexpr int16_t RECENT_COVER_WIDTH = 42;
  constexpr int16_t RECENT_COVER_HEIGHT = 61;

  const int16_t heroInfoX = static_cast<int16_t>(CONTENT_X + HERO_COVER_WIDTH + HERO_GAP);
  const int16_t heroInfoWidth = static_cast<int16_t>(CONTENT_WIDTH - HERO_COVER_WIDTH - HERO_GAP);
  const int16_t heroTitleHeight = static_cast<int16_t>(titleLineHeight * 2);
  const int16_t heroAuthorY = static_cast<int16_t>(224 + heroTitleHeight + 8);
  const int16_t progressY = static_cast<int16_t>(heroAuthorY + smallLineHeight + 14);
  const int16_t trackY = static_cast<int16_t>(progressY + smallLineHeight + 5);
  const int16_t secondRecentY = static_cast<int16_t>(567 + RECENT_ROW_HEIGHT + RECENT_ROW_GAP);

  X4ProHomeLayout layout{{CONTENT_X, 29, CONTENT_WIDTH, 27},
                         {CONTENT_X, 99, CONTENT_WIDTH, 45},
                         {CONTENT_X, 149, CONTENT_WIDTH, 20},
                         {340, 99, 109, 44},
                         {208, 247, 64, 64},
                         {CONTENT_X, 323, CONTENT_WIDTH, 42},
                         {CONTENT_X + 24, 373, CONTENT_WIDTH - 48, 44},
                         {CONTENT_X, 437, CONTENT_WIDTH, 50},
                         {CONTENT_X, 499, CONTENT_WIDTH, 50},
                         {CONTENT_X, 561, CONTENT_WIDTH, 50},
                         {CONTENT_X, 194, HERO_COVER_WIDTH, HERO_COVER_HEIGHT},
                         {heroInfoX, 194, heroInfoWidth, HERO_COVER_HEIGHT},
                         {heroInfoX, 200, heroInfoWidth, smallLineHeight},
                         {heroInfoX, 224, heroInfoWidth, heroTitleHeight},
                         {heroInfoX, heroAuthorY, heroInfoWidth, smallLineHeight},
                         {heroInfoX, progressY, heroInfoWidth, smallLineHeight},
                         {heroInfoX, trackY, heroInfoWidth, 3},
                         {CONTENT_X, 428, CONTENT_WIDTH, 50},
                         {CONTENT_X, 507, CONTENT_WIDTH, 44},
                         {CONTENT_X, 557, CONTENT_WIDTH, 1},
                         {{{CONTENT_X, 567, CONTENT_WIDTH, RECENT_ROW_HEIGHT},
                           {CONTENT_X, secondRecentY, CONTENT_WIDTH, RECENT_ROW_HEIGHT}}},
                         {{{CONTENT_X, 575, RECENT_COVER_WIDTH, RECENT_COVER_HEIGHT},
                           {CONTENT_X, static_cast<int16_t>(secondRecentY + 8), RECENT_COVER_WIDTH,
                            RECENT_COVER_HEIGHT}}},
                         {{{static_cast<int16_t>(CONTENT_X + RECENT_COVER_WIDTH + 12), 575, 330, titleLineHeight},
                           {static_cast<int16_t>(CONTENT_X + RECENT_COVER_WIDTH + 12),
                            static_cast<int16_t>(secondRecentY + 8), 330, titleLineHeight}}},
                         {{{static_cast<int16_t>(CONTENT_X + RECENT_COVER_WIDTH + 12), 609, 330, smallLineHeight},
                           {static_cast<int16_t>(CONTENT_X + RECENT_COVER_WIDTH + 12),
                            static_cast<int16_t>(secondRecentY + 42), 330, smallLineHeight}}},
                         {CONTENT_X, 744, CONTENT_WIDTH, 44}};
  return layout;
}

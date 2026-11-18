#pragma once

#include <algorithm>
#include <cstdint>

namespace AtelierSelectionMetrics {
inline constexpr int BAR_WIDTH = 5;
inline constexpr int BAR_GAP = 16;
inline constexpr int INDENT = BAR_WIDTH + BAR_GAP;
inline constexpr int INDEX_COL_WIDTH = 44;
inline constexpr int BAR_V_PAD = 10;
inline constexpr int MIN_BAR_HEIGHT = 12;
inline constexpr int MIN_TEXT_WIDTH = 48;
}  // namespace AtelierSelectionMetrics

struct AtelierRowSelectionLayout {
  bool selected = false;
  int barX = 0;
  int barY = 0;
  int barWidth = 0;
  int barHeight = 0;
  int indexX = 0;
  int textX = 0;
  int textWidth = 0;
};

inline constexpr AtelierRowSelectionLayout makeAtelierRowSelectionLayout(const int rowX, const int rowY,
                                                                       const int rowWidth, const int rowHeight,
                                                                       const bool selected,
                                                                       const int rightReserved = 0) {
  const int shift = selected ? AtelierSelectionMetrics::INDENT : 0;
  const int barWidth = selected ? AtelierSelectionMetrics::BAR_WIDTH : 0;
  const int barHeight =
      selected ? std::max(AtelierSelectionMetrics::MIN_BAR_HEIGHT, rowHeight - AtelierSelectionMetrics::BAR_V_PAD * 2)
               : 0;
  const int indexX = rowX + shift;
  const int textX = rowX + AtelierSelectionMetrics::INDEX_COL_WIDTH + shift;
  const int textWidth = std::max(AtelierSelectionMetrics::MIN_TEXT_WIDTH, (rowX + rowWidth) - rightReserved - textX);
  return AtelierRowSelectionLayout{
      selected,
      rowX,
      rowY + AtelierSelectionMetrics::BAR_V_PAD,
      barWidth,
      barHeight,
      indexX,
      textX,
      textWidth,
  };
}

inline constexpr uint8_t ATELIER_FOCUS_CLEAN_INTERVAL = 8;

struct AtelierFocusRefreshPlan {
  bool bwFastOnly = false;
  bool useHalfRefresh = false;
  uint8_t nextCounter = 0;
};

// Pure focus-refresh policy: initial / forced / every 8th move runs full 3-pass
// grayscale (HALF_REFRESH on periodic cleanup), steps 1..7 run BW-only FAST_REFRESH.
inline constexpr AtelierFocusRefreshPlan planAtelierFocusRefresh(const uint8_t movesSinceClean, const bool forceFull,
                                                                 const bool cleanOnForceFull) {
  if (forceFull) {
    return AtelierFocusRefreshPlan{false, cleanOnForceFull, 0};
  }
  const int next = static_cast<int>(movesSinceClean) + 1;
  if (next >= ATELIER_FOCUS_CLEAN_INTERVAL) {
    return AtelierFocusRefreshPlan{false, true, 0};
  }
  return AtelierFocusRefreshPlan{true, false, static_cast<uint8_t>(next)};
}

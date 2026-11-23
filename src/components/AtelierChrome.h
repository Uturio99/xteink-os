#pragma once

#include <BoardConfig.h>
#include <FreeInkUI.h>
#include <GfxRenderer.h>
#include <HalDisplay.h>

#include <cstddef>
#include <cstdint>
#include <string>

#include "activities/ActivityManager.h"
#include "components/UiAppHost.h"

struct AtelierGlyph;
struct AtelierFont;

#if FREEINK_DEVICE_X4PRO

enum class AtelierTone : uint8_t { Ink, Muted, PaperOnInk, MutedOnInk };

#include "AtelierWrap.h"

namespace AtelierChrome {

constexpr int16_t MARGIN_X = 32;
constexpr int16_t CONTENT_W = 416;
constexpr int16_t HEADER_RULE_Y = 86;
constexpr int16_t LIST_TOP_MIN_Y = 90;
constexpr int16_t LIST_BOTTOM_MAX_Y = 734;
constexpr int16_t BOTTOM_NAV_Y = 740;
constexpr int16_t BOTTOM_NAV_H = 48;
// Top of the bottom-nav touch strip (extends to the screen bottom).
constexpr int16_t BOTTOM_NAV_HIT_Y = 736;

inline bool isPortrait480x800(const GfxRenderer& renderer) {
  return renderer.getScreenWidth() == 480 && renderer.getScreenHeight() == 800;
}

// 2-bit AA editorial font accessors backed by single-TU AtelierFontData.h
const AtelierFont& font8Bold();
const AtelierFont& font10Bold();
const AtelierFont& font11Reg();
const AtelierFont& font12Reg();
const AtelierFont& font12Bold();
const AtelierFont& font13Reg();
const AtelierFont& font13Bold();
const AtelierFont& font14Reg();
const AtelierFont& font15Light();
const AtelierFont& font16Med();
const AtelierFont& font17Serif();
const AtelierFont& font20Light();
const AtelierFont& font27Light();
const AtelierFont& font34Light();
int fontLineHeight(const AtelierFont& font);

// UTF-8 & typography helpers
void normalizeUtf8Stack(const char* src, char* dst, size_t cap);
uint32_t toUpperViCodepoint(uint32_t cp);
void formatUpperVi(const char* src, char* dst, size_t cap);
void formatUpperAuthor(const std::string& rawAuthor, char* dst, size_t cap);
int measureText(const GfxRenderer& renderer, const AtelierFont& font, const char* text, int8_t trackingPx = 0);
void drawText(const GfxRenderer& renderer, int x, int yTop, const AtelierFont& font, const char* text, AtelierTone tone,
              int8_t trackingPx = 0, int maxWidth = 480, int align = 0, bool ellipsisIfOverflow = true,
              bool forceFallback = false);
WrappedAtelierLines wrapText(const GfxRenderer& renderer, const AtelierFont& font, const char* text, int maxWidth,
                             int maxLines, bool ellipsisIfTruncated = false);

// 4-level grayscale AA drawing primitives
void paintPixel(const GfxRenderer& renderer, int x, int y, uint8_t shade);
void fillRect(const GfxRenderer& renderer, int x, int y, int w, int h, uint8_t shade);
void drawLineAA(const GfxRenderer& renderer, float x0, float y0, float x1, float y1, float halfWidth, uint8_t fgShade,
                uint8_t bgShade = 0);
void fillCapsule(const GfxRenderer& renderer, int x, int y, int w, int h);
void strokeCapsule(const GfxRenderer& renderer, int x, int y, int w, int h, uint8_t maxShade, float thickness);
void drawCapsuleButton(const GfxRenderer& renderer, int x, int y, int w, int h, const char* label,
                       const AtelierFont& font, bool filled, bool drawOutline = true, bool outerFocusRing = false,
                       bool ellipsis = true);
void fillCircle(const GfxRenderer& renderer, float cx, float cy, float radius, uint8_t shade, uint8_t bgShade = 0);

void drawWifiGlyph(const GfxRenderer& renderer, int x, int y);
void drawTocIcon(const GfxRenderer& renderer, int x, int y);
void drawSlidersIcon(const GfxRenderer& renderer, int x, int y);
void drawSearchIcon(const GfxRenderer& renderer, int x, int y);
void drawRefreshIcon(const GfxRenderer& renderer, int x, int y);
void drawRightArrow(const GfxRenderer& renderer, int x, int y, uint8_t fgShade, uint8_t bgShade = 0);
void drawChevron(const GfxRenderer& renderer, float tipX, float midY, float dx, float dy, float halfWidth = 0.7f,
                 uint8_t shade = 3);
void drawChevronRight(const GfxRenderer& renderer, int x, int y);
void drawToggleSwitch(const GfxRenderer& renderer, int x, int y, bool checked);

// 4-level grayscale pass orchestration + GfxRenderer text/dither redirection
struct AtelierFocusRefreshTracker {
  bool hasFullGrayscaleFrame = false;
  bool pendingFocusOnly = false;
  uint8_t focusMovesSinceClean = 0;
  int lastFullKey = 0;

  void reset() {
    hasFullGrayscaleFrame = false;
    pendingFocusOnly = false;
    focusMovesSinceClean = 0;
    lastFullKey = 0;
  }

  void render(GfxRenderer& renderer, bool focusEligible, int fullKey, bool cleanOnForceFull,
              void (*paintPass)(void*), void* ctx);
};

GfxRenderer::UiTextOverrideHook textHook(bool ditherAsGrayscalePlanes);
void renderGrayscalePass(GfxRenderer& renderer, HalDisplay::RefreshMode refresh, void (*paintPass)(void*), void* ctx,
                         bool bwFastOnly = false);

// Shared Concept 01 screen chrome
void drawStatusBar(const GfxRenderer& renderer, int x, int y, int w, int h, bool backButton = false);
void drawHeader(const GfxRenderer& renderer, const char* title, const char* rightMeta = nullptr, bool backButton = true,
                const char* eyebrowCode = nullptr, int countFallback = -1);
void drawTabBar(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, const freeink::ui::TabItem* tabs,
                uint16_t count, freeink::ui::ActionId action);
void drawEditorialList(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, freeink::ui::ListProps& props,
                       freeink::ui::ListNav& nav, bool hasBottomNav = true);

// Bottom navigation (0=Home, 1=Library, 2=FileBrowser, 3=Settings)
void drawBottomNav(const GfxRenderer& renderer, int activeTab, UiAppHost::UiScreen* screen = nullptr,
                   freeink::ui::ActionId action = 0, const int* actionValues = nullptr);
bool handleBottomNavTap(int tx, int ty, int currentTab, void (*goHomeFn)(), void (*goBackFn)());
int takePendingBottomNavTab();

// Shared Phase B Concept 01 renderers (scrubber, reader chrome, full-screen editorial, option sheet)
void drawScrubberTrack(const GfxRenderer& renderer, int x, int y, int w, int h, int permille, bool circleKnob = false,
                       bool allowGrayscale = false);
void drawReaderChrome(const GfxRenderer& renderer, const char* chapterTitle, int currentPage, int pageCount,
                      int bookPercent, bool isBookmarked, bool pageCountEstimated, bool showTop, bool showPageCount,
                      bool showProgressBar, bool showBattery, bool showClock, bool allowGrayscale = false);
void drawEditorialMessageScreen(const GfxRenderer& renderer, const char* eyebrowCode, const char* topRight,
                                const char* headline, const char* subline, const char* footerLeft,
                                const char* footerRight);
void drawNoveOsSplashScreen(const GfxRenderer& renderer, const char* eyebrowCode, const char* statusLine,
                            const char* subHint, const char* footerLeft, const char* footerRight);
void drawSubHeader(const GfxRenderer& renderer, const char* leftText, const char* rightText = nullptr);
void drawFooterHint(const GfxRenderer& renderer, const char* text);

struct AtelierStatusScreenSpec {
  const char* eyebrowCode = nullptr;
  const char* title = nullptr;
  const char* rightMeta = nullptr;
  bool backButton = true;
  const char* subtitle = nullptr;
  const char* subtitleRight = nullptr;
  const char* headline = nullptr;
  const char* body = nullptr;
  const char* detail = nullptr;
  int progressPct = -1;
  const char* progressLabel = nullptr;
  const char* footerHint = nullptr;
};

void paintStatusScreenContent(const GfxRenderer& renderer, const AtelierStatusScreenSpec& spec);
void drawStatusScreen(GfxRenderer& renderer, const AtelierStatusScreenSpec& spec,
                      HalDisplay::RefreshMode refresh = HalDisplay::FAST_REFRESH, bool bwFastOnly = false);

struct AtelierOptionSheetSpec {
  const char* title = nullptr;
  const char* headline = nullptr;
  const char* message = nullptr;
  const char* const* options = nullptr;
  int optionCount = 0;
  int selectedIndex = 0;
  int primaryIndex = 1;
};

using AtelierHitAdder = void (*)(void* ctx, freeink::ui::Rect rect, freeink::ui::ActionId action, int16_t value);
void renderOptionSheet(const GfxRenderer& renderer, const AtelierOptionSheetSpec& spec, AtelierHitAdder addHit,
                       void* hitCtx, freeink::ui::ActionId actionOption, freeink::ui::ActionId actionChrome);

template <typename FrameT>
inline void renderOptionSheet(FrameT& frame, const GfxRenderer& renderer, const AtelierOptionSheetSpec& spec,
                              const freeink::ui::ActionId actionOption, const freeink::ui::ActionId actionChrome = 0) {
  auto addHit = [](void* ctx, const freeink::ui::Rect rect, const freeink::ui::ActionId action, const int16_t value) {
    static_cast<FrameT*>(ctx)->hit(rect, action, value, freeink::ui::InputTouch);
  };
  renderOptionSheet(renderer, spec, addHit, &frame, actionOption, actionChrome);
}

inline void renderOptionSheet(UiAppHost::UiScreen& screen, const GfxRenderer& renderer,
                              const AtelierOptionSheetSpec& spec, const freeink::ui::ActionId actionOption,
                              const freeink::ui::ActionId actionChrome = 0) {
  renderOptionSheet(screen.frame(), renderer, spec, actionOption, actionChrome);
}

}  // namespace AtelierChrome

#endif

#include "CoverGridHomeUi.h"

#include <BitmapHelpers.h>
#include <BoardConfig.h>
#include <CrossPointSettings.h>
#include <Epub.h>
#include <FsHelpers.h>
#include <GfxRenderer.h>
#include <HalClock.h>
#include <HalPowerManager.h>
#include <HalStorage.h>
#include <I18n.h>
#include <Memory.h>
#include <Utf8.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <utility>

#include "AtelierChrome.h"
#include "AtelierSelection.h"
#include "MappedInputManager.h"
#include "UITheme.h"
#include "X4ProHomeLayout.h"
#include "icons/blocks.h"
#include "icons/book.h"
#include "icons/folder.h"
#include "icons/library.h"
#include "icons/settings2.h"
#include "icons/transfer.h"
#include "util/BookProgress.h"

namespace fui = freeink::ui;
namespace {
constexpr fui::ActionId SELECT = 1;
constexpr int16_t COVER_CELL_INSET = 6;
constexpr int COVER_ROW_EXTRA_HEIGHT = 8;

#if FREEINK_DEVICE_X4PRO
int sShelfProgress[X4ProHomeLayout::SHELF_ROW_COUNT]{-1, -1};
int sHeroChapter = -1;

int readHeroChapterNumber(const std::string& path) {
  if (!FsHelpers::hasReflowableBookExtension(path)) return -1;
  auto epub = makeUniqueNoThrow<Epub>(path, "/.crosspoint");
  if (!epub || !epub->load(false, true)) return -1;
  HalFile file;
  uint8_t data[4]{};
  if (!Storage.openFileForRead("HOME", epub->getCachePath() + "/progress.bin", file) || file.read(data, 4) != 4) {
    return -1;
  }
  const int spine = data[0] | (data[1] << 8);
  if (spine < 0 || spine >= epub->getSpineItemsCount()) return -1;
  return spine + 1;
}

void drawHomeBottomNav(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, const int libraryAction,
                       const int fileBrowserAction, const int settingsAction) {
  const int tabActions[4] = {-1, libraryAction, fileBrowserAction, settingsAction};
  AtelierChrome::drawBottomNav(renderer, 0, &screen, SELECT, tabActions);
}

void drawAtelierEditorialCover(const GfxRenderer& renderer, const HomeLayoutRect& r, const RecentBook& book,
                               const int progress) {
  AtelierChrome::fillRect(renderer, r.x, r.y, r.width, r.height, 3);

  // Clipped r=70px decorative ring arc (.c-ring: right:-46px, top:38px)
  const float ringCx = static_cast<float>(r.right() + 46 - 70);
  const float ringCy = static_cast<float>(r.y + 38 + 70);
  for (int py = r.y; py < r.bottom(); ++py) {
    for (int px = r.x; px < r.right(); ++px) {
      const float d = std::fabs(std::hypotf((px + 0.5f) - ringCx, (py + 0.5f) - ringCy) - 70.0f);
      if (d <= 0.65f) AtelierChrome::paintPixel(renderer, px, py, 2);
    }
  }

  constexpr int PAD_X = 11;
  constexpr int PAD_Y = 12;
  const int innerW = r.width - PAD_X * 2;
  char upperAuthor[64]{};
  AtelierChrome::formatUpperAuthor(book.author, upperAuthor, sizeof(upperAuthor));
  if (upperAuthor[0] != '\0') {
    AtelierChrome::drawText(renderer, r.x + PAD_X, r.y + PAD_Y, AtelierChrome::font8Bold(), upperAuthor,
                            AtelierTone::MutedOnInk, 1, innerW, 0, true);
  }

  // Split title on " — " for cover main title
  char mainTitle[128]{};
  AtelierChrome::normalizeUtf8Stack(book.title.c_str(), mainTitle, sizeof(mainTitle));
  if (char* dashPos = std::strstr(mainTitle, " — ")) *dashPos = '\0';

  const WrappedAtelierLines coverLines = AtelierChrome::wrapText(renderer, AtelierChrome::font17Serif(), mainTitle, innerW, 3);
  const int lineStep = 20;
  const int blockH = std::max(1, coverLines.count) * lineStep;
  const int blockY = r.y + (r.height - blockH) / 2 - 2;
  for (int i = 0; i < coverLines.count; ++i) {
    AtelierChrome::drawText(renderer, r.x + PAD_X, blockY + i * lineStep, AtelierChrome::font17Serif(),
                            coverLines.lines[i], AtelierTone::PaperOnInk, 0, innerW, 0, i + 1 == coverLines.count, coverLines.useFallback);
  }

  const int ruleY = r.bottom() - 28;
  AtelierChrome::fillRect(renderer, r.x + PAD_X, ruleY, innerW, 1, 2);
  char footer[32]{};
  const char* fmt = FsHelpers::hasXtcExtension(book.path) ? "XTC" : "EPUB";
  if (progress >= 0) {
    snprintf(footer, sizeof(footer), "%s · %d%%", fmt, progress);
  } else {
    snprintf(footer, sizeof(footer), "%s", fmt);
  }
  AtelierChrome::drawText(renderer, r.x + PAD_X, ruleY + 6, AtelierChrome::font8Bold(), footer, AtelierTone::MutedOnInk,
                          1, innerW, 0, true);
}
#endif
}  // namespace

CoverGridHomeUi::CoverGridHomeUi(GfxRenderer& renderer)
    : UiAppHost(renderer), coverCache(renderer), renderer(renderer) {}

void CoverGridHomeUi::begin(const std::vector<RecentBook>& recent, bool opds, bool continuing) {
  books = &recent;
  hasOpds = opds;
  hasContinueReading = continuing;
  if (!recent.empty()) coverCache.begin();
  resetUi();
  app.on(SELECT, &CoverGridHomeUi::onAction, this);
  app.setScreen(&CoverGridHomeUi::screenFn, this);
#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    sHeroChapter = hasContinueReading && !books->empty() ? readHeroChapterNumber(books->front().path) : -1;
    for (size_t i = 0; i < X4ProHomeLayout::SHELF_ROW_COUNT; ++i) {
      sShelfProgress[i] = i + 1 < books->size() ? loadBookProgress((*books)[i + 1].path) : -1;
    }
  } else
#endif
  {
    refreshCoverPaths();
  }
  progress = hasContinueReading && !books->empty() ? loadBookProgress(books->front().path) : -1;
  if (progress >= 0) snprintf(progressText, sizeof(progressText), "%d%%", progress);
}

void CoverGridHomeUi::refreshCoverPaths() {
  coverCache.invalidate();
  for (size_t i = 0; i < books->size() && i < coverPaths.size(); ++i) refreshCoverPath(i);
}

void CoverGridHomeUi::refreshCoverPath(size_t index) {
  if (index >= books->size() || index >= coverPaths.size()) return;
  coverCache.invalidate(index);
  coverPaths[index] = thumbHeights[index] > 0
                          ? UITheme::getCoverThumbPath((*books)[index].coverBmpPath, thumbHeights[index])
                          : std::string();
}

int CoverGridHomeUi::thumbHeightFor(const size_t index) const {
  return index < thumbHeights.size() && thumbHeights[index] > 0 ? thumbHeights[index] : THUMB_HEIGHT;
}

bool CoverGridHomeUi::takeThumbHeightChanged() { return std::exchange(thumbHeightChanged, false); }

void CoverGridHomeUi::noteThumbHeight(int slotHeight) {
  const int height = std::max(1, slotHeight + 8);
  bool changed = false;
  for (size_t i = 0; i < books->size() && i < thumbHeights.size(); ++i) {
    if (thumbHeights[i] == height) continue;
    thumbHeights[i] = height;
    changed = true;
  }
  if (!changed) return;
  thumbHeightChanged = true;
  refreshCoverPaths();
}

void CoverGridHomeUi::onAction(const fui::ActionEvent& event, void* user) {
  auto& self = *static_cast<CoverGridHomeUi*>(user);
  self.pending = event.value;
  self.app.clearTapFlash();
}

int CoverGridHomeUi::selectedAction(const MappedInputManager& input) {
  pending = -1;
  const auto touch = routeTouch(input);
  return touch.snap.touchReleased ? pending : -1;
}

int CoverGridHomeUi::visibleBookCount() const {
  if (!books) return 0;
#if FREEINK_DEVICE_X4PRO
  if (renderer.getScreenWidth() == 480 && renderer.getScreenHeight() == 800) {
    return x4ProVisibleBookCount(static_cast<int>(books->size()));
  }
#endif
  return static_cast<int>(books->size());
}

void CoverGridHomeUi::screenFn(UiScreen& screen, void* user) { static_cast<CoverGridHomeUi*>(user)->draw(screen); }

void CoverGridHomeUi::draw(UiScreen& screen) {
#if FREEINK_DEVICE_X4PRO
  if (renderer.getScreenWidth() == 480 && renderer.getScreenHeight() == 800) {
    drawX4ProPortrait(screen);
    return;
  }
#endif
  coverCache.prepare();
  const auto& theme = screen.theme();
  const auto safe = UITheme::getInstance().getScreenSafeArea(renderer, true);
  const int corner = BoardConfig::ACTIVE.cornerRadius;
  const auto left = static_cast<int16_t>(std::max(safe.x, corner));
  const auto right = static_cast<int16_t>(std::max(renderer.getScreenWidth() - safe.x - safe.width, corner));
  screen.setContentMarginFromScreen(fui::Insets{static_cast<int16_t>(safe.y), right,
                                                static_cast<int16_t>(renderer.getScreenHeight() - safe.y - safe.height),
                                                left});
  const int16_t hInset = std::max<int16_t>(
      0, static_cast<int16_t>(UITheme::getInstance().getMetrics().headerSidePadding - COVER_CELL_INSET - left));
  const int16_t topInset =
      BoardConfig::hasTouch() ? static_cast<int16_t>(theme.spaceLg) : static_cast<int16_t>(theme.spaceSm + 4);
  screen.insetContent(fui::Insets{topInset, hInset, theme.spaceSm, hInset});
  const bool landscape = renderer.getScreenWidth() > renderer.getScreenHeight();
  screen.takeTop(UITheme::getInstance().getMetrics().batteryBarHeight, theme.spaceSm);
  const int16_t tabGap = BoardConfig::hasTouch() ? theme.spaceSm : static_cast<int16_t>(4);
  const auto& board = BoardConfig::ACTIVE;
  auto tabRect = screen.takeBottom(UITheme::getInstance().getMetrics().coverGridTabBarHeight, tabGap);
  const auto tabHeight = static_cast<int16_t>(tabRect.height * board.tabBarHeightPct / 100);
  tabRect.y = static_cast<int16_t>(tabRect.y + tabRect.height - tabHeight + board.tabBarOffsetY);
  tabRect.height = tabHeight;
  const auto fitTabWidth = [&board](fui::Rect rect) {
    const auto width = static_cast<int16_t>(rect.width * board.tabBarWidthPct / 100);
    rect.x = static_cast<int16_t>(rect.x + (rect.width - width) / 2);
    rect.width = width;
    return rect;
  };
  if (books->empty()) {
    drawTabs(screen, fitTabWidth(tabRect.inset(fui::Insets{0, COVER_CELL_INSET, 0, COVER_CELL_INSET})));
    drawEmpty(screen);
    drawHeaderBand();
    return;
  }
  const fui::Rect body = screen.body();
  const int rowGap = std::max<int>(4, body.width / 100);
  grid.gap = grid.rowGap = rowGap;
  const int coverRowHeight = std::max(1, (body.height - theme.spaceSm - 2 * rowGap) / 3 + COVER_ROW_EXTRA_HEIGHT);
  const int16_t featuredHeight = std::min<int>(
      body.height, std::max<int>(coverRowHeight, screen.target().lineHeight(theme.bodyText.font) * (landscape ? 1 : 2) +
                                                     screen.target().lineHeight(theme.smallText.font) * 2 + 32));
  drawCurrent(screen, screen.takeTop(featuredHeight, theme.spaceSm), coverRowHeight);
  drawGrid(screen);
  tabRect.x = gridBounds.x + grid.cellInset.left;
  tabRect.width = gridBounds.width - grid.cellInset.left - grid.cellInset.right;
  drawTabs(screen, fitTabWidth(tabRect));
  drawHeaderBand();
}

#if FREEINK_DEVICE_X4PRO
void CoverGridHomeUi::drawX4ProPortrait(UiScreen& screen) {
  constexpr auto layout = makeX4ProHomeLayout(29, 24);
  screen.setContentMarginFromScreen(fui::Insets{layout.content.y,
                                                static_cast<int16_t>(480 - layout.content.right()),
                                                static_cast<int16_t>(800 - layout.content.bottom()), layout.content.x});
  const int bookCount = static_cast<int>(books->size());
  const int fileBrowserAction = bookCount;
  const int libraryAction = bookCount + 1;
  const int transferAction = bookCount + (hasOpds ? 3 : 2);
  const int settingsAction = bookCount + (hasOpds ? 4 : 3);

  AtelierChrome::drawStatusBar(renderer, layout.masthead.x, layout.masthead.y, layout.masthead.width,
                               layout.masthead.height, false);
  screen.frame().hit(fui::Rect{static_cast<int16_t>(layout.masthead.right() - 72), 8, 72, 44}, SELECT,
                     static_cast<int16_t>(settingsAction), fui::InputTouch, fui::StateNormal);

  if (books->empty()) {
    char upperHome[48]{};
    AtelierChrome::formatUpperVi(tr(STR_NAV_HOME), upperHome, sizeof(upperHome));
    char emptyEyebrow[64]{};
    snprintf(emptyEyebrow, sizeof(emptyEyebrow), "01.1 — %s", upperHome);
    AtelierChrome::drawText(renderer, 32, 160, AtelierChrome::font10Bold(), emptyEyebrow, AtelierTone::Muted, 1, 416, 0,
                            false);

    const WrappedAtelierLines headlineLines =
        AtelierChrome::wrapText(renderer, AtelierChrome::font34Light(), tr(STR_HOME_EMPTY_HEADLINE), 416, 2);
    int headlineY = 194;
    for (int i = 0; i < headlineLines.count; ++i) {
      AtelierChrome::drawText(renderer, 32, headlineY, AtelierChrome::font34Light(), headlineLines.lines[i],
                              AtelierTone::Ink, 0, 416, 0, i + 1 == headlineLines.count, headlineLines.useFallback);
      headlineY += 38;
    }
    AtelierChrome::drawText(renderer, 32, 286, AtelierChrome::font14Reg(), tr(STR_NO_OPEN_BOOK), AtelierTone::Muted, 0,
                            416, 0, false);

    constexpr int ctaX = 32;
    constexpr int ctaY = 344;
    const char* openLibLabel = tr(STR_HOME_OPEN_LIBRARY);
    const int ctaW =
        std::clamp(AtelierChrome::measureText(renderer, AtelierChrome::font13Bold(), openLibLabel, 0) + 60, 176, 224);
    constexpr int ctaH = 44;
    AtelierChrome::fillCapsule(renderer, ctaX, ctaY, ctaW, ctaH);
    AtelierChrome::drawText(renderer, ctaX + 20,
                            ctaY + (ctaH - AtelierChrome::fontLineHeight(AtelierChrome::font13Bold())) / 2,
                            AtelierChrome::font13Bold(), openLibLabel, AtelierTone::PaperOnInk, 0, ctaW - 56, 0, true);
    AtelierChrome::drawRightArrow(renderer, ctaX + ctaW - 34, ctaY + (ctaH - 16) / 2, 0, 3);
    screen.frame().hit(fui::Rect{ctaX, ctaY, static_cast<int16_t>(ctaW), ctaH}, SELECT,
                       static_cast<int16_t>(libraryAction), fui::InputTouch, fui::StateNormal);

    const int transferX = ctaX + ctaW + 12;
    const char* transferLabel = tr(STR_FILE_TRANSFER);
    const int transferW =
        std::clamp(AtelierChrome::measureText(renderer, AtelierChrome::font13Reg(), transferLabel, 0) + 36, 148, 448 - transferX);
    AtelierChrome::drawCapsuleButton(renderer, transferX, 344, transferW, 44, transferLabel,
                                     AtelierChrome::font13Reg(), false, true, false, true);
    screen.frame().hit(
        fui::Rect{static_cast<int16_t>(transferX), 344, static_cast<int16_t>(transferW), 44}, SELECT,
        static_cast<int16_t>(transferAction), fui::InputTouch, fui::StateNormal);

    const char* browseLabel = tr(STR_BROWSE_FILES);
    const int browseW =
        std::clamp(AtelierChrome::measureText(renderer, AtelierChrome::font13Reg(), browseLabel, 0) + 36, 148, 280);
    AtelierChrome::drawCapsuleButton(renderer, 32, 400, browseW, 44, browseLabel, AtelierChrome::font13Reg(), false,
                                     true, false, true);
    screen.frame().hit(fui::Rect{32, 400, static_cast<int16_t>(browseW), 44}, SELECT,
                       static_cast<int16_t>(fileBrowserAction), fui::InputTouch, fui::StateNormal);

    drawHomeBottomNav(screen, renderer, libraryAction, fileBrowserAction, settingsAction);
    return;
  }

  const auto& hero = books->front();

  // 01.1 Eyebrow
  char upperNowReading[48]{};
  AtelierChrome::formatUpperVi(tr(STR_HOME_NOW_READING), upperNowReading, sizeof(upperNowReading));
  char leftEyebrow[64]{};
  snprintf(leftEyebrow, sizeof(leftEyebrow), "01.1 — %s", upperNowReading);
  AtelierChrome::drawText(renderer, layout.indexRow.x, layout.indexRow.y + 4, AtelierChrome::font10Bold(),
                          leftEyebrow, AtelierTone::Muted, 1, 240, 0, true);
  char rightEyebrow[40]{};
  if (sHeroChapter > 0) {
    char upperChapter[24]{};
    AtelierChrome::formatUpperVi(tr(STR_CHAPTER), upperChapter, sizeof(upperChapter));
    snprintf(rightEyebrow, sizeof(rightEyebrow), "%s %02d", upperChapter, sHeroChapter);
  } else {
    snprintf(rightEyebrow, sizeof(rightEyebrow), "%s", FsHelpers::hasXtcExtension(hero.path) ? "XTC" : "EPUB");
  }
  AtelierChrome::drawText(renderer, layout.indexRow.x + 240, layout.indexRow.y + 4, AtelierChrome::font10Bold(),
                          rightEyebrow, AtelierTone::Muted, 1, layout.indexRow.width - 240, 2, false);

  // Split "Title — Subtitle" cleanly so hero editorial stack mirrors #s1
  char heroMain[192]{};
  AtelierChrome::normalizeUtf8Stack(hero.title.c_str(), heroMain, sizeof(heroMain));
  const char* heroSub = nullptr;
  if (char* emDash = std::strstr(heroMain, " — ")) {
    *emDash = '\0';
    heroSub = emDash + 5;
  }
  const WrappedAtelierLines titleLines =
      AtelierChrome::wrapText(renderer, AtelierChrome::font27Light(), heroMain, layout.heroTitle.width, 2);
  int titleY = layout.heroTitle.y;
  for (int i = 0; i < titleLines.count; ++i) {
    AtelierChrome::drawText(renderer, layout.heroTitle.x, titleY, AtelierChrome::font27Light(), titleLines.lines[i],
                            AtelierTone::Ink, 0, layout.heroTitle.width, 0, i + 1 == titleLines.count && !heroSub, titleLines.useFallback);
    titleY += 30;
  }
  if (heroSub && heroSub[0] != '\0') {
    AtelierChrome::drawText(renderer, layout.heroTitle.x, titleY + 2, AtelierChrome::font20Light(), heroSub,
                            AtelierTone::Muted, 0, layout.heroTitle.width, 0, true);
  }

  if (!hero.author.empty()) {
    AtelierChrome::drawText(renderer, layout.heroAuthor.x, layout.heroAuthor.y, AtelierChrome::font13Reg(),
                            hero.author.c_str(), AtelierTone::Muted, 0, layout.heroAuthor.width, 0, true);
  }

  // Delicate 1px scrubber track + tick
  const int trackY = layout.progressTrack.y + layout.progressTrack.height / 2;
  AtelierChrome::fillRect(renderer, layout.progressTrack.x, trackY, layout.progressTrack.width, 1, 1);
  if (progress >= 0) {
    const int16_t tickX = x4ProScrubberTickX(layout.progressTrack, progress);
    const int fillW = std::max(0, static_cast<int>(tickX) - layout.progressTrack.x);
    if (fillW > 0) AtelierChrome::fillRect(renderer, layout.progressTrack.x, trackY, fillW, 1, 3);
    AtelierChrome::fillRect(renderer, tickX, layout.progressTrack.y, 2, layout.progressTrack.height, 3);
    char leftProgressLabel[48]{};
    if (sHeroChapter > 0) {
      const int pageEst = (progress * 238 + 50) / 100;
      snprintf(leftProgressLabel, sizeof(leftProgressLabel), tr(STR_ATELIER_PAGE_OF_FMT), pageEst, 238);
    } else {
      snprintf(leftProgressLabel, sizeof(leftProgressLabel), "%s", tr(STR_READING_PROGRESS));
    }
    AtelierChrome::drawText(renderer, layout.progressLabel.x, layout.progressLabel.y, AtelierChrome::font11Reg(),
                            leftProgressLabel, AtelierTone::Muted, 0, 180, 0, true);
    AtelierChrome::drawText(renderer, layout.progressLabel.x + 180, layout.progressLabel.y, AtelierChrome::font11Reg(),
                            progressText, AtelierTone::Muted, 0, layout.progressLabel.width - 180, 2, false);
  }

  // Frameless Editorial Cover + touch hit
  drawAtelierEditorialCover(renderer, layout.heroCover, hero, progress);
  screen.frame().hit(fui::Rect{layout.heroCover.x, layout.heroCover.y, layout.heroCover.width, layout.heroCover.height},
                     SELECT, 0, fui::InputTouch, fui::StateNormal);

  // Action row: Apple capsule CTA + two 44x44 outlined circular buttons
  const int16_t ctaAction = static_cast<int16_t>(x4ProHeroCtaAction(hasContinueReading, bookCount));
  const char* ctaText = hasContinueReading ? tr(STR_CONTINUE_READING) : tr(STR_LIBRARY);
  const int heroCtaW = std::clamp(AtelierChrome::measureText(renderer, AtelierChrome::font13Bold(), ctaText, 0) + 60,
                                  static_cast<int>(layout.cta.width),
                                  static_cast<int>(layout.btnSecondaryLeft.x - 16 - layout.cta.x));
  AtelierChrome::fillCapsule(renderer, layout.cta.x, layout.cta.y, heroCtaW, layout.cta.height);
  AtelierChrome::drawText(
      renderer, layout.cta.x + 20,
      layout.cta.y + (layout.cta.height - AtelierChrome::fontLineHeight(AtelierChrome::font13Bold())) / 2,
      AtelierChrome::font13Bold(), ctaText, AtelierTone::PaperOnInk, 0, heroCtaW - 56, 0, true);
  AtelierChrome::drawRightArrow(renderer, layout.cta.x + heroCtaW - 34, layout.cta.y + (layout.cta.height - 16) / 2, 0,
                                3);
  screen.frame().hit(fui::Rect{layout.cta.x, layout.cta.y, static_cast<int16_t>(heroCtaW), layout.cta.height}, SELECT,
                     ctaAction, fui::InputTouch, fui::StateNormal);

  const bool leftFocused = selected == libraryAction;
  AtelierChrome::strokeCapsule(renderer, layout.btnSecondaryLeft.x, layout.btnSecondaryLeft.y,
                               layout.btnSecondaryLeft.width, layout.btnSecondaryLeft.height, leftFocused ? 3 : 1,
                               leftFocused ? 2.0f : 1.0f);
  AtelierChrome::drawTocIcon(renderer, layout.btnSecondaryLeft.x + (layout.btnSecondaryLeft.width - 16) / 2,
                             layout.btnSecondaryLeft.y + (layout.btnSecondaryLeft.height - 16) / 2);
  screen.frame().hit(fui::Rect{layout.btnSecondaryLeft.x, layout.btnSecondaryLeft.y, layout.btnSecondaryLeft.width,
                               layout.btnSecondaryLeft.height},
                     SELECT, static_cast<int16_t>(libraryAction), fui::InputTouch, fui::StateNormal);

  const bool rightFocused = selected == settingsAction;
  AtelierChrome::strokeCapsule(renderer, layout.btnSecondaryRight.x, layout.btnSecondaryRight.y,
                               layout.btnSecondaryRight.width, layout.btnSecondaryRight.height, rightFocused ? 3 : 1,
                               rightFocused ? 2.0f : 1.0f);
  AtelierChrome::drawSlidersIcon(renderer, layout.btnSecondaryRight.x + (layout.btnSecondaryRight.width - 16) / 2,
                                 layout.btnSecondaryRight.y + (layout.btnSecondaryRight.height - 16) / 2);
  screen.frame().hit(fui::Rect{layout.btnSecondaryRight.x, layout.btnSecondaryRight.y, layout.btnSecondaryRight.width,
                               layout.btnSecondaryRight.height},
                     SELECT, static_cast<int16_t>(settingsAction), fui::InputTouch, fui::StateNormal);

  // 01.2 Shelf Header + 1px bottom Ink hairline
  char upperRecent[64]{};
  AtelierChrome::formatUpperVi(tr(STR_MENU_RECENT_BOOKS), upperRecent, sizeof(upperRecent));
  char shelfEyebrow[80]{};
  snprintf(shelfEyebrow, sizeof(shelfEyebrow), "01.2 — %s", upperRecent);
  AtelierChrome::drawText(renderer, layout.shelfHeader.x, layout.shelfHeader.y + 2, AtelierChrome::font10Bold(),
                          shelfEyebrow, AtelierTone::Ink, 1, 260, 0, true);
  char countBuf[32]{};
  snprintf(countBuf, sizeof(countBuf), tr(STR_ATELIER_COUNT_BOOKS_FMT), bookCount);
  AtelierChrome::drawText(renderer, layout.shelfHeader.x + 260, layout.shelfHeader.y + 2, AtelierChrome::font10Bold(),
                          countBuf, AtelierTone::Muted, 1, layout.shelfHeader.width - 260, 2, false);
  screen.frame().hit(fui::Rect{static_cast<int16_t>(layout.shelfHeader.right() - 120), layout.shelfHeader.y, 120,
                               layout.shelfHeader.height},
                     SELECT, static_cast<int16_t>(libraryAction), fui::InputTouch, fui::StateNormal);
  AtelierChrome::fillRect(renderer, layout.shelfHeader.x, layout.shelfHeader.bottom() - 1, layout.shelfHeader.width, 1,
                          3);

  // Editorial Index Rows (02, 03)
  const int shelfCount = x4ProShelfRowCount(bookCount);
  for (int i = 0; i < shelfCount; ++i) {
    const size_t bookIndex = static_cast<size_t>(i + 1);
    const auto& book = (*books)[bookIndex];
    const auto& row = layout.shelfRows[static_cast<size_t>(i)];
    screen.frame().hit(fui::Rect{row.x, row.y, row.width, row.height}, SELECT, static_cast<int16_t>(bookIndex),
                       fui::InputTouch, fui::StateNormal);

    const bool isSel = selected == static_cast<int>(bookIndex);

    char rightMetric[24]{};
    if (sShelfProgress[i] >= 0) {
      snprintf(rightMetric, sizeof(rightMetric), "%d%%", sShelfProgress[i]);
    } else {
      snprintf(rightMetric, sizeof(rightMetric), "%s", FsHelpers::hasXtcExtension(book.path) ? "XTC" : "EPUB");
    }
    const int metricW = AtelierChrome::measureText(renderer, AtelierChrome::font12Reg(), rightMetric, 0);
    const int chevronX = row.right() - 16;
    const auto sel = makeAtelierRowSelectionLayout(row.x, row.y, row.width, row.height, isSel, 34 + metricW);
    if (sel.selected) {
      AtelierChrome::fillRect(renderer, sel.barX, sel.barY, sel.barWidth, sel.barHeight, 3);
    }

    char idxBuf[16]{};
    snprintf(idxBuf, sizeof(idxBuf), "%02d", i + 2);
    const auto& idxFont = sel.selected ? AtelierChrome::font13Bold() : AtelierChrome::font15Light();
    AtelierChrome::drawText(renderer, sel.indexX, row.y + (row.height - AtelierChrome::fontLineHeight(idxFont)) / 2,
                            idxFont, idxBuf, sel.selected ? AtelierTone::Ink : AtelierTone::Muted, 0, 28, 0, false);

    AtelierChrome::drawChevronRight(renderer, chevronX, row.y + (row.height - 14) / 2);
    AtelierChrome::drawText(renderer, chevronX - 10 - metricW,
                            row.y + (row.height - AtelierChrome::fontLineHeight(AtelierChrome::font12Reg())) / 2,
                            AtelierChrome::font12Reg(), rightMetric, AtelierTone::Muted, 0, metricW + 4, 0, false);

    const bool hasAuthor = !book.author.empty();
    const int blockH = hasAuthor ? (AtelierChrome::fontLineHeight(AtelierChrome::font16Med()) + 3 +
                                    AtelierChrome::fontLineHeight(AtelierChrome::font12Reg()))
                                 : AtelierChrome::fontLineHeight(AtelierChrome::font16Med());
    const int textY = row.y + (row.height - 1 - blockH) / 2;
    AtelierChrome::drawText(renderer, sel.textX, textY, AtelierChrome::font16Med(), book.title.c_str(), AtelierTone::Ink,
                            0, sel.textWidth, 0, true);
    if (hasAuthor) {
      AtelierChrome::drawText(renderer, sel.textX,
                              textY + AtelierChrome::fontLineHeight(AtelierChrome::font16Med()) + 3,
                              AtelierChrome::font12Reg(), book.author.c_str(), AtelierTone::Muted, 0, sel.textWidth, 0,
                              true);
    }

    AtelierChrome::fillRect(renderer, row.x, row.bottom() - 1, row.width, 1, 1);
  }

  drawHomeBottomNav(screen, renderer, libraryAction, fileBrowserAction, settingsAction);
}
#endif

void CoverGridHomeUi::drawHeaderBand() {
  const ThemeMetrics& metrics = UITheme::getInstance().getMetrics();
  GUI.drawHeader(renderer, Rect{0, metrics.topPadding, renderer.getScreenWidth(), metrics.batteryBarHeight}, nullptr);
}

void CoverGridHomeUi::drawEmpty(UiScreen& screen) {
  const auto& theme = screen.theme();
  const auto body = screen.body();
  auto title = theme.titleText;
  title.bold = true;
  title.align = fui::TextAlign::Center;
  auto message = theme.bodyText;
  message.align = fui::TextAlign::Center;
  constexpr int16_t ICON_SIZE = 32;
  const int16_t titleHeight = screen.target().lineHeight(title.font);
  const int16_t messageHeight = screen.target().lineHeight(message.font);
  const int16_t contentHeight = ICON_SIZE + theme.spaceLg + titleHeight + theme.spaceSm + messageHeight;
  int16_t y = body.y + std::max(0, (body.height - contentHeight) / 2);
  renderer.drawIcon(BookIcon, body.x + (body.width - ICON_SIZE) / 2, y, ICON_SIZE);
  y += ICON_SIZE + theme.spaceLg;
  screen.target().text(fui::Rect{body.x, y, body.width, titleHeight}, tr(STR_NO_OPEN_BOOK), title);
  y += titleHeight + theme.spaceSm;
  screen.target().text(fui::Rect{body.x, y, body.width, messageHeight}, tr(STR_START_READING), message);
}

void CoverGridHomeUi::drawCurrent(UiScreen& screen, fui::Rect rect, const int coverRowHeight) {
  const auto& theme = screen.theme();
  const auto& book = books->front();
  card.title = book.title.c_str();
  card.author = book.author.empty() ? nullptr : book.author.c_str();
  card.meta = nullptr;
  card.progressLabel = progress >= 0 ? progressText : nullptr;
  card.centerTextOnCover = true;
  card.progress = std::max(0, progress);
  card.progressMax = progress >= 0 ? 100 : 0;
  card.action = SELECT;
  card.state = fui::StateNormal;
  card.styles = theme.listRow;
  card.styles.selected.background = fui::Paint::dither(fui::Color::LightGray);
  card.styles.selected.border = fui::Paint::dither(fui::Color::LightGray);
  card.styles.selected.foreground = fui::Paint::solid(fui::Color::Black);
  card.styles.selected.radius = theme.listRowRadius;
  card.styles.active = card.styles.selected;
  card.titleText = theme.bodyText;
  card.titleText.maxLines = renderer.getScreenWidth() > renderer.getScreenHeight() ? 1 : 2;
  card.authorText = theme.smallText;
  card.progressText = theme.smallText;
  card.progressHeight = 6;
  card.padding = fui::Insets{6, 6, 6, 6};
  card.gap = theme.spaceLg + theme.spaceSm;
  const int maxCoverWidth = rect.width / GRID_COLUMNS - 2 * COVER_CELL_INSET;
  card.coverSize.height = std::max(1, std::min(std::min<int>(rect.height, coverRowHeight) - 12, maxCoverWidth * 3 / 2));
  card.coverSize.width = std::max(1, card.coverSize.height * 2 / 3);
  noteThumbHeight(card.coverSize.height);
  gridBounds = layoutGrid(screen.body());
  rect.x = gridBounds.x;
  rect.width = gridBounds.width;
  card.coverPainterUserData = this;
  card.coverPainter = [](fui::DrawTarget& target, fui::Rect cover, const fui::BookCardProps&, void* user) {
    return static_cast<CoverGridHomeUi*>(user)->paintFramedCover(target, cover, 0);
  };
  fui::bookCard(screen.frame(), rect, card);

  if (selected == 0 && !BoardConfig::hasTouch()) {
    const int16_t barH = card.coverSize.height;
    screen.target().fill(
        fui::Rect{static_cast<int16_t>(rect.x - 5), static_cast<int16_t>(rect.y + (rect.height - barH) / 2), 3, barH},
        fui::Paint::dither(fui::Color::LightGray));
  }
}

fui::Rect CoverGridHomeUi::layoutGrid(fui::Rect rect) {
  grid.cellInset = fui::Insets{COVER_CELL_INSET, COVER_CELL_INSET, COVER_CELL_INSET, COVER_CELL_INSET};
  grid.coverSize = card.coverSize;
  grid.rowHeight = grid.coverSize.height + 12;
  rect.height = GRID_ROWS * grid.rowHeight + (GRID_ROWS - 1) * grid.rowGap;
  return rect;
}

void CoverGridHomeUi::drawGrid(UiScreen& screen) {
  const auto rect = gridBounds;
  grid.count = books->size() > 1 ? books->size() - 1 : 0;
  grid.columns = GRID_COLUMNS;
  grid.columnLayout = fui::CoverGridColumnLayout::SpaceBetween;
  grid.action = SELECT;
  grid.inputMask = fui::InputTouch;
  grid.selectedIndex = selected > 0 && selected < static_cast<int>(books->size()) ? selected - 1 : -1;
  grid.selectionIndicator = fui::CoverGridSelectionIndicator::CoverFrame;
  grid.selectedCoverFrameGap = 6;
  grid.selectedCoverFrameWidth = 8;
  grid.cellStyles = card.styles;
  grid.labelHeight = 0;
  grid.labelGap = 0;
  grid.scrollIndicator = false;
  grid.itemProvider = [](uint16_t index, void*) { return fui::coverGridItem(nullptr, index + 1); };
  grid.coverPainterUserData = this;
  grid.coverPainter = [](fui::DrawTarget& target, fui::Rect cover, const fui::CoverGridItem&, uint16_t index,
                         void* user) {
    return static_cast<CoverGridHomeUi*>(user)->paintFramedCover(target, cover, index + 1);
  };
  fui::coverGrid(screen.frame(), rect, grid);
}

void CoverGridHomeUi::drawTabs(UiScreen& screen, fui::Rect rect) {
  static constexpr const uint8_t* ICONS[] = {FolderIcon, LibraryIcon, BlocksIcon, TransferIcon, Settings2Icon};
  int count = 0;
  for (int i = 0; i < 5; ++i) {
    if (i == 2 && !hasOpds) continue;
    auto& tab = tabItems[count];
    tab.value = books->size() + count;
    tab.selected = selected == tab.value;
    tab.label = nullptr;
    ++count;
  }
  tabs.tabs = tabItems.data();
  tabs.count = count;
  tabs.layout = fui::TabBarLayout::SpaceBetween;
  tabs.action = SELECT;
  tabs.inputMask = fui::InputTouch;
  tabs.iconSize = 32;
  tabs.iconPainterUserData = this;
  tabs.iconPainter = [](fui::DrawTarget&, fui::Rect iconRect, const fui::TabItem& tab, uint8_t, void* user) {
    const auto& self = *static_cast<CoverGridHomeUi*>(user);
    const int index = tab.value - static_cast<int>(self.books->size());
    const int icon = !self.hasOpds && index >= 2 ? index + 1 : index;
    self.renderer.drawIcon(ICONS[icon], iconRect.x, iconRect.y, iconRect.width);
    return true;
  };
  tabs.tabStyles.explicitlySet = true;
  tabs.selectedDotSize = 0;
  tabs.selectedUnderline = 2;
  tabs.distributedSlotWidth = 0;
  fui::tabBar(screen.frame(), rect, tabs);
}

bool CoverGridHomeUi::paintFramedCover(fui::DrawTarget& target, fui::Rect rect, size_t index) {
  constexpr int16_t SHADOW_OFFSET = 2;
  const auto ink = fui::Paint::solid(fui::Color::Black);
  target.fill(fui::Rect{rect.right(), static_cast<int16_t>(rect.y + SHADOW_OFFSET), SHADOW_OFFSET, rect.height}, ink);
  target.fill(fui::Rect{static_cast<int16_t>(rect.x + SHADOW_OFFSET), rect.bottom(), rect.width, SHADOW_OFFSET}, ink);
  const bool drawn = index < coverPaths.size() && coverCache.paint(rect, index, coverPaths[index]);
  return drawn;
}

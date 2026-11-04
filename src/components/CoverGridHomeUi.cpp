#include "CoverGridHomeUi.h"

#include <BoardConfig.h>
#include <GfxRenderer.h>
#include <I18n.h>

#include <algorithm>
#include <cstdio>
#include <utility>

#include "MappedInputManager.h"
#include "UITheme.h"
#include "X4ProHomeLayout.h"
#include "components/controls/button.h"
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
// Grid cell padding around each cover; also feeds the screen's horizontal
// inset so the cover columns land on the header chrome's inset line.
constexpr int16_t COVER_CELL_INSET = 6;
constexpr int COVER_ROW_EXTRA_HEIGHT = 8;
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
  refreshCoverPaths();
  progress = hasContinueReading && !books->empty() ? loadBookProgress(books->front().path) : -1;
  if (progress >= 0) snprintf(progressText, sizeof(progressText), "%d%%", progress);
#if FREEINK_DEVICE_X4PRO
  if (renderer.getScreenWidth() == 480 && renderer.getScreenHeight() == 800) {
    x4ProCompact = true;
    loadShelfProgress();
  }
#endif
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
  // One shared height for every slot (hero and grid covers are the same
  // size). The +8 surplus gives the paint clip a little bleed so the art's
  // rightward nudge never exposes the left edge; covers narrower than the
  // 2:3 slot may show a thin sliver at the sides rather than being cropped.
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

void CoverGridHomeUi::noteThumbHeights(const int heroSlotHeight, const int shelfSlotHeight) {
  bool changed = false;
  for (size_t i = 0; i < books->size() && i < thumbHeights.size(); ++i) {
    const int height = std::max(1, (i == 0 ? heroSlotHeight : shelfSlotHeight) + 8);
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
  // Sides clear the rounded corners like the header, so the grid lines up under it.
  const int corner = BoardConfig::ACTIVE.cornerRadius;
  const auto left = static_cast<int16_t>(std::max(safe.x, corner));
  const auto right = static_cast<int16_t>(std::max(renderer.getScreenWidth() - safe.x - safe.width, corner));
  screen.setContentMarginFromScreen(fui::Insets{static_cast<int16_t>(safe.y), right,
                                                static_cast<int16_t>(renderer.getScreenHeight() - safe.y - safe.height),
                                                left});
  // Horizontal inset sized so the outer cover columns (content x plus the
  // grid's cell inset) sit on the theme's side-padding line: the heading and
  // tabs then align with the clock and battery, which tuck a few px further
  // in (headerStatusInset's optical bias).
  const int16_t hInset = std::max<int16_t>(
      0, static_cast<int16_t>(UITheme::getInstance().getMetrics().headerSidePadding - COVER_CELL_INSET - left));
  // Drop the status band, hero, and grid down: touch boards get a full step
  // (no button-hint band to fit), button boards a modest nudge. The tabs stay
  // put (bottom-anchored below).
  const int16_t topInset =
      BoardConfig::hasTouch() ? static_cast<int16_t>(theme.spaceLg) : static_cast<int16_t>(theme.spaceSm + 4);
  screen.insetContent(fui::Insets{topInset, hInset, theme.spaceSm, hInset});
  const bool landscape = renderer.getScreenWidth() > renderer.getScreenHeight();
  // Reserve the band's slot in the flow (its content draws at a fixed screen
  // position in drawHeaderBand); the slot doubles as padding above the heading.
  screen.takeTop(UITheme::getInstance().getMetrics().batteryBarHeight, theme.spaceSm);
  // Button boards run tighter above the tabs to make room for the top step.
  const int16_t tabGap = BoardConfig::hasTouch() ? theme.spaceSm : static_cast<int16_t>(4);
  const auto& board = BoardConfig::ACTIVE;
  auto tabRect = screen.takeBottom(UITheme::getInstance().getMetrics().coverGridTabBarHeight, tabGap);
  // The board's tab height sits at the bottom of the theme's slot, then its
  // offset lower; the slot itself is unchanged, so the grid keeps its size.
  const auto tabHeight = static_cast<int16_t>(tabRect.height * board.tabBarHeightPct / 100);
  tabRect.y = static_cast<int16_t>(tabRect.y + tabRect.height - tabHeight + board.tabBarOffsetY);
  tabRect.height = tabHeight;
  // The board's share of the row the tabs spread across, centered.
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
  // Hero cover matches the grid covers: the body splits into three equal
  // cover rows (hero + two grid rows), so the featured section is one row
  // tall and every cover on screen shares one size. The metadata floor still
  // applies when the rows would be too short for the hero's text lines.
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

void CoverGridHomeUi::loadShelfProgress() {
  shelfProgress.fill(-1);
  for (auto& text : shelfProgressText) text.front() = '\0';
  if (!books) return;
  const int rowCount = x4ProShelfRowCount(static_cast<int>(books->size()));
  for (int row = 0; row < rowCount && row < static_cast<int>(shelfProgress.size()); ++row) {
    const int value = loadBookProgress((*books)[static_cast<size_t>(x4ProShelfBookIndex(row))].path);
    shelfProgress[row] = value;
    if (value >= 0) snprintf(shelfProgressText[row].data(), shelfProgressText[row].size(), "%d%%", value);
  }
}

#if FREEINK_DEVICE_X4PRO
void CoverGridHomeUi::drawX4ProPortrait(UiScreen& screen) {
  coverCache.prepare();
  const auto& theme = screen.theme();
  const int16_t titleLineHeight = screen.target().lineHeight(theme.titleText.font);
  const int16_t smallLineHeight = screen.target().lineHeight(theme.smallText.font);
  const char* ctaLabel = hasContinueReading ? tr(STR_CONTINUE_READING) : tr(STR_LIBRARY);
  const int16_t ctaWidth =
      static_cast<int16_t>(screen.target().measureText(theme.smallText.font, ctaLabel, theme.smallText).width + 56);
  const auto layout = makeX4ProHomeLayout(titleLineHeight, smallLineHeight, ctaWidth);
  screen.setContentMarginFromScreen(fui::Insets{layout.content.y, static_cast<int16_t>(480 - layout.content.right()),
                                                static_cast<int16_t>(800 - layout.content.bottom()), layout.content.x});

  if (books->empty()) {
    drawUtilityLinks(screen, layout);
    drawEmpty(screen);
    drawHeaderBand();
    return;
  }

  noteThumbHeights(layout.heroCover.height, layout.shelfThumbs[0].height);
  paintFramedCover(screen.target(),
                   fui::Rect{layout.heroCover.x, layout.heroCover.y, layout.heroCover.width, layout.heroCover.height},
                   0);
  button = {};
  button.action = SELECT;
  button.value = 0;
  button.inputMask = fui::InputTouch;
  button.styles = fui::plainStyles();
  button.minTouchSize = 44;
  fui::button(screen.frame(),
              fui::Rect{layout.heroCover.x, layout.heroCover.y, layout.heroCover.width, layout.heroCover.height},
              button);

  auto title = theme.titleText;
  title.align = fui::TextAlign::Left;
  title.bold = true;
  title.maxLines = 3;
  screen.target().text(
      fui::Rect{layout.heroTitle.x, layout.heroTitle.y, layout.heroTitle.width, layout.heroTitle.height},
      books->front().title.c_str(), title);

  if (!books->front().author.empty()) {
    auto author = theme.smallText;
    author.align = fui::TextAlign::Left;
    author.maxLines = 1;
    screen.target().text(
        fui::Rect{layout.heroAuthor.x, layout.heroAuthor.y, layout.heroAuthor.width, layout.heroAuthor.height},
        books->front().author.c_str(), author);
  }

  if (progress >= 0) {
    auto progressStyle = theme.smallText;
    progressStyle.align = fui::TextAlign::Left;
    progressStyle.bold = true;
    screen.target().text(fui::Rect{layout.progressLabel.x, layout.progressLabel.y, layout.progressLabel.width,
                                   layout.progressLabel.height},
                         progressText, progressStyle);
    screen.target().fill(fui::Rect{layout.progressTrack.x, layout.progressTrack.y, layout.progressTrack.width,
                                   layout.progressTrack.height},
                         fui::Paint::dither(fui::Color::LightGray));
    const int16_t fillWidth = static_cast<int16_t>(layout.progressTrack.width * progress / 100);
    if (fillWidth > 0) {
      screen.target().fill(
          fui::Rect{layout.progressTrack.x, layout.progressTrack.y, fillWidth, layout.progressTrack.height},
          fui::Paint::solid(fui::Color::Black));
    }
  }

  button = {};
  button.label = ctaLabel;
  button.action = SELECT;
  button.value = static_cast<int16_t>(x4ProHeroCtaAction(hasContinueReading, static_cast<int>(books->size())));
  button.inputMask = fui::InputTouch;
  button.state = selected == button.value ? fui::StateSelected : fui::StateNormal;
  button.text = theme.smallText;
  button.text.bold = true;
  button.styles = fui::defaultButtonStyles();
  button.styles.normal.background = fui::Paint::solid(fui::Color::Black);
  button.styles.normal.foreground = fui::Paint::solid(fui::Color::White);
  button.styles.selected = button.styles.normal;
  button.styles.selected.border = fui::Paint::solid(fui::Color::White);
  button.styles.selected.borderWidth = 1;
  button.styles.focused = button.styles.selected;
  button.styles.active = button.styles.selected;
  button.styles.disabled = button.styles.normal;
  button.radius = 22;
  button.minTouchSize = 48;
  fui::button(screen.frame(), fui::Rect{layout.cta.x, layout.cta.y, layout.cta.width, layout.cta.height}, button);

  auto shelfHeading = theme.smallText;
  shelfHeading.bold = true;
  screen.target().text(
      fui::Rect{layout.shelfHeader.x, layout.shelfHeader.y, layout.shelfHeader.width / 2, layout.shelfHeader.height},
      tr(STR_LIBRARY_TAB_RECENT), shelfHeading);

  const int shelfCount = x4ProShelfRowCount(static_cast<int>(books->size()));
  for (int row = 0; row < shelfCount; ++row) {
    const int bookIndex = x4ProShelfBookIndex(row);
    const auto& book = (*books)[static_cast<size_t>(bookIndex)];

    button = {};
    button.action = SELECT;
    button.value = static_cast<int16_t>(bookIndex);
    button.inputMask = fui::InputTouch;
    button.styles = fui::plainStyles();
    button.minTouchSize = 48;
    fui::button(screen.frame(),
                fui::Rect{layout.shelfRows[row].x, layout.shelfRows[row].y, layout.shelfRows[row].width,
                          layout.shelfRows[row].height},
                button);

    const auto& thumb = layout.shelfThumbs[row];
    if (selected == bookIndex) {
      // Same selection ring the cover grid drew: 3 px stroke 4 px outside the cover.
      constexpr int16_t FRAME_GAP = 4;
      constexpr int16_t FRAME_WIDTH = 3;
      screen.target().stroke(
          fui::Rect{static_cast<int16_t>(thumb.x - FRAME_GAP), static_cast<int16_t>(thumb.y - FRAME_GAP),
                    static_cast<int16_t>(thumb.width + 2 * FRAME_GAP),
                    static_cast<int16_t>(thumb.height + 2 * FRAME_GAP)},
          fui::Paint::solid(fui::Color::Black), FRAME_WIDTH);
    }
    paintFramedCover(screen.target(), fui::Rect{thumb.x, thumb.y, thumb.width, thumb.height},
                     static_cast<size_t>(bookIndex));

    auto rowTitle = theme.smallText;
    rowTitle.align = fui::TextAlign::Left;
    rowTitle.bold = true;
    rowTitle.maxLines = 1;
    screen.target().text(fui::Rect{layout.shelfTitles[row].x, layout.shelfTitles[row].y, layout.shelfTitles[row].width,
                                   layout.shelfTitles[row].height},
                         book.title.c_str(), rowTitle);

    if (!book.author.empty()) {
      auto rowAuthor = theme.smallText;
      rowAuthor.align = fui::TextAlign::Left;
      rowAuthor.maxLines = 1;
      screen.target().text(fui::Rect{layout.shelfAuthors[row].x, layout.shelfAuthors[row].y,
                                     layout.shelfAuthors[row].width, layout.shelfAuthors[row].height},
                           book.author.c_str(), rowAuthor);
    }

    if (shelfProgress[row] >= 0) {
      auto rowProgress = theme.smallText;
      rowProgress.align = fui::TextAlign::Right;
      rowProgress.bold = true;
      screen.target().text(fui::Rect{layout.shelfProgress[row].x, layout.shelfProgress[row].y,
                                     layout.shelfProgress[row].width, layout.shelfProgress[row].height},
                           shelfProgressText[row].data(), rowProgress);
    }
  }

  drawUtilityLinks(screen, layout);
  drawHeaderBand();
}
#endif

void CoverGridHomeUi::drawHeaderBand() {
  // The stock full-width band at the theme's topPadding, exactly like every
  // pushed screen's header: the battery/clock hold one position across the
  // whole UI, and the grid is widened to meet them (see the hInset above).
  const ThemeMetrics& metrics = UITheme::getInstance().getMetrics();
#if FREEINK_DEVICE_X4PRO
  if (renderer.getScreenWidth() == 480 && renderer.getScreenHeight() == 800) {
    GUI.drawHeader(renderer, Rect{0, 0, 480, 40}, nullptr);
    return;
  }
#endif
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
  // The featured card's selected state is a slim accent bar drawn after the
  // card (see below), not a bookCard indicator: every ring/outline treatment
  // tried here either overwhelmed the large cover or made the heading above
  // read as misaligned.
  card.state = fui::StateNormal;
  card.styles = theme.listRow;
  card.styles.selected.background = fui::Paint::dither(fui::Color::LightGray);
  // The grid thumbs' selection ring draws with this border: gray like Lyra's
  // selection box, not solid black.
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
  // Fit the shared 2:3 cover box within both the row and a grid column. No
  // gap reserve: the cell insets already keep adjacent covers apart, and the
  // SpaceBetween layout absorbs whatever slack remains.
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
    // Button boards only: a vertical accent bar left of the card, cover-height
    // and vertically centered on it, marks the featured card as the button
    // cursor without framing the cover. Touch boards tap directly and need no
    // cursor on the hero card.
    const int16_t barH = card.coverSize.height;
    screen.target().fill(
        fui::Rect{static_cast<int16_t>(rect.x - 5), static_cast<int16_t>(rect.y + (rect.height - barH) / 2), 3, barH},
        fui::Paint::dither(fui::Color::LightGray));
  }
}

fui::Rect CoverGridHomeUi::layoutGrid(fui::Rect rect) {
  // Exactly the hero's box: the three-equal-rows split in draw() already
  // guarantees it fits, and sharing the size keeps one cached thumb per book.
  grid.cellInset = fui::Insets{COVER_CELL_INSET, COVER_CELL_INSET, COVER_CELL_INSET, COVER_CELL_INSET};
  grid.coverSize = card.coverSize;
  grid.rowHeight = grid.coverSize.height + 12;
  // Full content width: the SpaceBetween column layout pins the outer covers
  // to the rect edges, so the grid reaches the chrome's inset line instead of
  // centering at its natural width.
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
  // Same thick cover ring as the featured card; the dithered Cell background
  // was easy to miss behind a dark cover.
  grid.selectionIndicator = fui::CoverGridSelectionIndicator::CoverFrame;
  // Thick dithered ring sized for the small thumbs: 6px outside the cover,
  // 2px over its edge.
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

void CoverGridHomeUi::drawUtilityLinks(UiScreen& screen, const X4ProHomeLayout& layout) {
  static constexpr StrId LINK_LABELS[] = {StrId::STR_LIBRARY, StrId::STR_BROWSE_FILES, StrId::STR_SETTINGS_TITLE};
  auto linkText = screen.theme().bodyText;
  for (size_t i = 0; i < layout.utilityLinks.size(); ++i) {
    const auto& link = layout.utilityLinks[i];
    button = {};
    button.label = I18N.get(LINK_LABELS[i]);
    button.action = SELECT;
    button.value = utilityValues[i];
    button.inputMask = fui::InputTouch;
    button.text = linkText;
    button.styles = fui::plainStyles();
    button.minTouchSize = 48;
    fui::button(screen.frame(), fui::Rect{link.x, link.y, link.width, link.height}, button);
    if (selected == utilityValues[i]) {
      // Editorial selected marker: a 2 px rule under the link.
      screen.target().fill(fui::Rect{link.x, static_cast<int16_t>(link.y + link.height - 2), link.width, 2},
                           fui::Paint::solid(fui::Color::Black));
    }
  }
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
  // No pill fill; the underline marks the selected tab.
  tabs.tabStyles.explicitlySet = true;
  tabs.selectedUnderline = 2;
  tabs.distributedSlotWidth = 0;
  fui::tabBar(screen.frame(), rect, tabs);
}

bool CoverGridHomeUi::paintFramedCover(fui::DrawTarget& target, fui::Rect rect, size_t index) {
  constexpr int16_t SHADOW_OFFSET = 2;
  const auto ink = fui::Paint::solid(fui::Color::Black);
  target.fill(fui::Rect{rect.right(), static_cast<int16_t>(rect.y + SHADOW_OFFSET), SHADOW_OFFSET, rect.height}, ink);
  target.fill(fui::Rect{static_cast<int16_t>(rect.x + SHADOW_OFFSET), rect.bottom(), rect.width, SHADOW_OFFSET}, ink);
  // The false spine draws inside the cover paint (HomeCoverCache), glued to
  // the art's left edge, so it stays aligned whatever each cover's margin is.
  const bool drawn = index < coverPaths.size() && coverCache.paint(rect, index, coverPaths[index]);
  return drawn;
}

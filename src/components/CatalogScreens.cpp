#include "CatalogScreens.h"

#include <FreeInkUIIcon.h>
#include <GfxRenderer.h>
#include <HalGPIO.h>
#include <I18n.h>

#include <algorithm>
#include <cstdio>
#include <numeric>

#include "AtelierChrome.h"
#include "HeaderBackTapTarget.h"
#include "UITheme.h"
#include "icons/headerIcons.h"

namespace fui = freeink::ui;

void catalogScreenHeader(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, const char* title,
                         const fui::BitmapRef& trailingIcon, const fui::ActionId trailingAction) {
#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    const bool hasTrailingIcon = trailingIcon && trailingAction != fui::NO_ACTION;
    // Reserve space on the right so long titles do not overlap the 16x16 search icon.
    AtelierChrome::drawHeader(renderer, title, hasTrailingIcon ? "        " : nullptr, true, "05.1");
    if (hasTrailingIcon) {
      AtelierChrome::drawSearchIcon(renderer, AtelierChrome::MARGIN_X + AtelierChrome::CONTENT_W - 22, 57);
      screen.frame().hit(fui::Rect{static_cast<int16_t>(AtelierChrome::MARGIN_X + AtelierChrome::CONTENT_W - 44), 46,
                                   44, 40},
                         trailingAction, 0, fui::InputTouch);
    }
    screen.setContentMarginFromScreen(fui::Insets{AtelierChrome::LIST_TOP_MIN_Y, 0, 16, 0});
    return;
  }
#endif
  const auto& metrics = UITheme::getInstance().getMetrics();
  const auto& theme = screen.theme();
  fui::HeaderProps header;
  header.title = title;
  header.borderEdges = fui::EdgeBottom;
  // Same battery/clock band as every GUI.drawHeader screen; the header
  // heights are unified across themes, so the buttons derive from the band.
  GUI.applyHeaderStatus(renderer, header);
  const auto frameRect = screen.frame().screen();
  const fui::Rect band = BaseTheme::headerBand(renderer,
                                               fui::Rect{frameRect.x, static_cast<int16_t>(metrics.topPadding),
                                                         frameRect.width, static_cast<int16_t>(metrics.headerHeight)},
                                               header);
  // Back button on touch boards, as on every GUI.drawHeader screen: the rect
  // goes to HeaderBackTapTarget, which MappedInputManager folds into
  // Button::Back, so each page's own Back handling applies (leaving a list,
  // cancelling a download). The action id only makes the header paint the
  // button; no screen registers it.
  if (gpio.hasTouch()) {
    static constexpr fui::ActionId PAINT_ONLY_BACK = 0xFFFF;
    header.leadingIcon = fui::bitmapFromIcon(icon_header_back_32);
    header.leadingAction = PAINT_ONLY_BACK;
    // Every state identical, like the trailing icon: no filled active box,
    // and a tap flash (never cleared, since no handler owns this action)
    // renders the same as the resting state instead of sticking.
    header.leadingStyles = fui::plainStyles(fui::Paint::solid(fui::Color::Black));
    HeaderBackTapTarget::set(band.x + 4, band.y + 4 + header.actionOffsetY, header.leadingSize, header.leadingSize);
  }
  if (trailingIcon && trailingAction != fui::NO_ACTION) {
    header.trailingIcon = trailingIcon;
    header.trailingAction = trailingAction;
  }
  header.titleText = theme.titleText;
  header.titleText.align = theme.headerTitleAlign;
  header.styles = theme.popup;
  if (header.styles.normal.border.kind == fui::PaintKind::None && theme.headerUnderline > 0) {
    header.styles.normal.border = fui::Paint::solid(fui::Color::Black);
    header.styles.normal.borderWidth = theme.headerUnderline;
  }
  header.trailingStyles = fui::plainStyles(fui::Paint::solid(fui::Color::Black));
  header.sidePadding = theme.headerSidePadding;
  header.minTouchSize = theme.minTouchSize;
  fui::header(screen.frame(), band, header);
  screen.setContentMarginFromScreen(
      fui::Insets{static_cast<int16_t>(metrics.topPadding + metrics.headerHeight + metrics.verticalSpacing), 0,
                  static_cast<int16_t>(metrics.buttonHintsHeight), 0});
}

void catalogCenteredBlock(UiAppHost::UiScreen& screen, const GfxRenderer& renderer,
                          const std::initializer_list<CatalogLine> lines) {
  const int count = static_cast<int>(lines.size());
  if (count == 0) return;
#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    AtelierChrome::fillRect(renderer, AtelierChrome::MARGIN_X, 240, 48, 1, 3);
    int y = 260;
    int idx = 0;
    for (const CatalogLine& line : lines) {
      const char* txt = line.text ? line.text : "";
      if (txt[0] == '\0') continue;
      const AtelierFont& font = (idx == 0) ? AtelierChrome::font20Light() : AtelierChrome::font13Reg();
      const AtelierTone tone = (idx == 0) ? AtelierTone::Ink : AtelierTone::Muted;
      const WrappedAtelierLines wrapped = AtelierChrome::wrapText(renderer, font, txt, AtelierChrome::CONTENT_W, 4);
      const int step = AtelierChrome::fontLineHeight(font) + 4;
      for (int li = 0; li < wrapped.count; ++li) {
        AtelierChrome::drawText(renderer, AtelierChrome::MARGIN_X, y, font, wrapped.lines[li], tone, 0,
                                AtelierChrome::CONTENT_W, 0, li + 1 == wrapped.count, wrapped.useFallback);
        y += step;
      }
      y += 10;
      ++idx;
    }
    return;
  }
#endif
  (void)renderer;
  // Long lines (sign-in hints, server errors) wrap instead of clipping.
  fui::TextStyle centered = screen.theme().bodyText;
  centered.align = fui::TextAlign::Center;
  centered.maxLines = 4;
  const int16_t gap = screen.theme().spaceMd;
  const int16_t pad = static_cast<int16_t>(UITheme::getInstance().getMetrics().contentSidePadding);
  const int16_t width = static_cast<int16_t>(screen.body().width - 2 * pad);
  const auto styleOf = [&](const CatalogLine& line) {
    fui::TextStyle style = centered;
    style.bold = line.bold;
    return style;
  };
  const auto heightOf = [&](const CatalogLine& line) {
    const int16_t h = fui::measureWrappedText(screen.target(), line.text ? line.text : "", styleOf(line), width).height;
    return std::max(h, screen.target().lineHeight(centered.font));
  };
  const int blockH = std::accumulate(lines.begin(), lines.end(), gap * (count - 1),
                                     [&](const int sum, const CatalogLine& line) { return sum + heightOf(line); });
  const fui::Rect body = screen.body();
  if (body.height > blockH) screen.spacer(static_cast<int16_t>((body.height - blockH) / 2));
  int i = 0;
  for (const CatalogLine& line : lines) {
    const fui::Rect rect = screen.takeTop(heightOf(line), ++i < count ? gap : 0).inset(fui::Insets{0, pad, 0, pad});
    screen.target().text(rect, line.text ? line.text : "", styleOf(line));
  }
}

void catalogDownloadScreen(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, const char* status,
                           const size_t progress, const size_t total, const fui::ActionId cancelAction) {
#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    AtelierChrome::drawText(renderer, AtelierChrome::MARGIN_X, 180, AtelierChrome::font10Bold(),
                            tr(STR_DOWNLOADING), AtelierTone::Muted, 1, AtelierChrome::CONTENT_W, 0, true);
    const WrappedAtelierLines titleLines =
        AtelierChrome::wrapText(renderer, AtelierChrome::font20Light(), status ? status : "", AtelierChrome::CONTENT_W, 2);
    int titleY = 206;
    const int titleStep = AtelierChrome::fontLineHeight(AtelierChrome::font20Light()) + 4;
    for (int i = 0; i < titleLines.count; ++i) {
      AtelierChrome::drawText(renderer, AtelierChrome::MARGIN_X, titleY, AtelierChrome::font20Light(),
                              titleLines.lines[i], AtelierTone::Ink, 0, AtelierChrome::CONTENT_W, 0,
                              i + 1 == titleLines.count, titleLines.useFallback);
      titleY += titleStep;
    }
    if (total > 0) {
      const int permille = static_cast<int>((static_cast<uint64_t>(progress) * 1000) / total);
      AtelierChrome::drawScrubberTrack(renderer, AtelierChrome::MARGIN_X, 286, AtelierChrome::CONTENT_W, 24,
                                       permille, false, false);
      char pctBuf[24]{};
      snprintf(pctBuf, sizeof(pctBuf), "%d%%", std::clamp(permille / 10, 0, 100));
      AtelierChrome::drawText(renderer, AtelierChrome::MARGIN_X, 314, AtelierChrome::font12Bold(), pctBuf,
                              AtelierTone::Muted, 1, AtelierChrome::CONTENT_W, 0, false);
    } else {
      char bytes[24]{};
      if (progress >= 1024 * 1024) {
        snprintf(bytes, sizeof(bytes), "%u.%u MB", static_cast<unsigned>(progress >> 20),
                 static_cast<unsigned>((progress >> 10) % 1024 * 10 / 1024));
      } else {
        snprintf(bytes, sizeof(bytes), "%u KB", static_cast<unsigned>(progress >> 10));
      }
      AtelierChrome::drawText(renderer, AtelierChrome::MARGIN_X, 286, AtelierChrome::font13Bold(), bytes,
                              AtelierTone::Ink, 0, AtelierChrome::CONTENT_W, 0, false);
    }
    if (cancelAction != fui::NO_ACTION) {
      AtelierChrome::drawCapsuleButton(renderer, AtelierChrome::MARGIN_X, 344, 164, 44, tr(STR_CANCEL),
                                       AtelierChrome::font12Bold(), false, true);
      screen.frame().hit(fui::Rect{AtelierChrome::MARGIN_X, 344, 164, 44}, cancelAction, 0, fui::InputTouch);
    }
    return;
  }
#endif
  (void)renderer;
  // Centered block: status line, item title, progress, optional cancel.
  const auto& theme = screen.theme();
  fui::TextStyle centered = theme.bodyText;
  centered.align = fui::TextAlign::Center;
  const int16_t lh = screen.target().lineHeight(centered.font);
  const int16_t gap = theme.spaceMd;
  const bool showBytes = total == 0;
  const int16_t progressH = showBytes ? lh : 16;
  const bool withCancel = cancelAction != fui::NO_ACTION;
  const int16_t btnH = withCancel ? theme.rowHeight : 0;
  // The item title gets two padded lines: long book titles wrap once and then
  // truncate, instead of running edge-to-edge in a single clipped line.
  fui::TextStyle title = centered;
  title.maxLines = 2;
  const int16_t titleH = static_cast<int16_t>(lh * 2);
  const int16_t pad = static_cast<int16_t>(UITheme::getInstance().getMetrics().contentSidePadding);
  const int16_t blockH = static_cast<int16_t>(lh + titleH + progressH + btnH + gap * (withCancel ? 3 : 2));
  const fui::Rect body = screen.body();
  if (body.height > blockH) screen.spacer(static_cast<int16_t>((body.height - blockH) / 2));

  screen.target().text(screen.takeTop(lh, gap), tr(STR_DOWNLOADING), centered);
  screen.target().text(screen.takeTop(titleH, gap).inset(fui::Insets{0, pad, 0, pad}), status, title);

  const fui::Rect progressArea = screen.takeTop(progressH, gap).inset(fui::Insets{0, 50, 0, 50});
  if (!showBytes) {
    fui::ProgressBarProps progressProps;
    progressProps.value = static_cast<int32_t>(progress);
    progressProps.max = static_cast<int32_t>(total);
    progressProps.border = fui::Paint::solid(fui::Color::Black);
    progressProps.borderWidth = 1;
    fui::progressBar(screen.frame(), progressArea, progressProps);
  } else {
    char bytes[24];
    if (progress >= 1024 * 1024) {
      snprintf(bytes, sizeof(bytes), "%u.%u MB", static_cast<unsigned>(progress >> 20),
               static_cast<unsigned>((progress >> 10) % 1024 * 10 / 1024));
    } else {
      snprintf(bytes, sizeof(bytes), "%u KB", static_cast<unsigned>(progress >> 10));
    }
    screen.target().text(progressArea, bytes, centered);
  }

  if (withCancel) {
    const fui::Rect btnArea = screen.takeTop(btnH);
    const int16_t btnW = static_cast<int16_t>(btnArea.width / 3);
    fui::ButtonProps cancel;
    cancel.label = tr(STR_CANCEL);
    cancel.action = cancelAction;
    // Themed button with an outline and the theme's control rounding, same
    // as the publication page's acquire button.
    cancel.styles = screen.theme().button;
    cancel.styles.normal.border = fui::Paint::solid(fui::Color::Black);
    cancel.styles.normal.borderWidth = 1;
    cancel.radius = screen.theme().controlRadius;
    screen.button(cancel,
                  fui::Rect{static_cast<int16_t>(btnArea.x + (btnArea.width - btnW) / 2), btnArea.y, btnW, btnH});
  }
}

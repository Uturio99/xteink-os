#include "QrDisplayActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "MappedInputManager.h"
#include "components/AtelierChrome.h"
#include "components/UITheme.h"
#include "fontIds.h"
#include "util/QrUtils.h"

void QrDisplayActivity::onEnter() {
  Activity::onEnter();
  requestUpdate();
}

void QrDisplayActivity::onExit() { Activity::onExit(); }

void QrDisplayActivity::loop() {
  int x = 0;
  int y = 0;
  if (mappedInput.wasReleased(MappedInputManager::Button::Back) ||
      mappedInput.wasReleased(MappedInputManager::Button::Confirm) || mappedInput.wasScreenTapped(x, y)) {
    finish();
    return;
  }
}

void QrDisplayActivity::render(RenderLock&&) {
#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    struct Ctx {
      QrDisplayActivity* self;
    } ctx{this};
    AtelierChrome::renderGrayscalePass(
        renderer, HalDisplay::FAST_REFRESH,
        [](void* raw) {
          auto* self = static_cast<Ctx*>(raw)->self;
          self->renderer.clearScreen();
          AtelierChrome::drawHeader(self->renderer, tr(STR_DISPLAY_QR), nullptr, true, "02.8");
          if (self->renderer.getRenderMode() == GfxRenderer::BW) {
            constexpr int qrSize = 360;
            QrUtils::drawQrCode(self->renderer, Rect((480 - qrSize) / 2, 180, qrSize, qrSize), self->textPayload);
          }
          AtelierChrome::drawFooterHint(self->renderer, tr(STR_TAP_BACK_TO_RETURN));
        },
        &ctx);
    return;
  }
#endif
  renderer.clearScreen();
  auto metrics = UITheme::getInstance().getMetrics();
  const auto pageWidth = renderer.getScreenWidth();
  const auto pageHeight = renderer.getScreenHeight();

  GUI.drawHeader(renderer, Rect{0, metrics.topPadding, pageWidth, metrics.headerHeight}, tr(STR_DISPLAY_QR), nullptr);

  const int availableWidth = pageWidth - 40;
  const int availableHeight = pageHeight - metrics.topPadding - metrics.headerHeight - metrics.verticalSpacing * 2 - 40;
  const int startY = metrics.topPadding + metrics.headerHeight + metrics.verticalSpacing;

  const Rect qrBounds(20, startY, availableWidth, availableHeight);
  QrUtils::drawQrCode(renderer, qrBounds, textPayload);

  const auto labels = mappedInput.mapLabels(tr(STR_BACK), "", "", "");
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);

  renderer.displayBuffer();
}

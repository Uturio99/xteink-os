#include "FullScreenMessageActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "components/AtelierChrome.h"
#include "fontIds.h"

void FullScreenMessageActivity::onEnter() {
  Activity::onEnter();

#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    struct Ctx {
      FullScreenMessageActivity* self;
    } ctx{this};
    AtelierChrome::renderGrayscalePass(
        renderer, refreshMode,
        [](void* raw) {
          auto* self = static_cast<Ctx*>(raw)->self;
          self->renderer.clearScreen();
          char upperNotice[48]{};
          AtelierChrome::formatUpperVi(tr(STR_ATELIER_NOTICE), upperNotice, sizeof(upperNotice));
          char eyebrow[64]{};
          snprintf(eyebrow, sizeof(eyebrow), "00.0 — %s", upperNotice);
          char upperSys[48]{};
          AtelierChrome::formatUpperVi(tr(STR_CAT_SYSTEM), upperSys, sizeof(upperSys));
          AtelierChrome::drawEditorialMessageScreen(self->renderer, eyebrow, upperSys, self->text.c_str(), nullptr,
                                                    CROSSPOINT_VERSION, "X4 PRO");
        },
        &ctx);
    return;
  }
#endif

  const auto height = renderer.getLineHeight(UI_10_FONT_ID);
  const auto top = (renderer.getScreenHeight() - height) / 2;

  renderer.clearScreen();
  renderer.drawCenteredText(UI_10_FONT_ID, top, text.c_str(), true, style);
  renderer.displayBuffer(refreshMode);
}

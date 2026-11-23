#include "BootActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "components/AtelierChrome.h"
#include "fontIds.h"
#include "images/NoveOsLogo.h"

void BootActivity::onEnter() {
  Activity::onEnter();

#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer)) {
    struct Ctx {
      BootActivity* self;
    } ctx{this};
    AtelierChrome::renderGrayscalePass(
        renderer, HalDisplay::FAST_REFRESH,
        [](void* raw) {
          auto* self = static_cast<Ctx*>(raw)->self;
          self->renderer.clearScreen();
          AtelierChrome::drawNoveOsSplashScreen(self->renderer);
        },
        &ctx);
    return;
  }
#endif

  const auto pageWidth = renderer.getScreenWidth();
  const auto pageHeight = renderer.getScreenHeight();

  renderer.clearScreen();
  renderer.drawImage(NoveOsBranding::NOVEOS_LOGO_1BPP, (pageWidth - NoveOsBranding::LOGO_WIDTH) / 2,
                     (pageHeight - NoveOsBranding::LOGO_HEIGHT) / 2, NoveOsBranding::LOGO_WIDTH,
                     NoveOsBranding::LOGO_HEIGHT);
  renderer.drawCenteredText(UI_10_FONT_ID, pageHeight / 2 + 70, NoveOsBranding::OS_NAME, true, EpdFontFamily::BOLD);
  renderer.drawCenteredText(SMALL_FONT_ID, pageHeight / 2 + 95, tr(STR_BOOTING));
  renderer.drawCenteredText(SMALL_FONT_ID, pageHeight - 30, CROSSPOINT_VERSION);
  renderer.displayBuffer();
}

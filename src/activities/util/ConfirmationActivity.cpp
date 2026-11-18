#include "ConfirmationActivity.h"

#include <I18n.h>

#include "HalDisplay.h"
#include "components/AtelierChrome.h"
#include "components/UITheme.h"

ConfirmationActivity::ConfirmationActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                                           const std::string& heading, const std::string& body,
                                           const std::string& cancelLabel, const std::string& confirmLabel)
    : Activity("Confirmation", renderer, mappedInput),
      heading(heading),
      body(body),
      cancelLabel(cancelLabel),
      confirmLabel(confirmLabel) {}

void ConfirmationActivity::onEnter() {
  Activity::onEnter();

  // Both texts live inside the dialog: the heading as its caption and the
  // subject (a book title) as the wrapping headline beneath it. No
  // pre-truncation — the dialog wraps both to its own width.
  const char* options[] = {
      cancelLabel.empty() ? I18N.get(StrId::STR_CANCEL) : cancelLabel.c_str(),
      confirmLabel.empty() ? I18N.get(StrId::STR_CONFIRM) : confirmLabel.c_str(),
  };
  confirmPopup.show(heading.c_str(), body.c_str(), options, 2, 0, [this](int idx) {
    ActivityResult res;
    res.isCancelled = (idx != 1);
    setResult(std::move(res));
    finish();
  });

  requestUpdate(true);
}

void ConfirmationActivity::render(RenderLock&& lock) {
#if FREEINK_DEVICE_X4PRO
  if (AtelierChrome::isPortrait480x800(renderer) && confirmPopup.isActive()) {
    struct Ctx {
      ConfirmationActivity* self;
    } ctx{this};
    AtelierChrome::renderGrayscalePass(
        renderer, HalDisplay::RefreshMode::FAST_REFRESH,
        [](void* raw) {
          auto* self = static_cast<Ctx*>(raw)->self;
          self->renderer.clearScreen();
          AtelierChrome::drawHeader(self->renderer, tr(STR_CONFIRM), nullptr, true, "00.0");
          self->confirmPopup.render(self->renderer);
        },
        &ctx);
    return;
  }
#endif
  renderer.clearScreen();

  if (confirmPopup.processRender(renderer, mappedInput)) return;

  renderer.displayBuffer(HalDisplay::RefreshMode::FAST_REFRESH);
}

void ConfirmationActivity::loop() {
  if (confirmPopup.handleInput(mappedInput, [this] {
        if (confirmPopup.isActive()) requestUpdate();
      }))
    return;

  // Popup dismissed without a selection (Back button or tap outside): cancel.
  ActivityResult res;
  res.isCancelled = true;
  setResult(std::move(res));
  finish();
}

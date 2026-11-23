#include "AtelierChrome.h"

#if FREEINK_DEVICE_X4PRO

#include <BidiUtils.h>
#include <BitmapHelpers.h>
#include <CrossPointSettings.h>
#include <HalClock.h>
#include <HalPowerManager.h>
#include <I18n.h>
#include <Utf8.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "AtelierFontData.h"
#include "AtelierSelection.h"
#include "HeaderBackTapTarget.h"
#include "fontIds.h"
#include "images/NoveOsLogo.h"

namespace fui = freeink::ui;

namespace AtelierChrome {
namespace {

bool sGrayscalePassActive = false;
int sPendingBottomNavTab = -1;

// Suspends the Atelier text hook for a scoped renderer call so fallback
// measure/draw see the real EpdFont metrics and never re-enter the hook.
struct ScopedTextHookSuspend {
  const GfxRenderer& r;
  const GfxRenderer::UiTextOverrideHook saved;
  explicit ScopedTextHookSuspend(const GfxRenderer& r) : r(r), saved(r.uiTextOverrideHook()) {
    r.setUiTextOverrideHook(GfxRenderer::UiTextOverrideHook{});
  }
  ~ScopedTextHookSuspend() { r.setUiTextOverrideHook(saved); }
};

const AtelierGlyph* findAtelierGlyphRaw(const AtelierFont& font, const uint32_t cp) {
  const uint16_t key = cp <= 0xFFFFu ? static_cast<uint16_t>(cp) : static_cast<uint16_t>('?');
  const AtelierGlyph* begin = font.glyphs;
  const AtelierGlyph* end = font.glyphs + font.glyphCount;
  const auto* it =
      std::lower_bound(begin, end, key, [](const AtelierGlyph& g, const uint16_t c) { return g.codepoint < c; });
  if (it != end && it->codepoint == key) return it;
  return nullptr;
}

const AtelierGlyph* findAtelierGlyph(const AtelierFont& font, const uint32_t cp) {
  if (const AtelierGlyph* direct = findAtelierGlyphRaw(font, cp)) return direct;
  if (cp == 0x2013 || cp == 0x2014) return findAtelierGlyphRaw(font, '-');
  if (cp != '?') return findAtelierGlyphRaw(font, '?');
  return nullptr;
}

bool fontCoversNormalized(const AtelierFont& font, const char* nfcText) {
  const auto* p = reinterpret_cast<const unsigned char*>(nfcText);
  while (const uint32_t cp = utf8NextCodepoint(&p)) {
    if (cp <= 0x0020u || cp == 0x2013u || cp == 0x2014u) continue;
    if (!findAtelierGlyphRaw(font, cp)) return false;
  }
  return true;
}

bool fontCoversText(const AtelierFont& font, const char* text) {
  if (!text) return true;
  char norm[256];
  normalizeUtf8Stack(text, norm, sizeof(norm));
  return fontCoversNormalized(font, norm);
}

int measureNormalized(const AtelierFont& font, const char* nfcText, const int8_t trackingPx) {
  int32_t penFp4 = 0;
  int count = 0;
  const auto* p = reinterpret_cast<const unsigned char*>(nfcText);
  while (const uint32_t cp = utf8NextCodepoint(&p)) {
    const AtelierGlyph* g = findAtelierGlyph(font, cp);
    if (!g) continue;
    if (count > 0 && trackingPx != 0) penFp4 += static_cast<int32_t>(trackingPx) << 4;
    penFp4 += g->advFp4;
    ++count;
  }
  return static_cast<int>((penFp4 + 8) >> 4);
}

// Built-in UI font that stands in for an Atelier font when a string has
// uncovered codepoints. Sized by ascender (the built-ins run much taller per
// nominal size at 150 DPI: ascender 16/20/24 for 8/10/12pt), nearest real
// ascender with ties going smaller so fallback never grows a row.
// BOLD only for the bold faces (16Med maps to REGULAR — medium is not bold).
struct FallbackFont {
  int fontId;
  EpdFontFamily::Style style;
};

FallbackFont fallbackFontFor(const AtelierFont& font) {
  const bool bold = &font == &kAtelierFont8Bold || &font == &kAtelierFont10Bold ||
                    &font == &kAtelierFont11Bold || &font == &kAtelierFont12Bold ||
                    &font == &kAtelierFont13Bold;
  const int fontId = font.ascender <= 18 ? SMALL_FONT_ID : font.ascender <= 22 ? UI_10_FONT_ID : UI_12_FONT_ID;
  return {fontId, bold ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR};
}

// Point size (at 150 DPI) of the built-in UI font id, matching the size the
// SD-font system registers UI fallbacks under (kUiFontSizes in
// SdCardFontSystem.cpp).
int fallbackPointSize(const int fontId) {
  return fontId == SMALL_FONT_ID ? 8 : fontId == UI_10_FONT_ID ? 10 : 12;
}

// Whether the resolved (primary or SD-fallback) font can draw `cp`.
// resolveTextFontId is private, so this mirrors its outcome: the built-in
// primary family, plus same-point-size SD fallback candidates in the font map
// (the paired fallback is always among them; over-approximates only when two
// different SD fallback families share a UI point size).
bool fallbackChainDraws(const GfxRenderer& renderer, const int fontId, const EpdFontFamily::Style style,
                        const uint32_t cp) {
  const auto& fontMap = renderer.getFontMap();
  const auto primIt = fontMap.find(fontId);
  if (primIt == fontMap.end()) return false;
  if (primIt->second.hasCodepoint(cp, style)) return true;
  const float px = fallbackPointSize(fontId) * FONT_RASTER_DPI / 72.0f;
  for (const auto& kv : renderer.getSdCardFonts()) {
    const auto it = fontMap.find(kv.first);
    if (it == fontMap.end()) continue;
    if (std::abs(it->second.getSizeInPixels() - px) > 1.0f) continue;
    if (it->second.hasCodepoint(cp, style)) return true;
  }
  return false;
}

// True when the renderer's fallback chain can draw at least one codepoint the
// Atelier font lacks; otherwise the caller keeps the old `?` rendering —
// mirrors the renderer's own whole-string policy in resolveTextFontId.
// RTL text is checked on its shaped (presentation-form) stream, the same
// codepoints drawText actually looks up after resolveVisualText.
bool fallbackChainCovers(const GfxRenderer& renderer, const FallbackFont& fb, const AtelierFont& font,
                         const char* nfcText) {
  const auto anyDrawable = [&](const char* s) {
    const auto* p = reinterpret_cast<const unsigned char*>(s);
    while (const uint32_t cp = utf8NextCodepoint(&p)) {
      if (cp >= 0x80u && !findAtelierGlyphRaw(font, cp) && fallbackChainDraws(renderer, fb.fontId, fb.style, cp)) {
        return true;
      }
    }
    return false;
  };
  if (anyDrawable(nfcText)) return true;
  bool hasRtl = false;
  for (const auto* s = reinterpret_cast<const unsigned char*>(nfcText); *s; ++s) {
    if (*s >= 0xD6 && *s <= 0xDB) {
      hasRtl = true;
      break;
    }
  }
  if (!hasRtl) return false;
  std::string visual;
  if (!BidiUtils::applyBidiVisual(nfcText, visual)) return false;
  return anyDrawable(visual.c_str());
}

// One font choice per string: covered text keeps the Atelier font; uncovered
// text falls through to the renderer so resolveTextFontId/SD fallbacks decide
// the same font GfxRenderer::drawText would use — but only when the chain can
// actually draw an uncovered codepoint. trackingPx is ignored on the fallback
// path so measure and draw stay consistent.
struct TextMeasure {
  const AtelierFont* font;
  const GfxRenderer* renderer;
  FallbackFont fallback;
  bool useFallback;
};

TextMeasure pickTextMeasure(const GfxRenderer& renderer, const AtelierFont& font, const char* nfcText) {
  TextMeasure m{&font, &renderer, fallbackFontFor(font), false};
  if (!fontCoversNormalized(font, nfcText) && fallbackChainCovers(renderer, m.fallback, font, nfcText)) {
    m.useFallback = true;
  }
  return m;
}

// Wrap/truncate probes: Layout-mode advance measure avoids the glyph-resident
// prewarm in getTextWidth for SD fonts with an advance table.
int measureProbeWith(const TextMeasure& m, const char* text, const int8_t trackingPx) {
  if (!m.useFallback) return measureNormalized(*m.font, text, trackingPx);
  const ScopedTextHookSuspend suspend(*m.renderer);
  return m.renderer->getTextAdvanceX(m.fallback.fontId, text, m.fallback.style, 0,
                                     BidiUtils::BidiBaseDir::AUTO, GfxRenderer::TextMeasureMode::Layout);
}

// Final width for alignment: the same code path GfxRenderer::getTextWidth
// would use inside the draw.
int measureFinalWith(const TextMeasure& m, const char* text, const int8_t trackingPx) {
  if (!m.useFallback) return measureNormalized(*m.font, text, trackingPx);
  const ScopedTextHookSuspend suspend(*m.renderer);
  return m.renderer->getTextWidth(m.fallback.fontId, text, m.fallback.style);
}

inline uint8_t shadeForTone(const AtelierTone tone, const uint8_t q) {
  switch (tone) {
    case AtelierTone::Ink:
      return q;
    case AtelierTone::Muted:
      return q >= 2 ? 2 : 1;
    case AtelierTone::PaperOnInk:
      return static_cast<uint8_t>(3 - q);
    case AtelierTone::MutedOnInk:
      return q >= 2 ? 1 : 2;
  }
  return q;
}

const AtelierFont* resolveOverrideFont(const int fontId, const EpdFontFamily::Style style) {
  const bool bold = (style & EpdFontFamily::BOLD) != 0;
  if (fontId == SMALL_FONT_ID) {
    return bold ? &kAtelierFont11Bold : &kAtelierFont11Reg;
  }
  if (fontId == UI_10_FONT_ID) {
    return bold ? &kAtelierFont12Bold : &kAtelierFont12Reg;
  }
  if (fontId == UI_12_FONT_ID) {
    return bold ? &kAtelierFont13Bold : &kAtelierFont16Med;
  }
  return nullptr;
}

int hookLineHeight(const int fontId) {
  const AtelierFont* f = resolveOverrideFont(fontId, EpdFontFamily::REGULAR);
  return f ? f->lineHeight : 0;
}

int hookAscender(const int fontId) {
  const AtelierFont* f = resolveOverrideFont(fontId, EpdFontFamily::REGULAR);
  return f ? f->ascender : 0;
}

int hookMeasureWidth(const int fontId, const char* text, const EpdFontFamily::Style style) {
  const AtelierFont* f = resolveOverrideFont(fontId, style);
  if (!f || !fontCoversText(*f, text)) return -1;
  // Text is covered, so only the Atelier path is needed (no fallback measure).
  char norm[256];
  normalizeUtf8Stack(text, norm, sizeof(norm));
  return measureNormalized(*f, norm, 0);
}

bool hookDrawText(const GfxRenderer& renderer, const int fontId, const int x, const int y, const char* text,
                  const Color color, const EpdFontFamily::Style style) {
  const AtelierFont* f = resolveOverrideFont(fontId, style);
  if (!f || !fontCoversText(*f, text)) return false;
  AtelierTone tone = AtelierTone::Ink;
  if (color == Color::White) {
    tone = AtelierTone::PaperOnInk;
  } else if (color == Color::LightGray || color == Color::DarkGray) {
    tone = AtelierTone::Muted;
  }
  drawText(renderer, x, y, *f, text, tone, 0, renderer.getScreenWidth() - x, 0, false);
  return true;
}

static constexpr char kEllipsis[] = "…";

void truncateTailWithEllipsis(const TextMeasure& m, char* buf, const size_t cap, const int8_t trackingPx,
                              const int maxWidth) {
  if (measureProbeWith(m, buf, trackingPx) <= maxWidth) return;
  constexpr size_t kEllLen = sizeof(kEllipsis) - 1;
  const int ellW = measureProbeWith(m, kEllipsis, trackingPx);
  const int budget = std::max(0, maxWidth - ellW);
  int len = static_cast<int>(std::strlen(buf));
  while (len > 0) {
    len = utf8SafeTruncateBuffer(buf, len - 1);
    buf[len] = '\0';
    if (measureProbeWith(m, buf, trackingPx) <= budget) break;
  }
  const size_t avail = cap > kEllLen ? cap - 1 - kEllLen : 0;
  const size_t copyLen = std::min(static_cast<size_t>(len), avail);
  const size_t ellCopy = std::min(kEllLen, cap - 1 - copyLen);
  memcpy(buf + copyLen, kEllipsis, ellCopy);
  buf[copyLen + ellCopy] = '\0';
}

void truncateLeadingPathEllipsis(const GfxRenderer& renderer, char* pathBuf, const size_t cap, const int maxWidth) {
  if (pathBuf[0] != '/' || measureText(renderer, kAtelierFont10Bold, pathBuf, 1) <= maxWidth) return;
  char candidate[64]{};
  constexpr size_t kEllLen = sizeof(kEllipsis) - 1;
  const auto setCandidate = [&](const char* suffix) {
    memcpy(candidate, kEllipsis, kEllLen);
    const size_t avail = sizeof(candidate) - 1 - kEllLen;
    const int safe = utf8SafeTruncateBuffer(suffix, static_cast<int>(std::min(std::strlen(suffix), avail)));
    memcpy(candidate + kEllLen, suffix, static_cast<size_t>(safe));
    candidate[kEllLen + static_cast<size_t>(safe)] = '\0';
  };
  const char* p = pathBuf;
  bool fitted = false;
  while (const char* slash = std::strchr(p + 1, '/')) {
    setCandidate(slash);
    if (measureText(renderer, kAtelierFont10Bold, candidate, 1) <= maxWidth) {
      fitted = true;
      break;
    }
    p = slash;
  }
  if (!fitted) {
    const char* tail = p;
    while (*tail != '\0') {
      setCandidate(tail);
      if (measureText(renderer, kAtelierFont10Bold, candidate, 1) <= maxWidth) break;
      ++tail;
      while ((*tail & 0xC0u) == 0x80u) ++tail;
    }
  }
  const size_t n = std::min(cap - 1, std::strlen(candidate));
  memcpy(pathBuf, candidate, n);
  pathBuf[n] = '\0';
}

}  // namespace

const AtelierFont& font8Bold() { return kAtelierFont8Bold; }
const AtelierFont& font10Bold() { return kAtelierFont10Bold; }
const AtelierFont& font11Reg() { return kAtelierFont11Reg; }
const AtelierFont& font12Reg() { return kAtelierFont12Reg; }
const AtelierFont& font12Bold() { return kAtelierFont12Bold; }
const AtelierFont& font13Reg() { return kAtelierFont13Reg; }
const AtelierFont& font13Bold() { return kAtelierFont13Bold; }
const AtelierFont& font14Reg() { return kAtelierFont14Reg; }
const AtelierFont& font15Light() { return kAtelierFont15Light; }
const AtelierFont& font16Med() { return kAtelierFont16Med; }
const AtelierFont& font17Serif() { return kAtelierFont17Serif; }
const AtelierFont& font20Light() { return kAtelierFont20Light; }
const AtelierFont& font27Light() { return kAtelierFont27Light; }
const AtelierFont& font34Light() { return kAtelierFont34Light; }
int fontLineHeight(const AtelierFont& font) { return font.lineHeight; }

void normalizeUtf8Stack(const char* src, char* dst, const size_t cap) {
  if (!src || cap == 0) {
    if (cap > 0) dst[0] = '\0';
    return;
  }
  const int safeLen = utf8SafeTruncateBuffer(src, static_cast<int>(std::min(std::strlen(src), cap - 1)));
  memcpy(dst, src, static_cast<size_t>(safeLen));
  dst[safeLen] = '\0';
  utf8ComposeNfcInPlace(dst);
}

uint32_t toUpperViCodepoint(const uint32_t cp) {
  if (cp >= 'a' && cp <= 'z') return cp - 32;
  if ((cp >= 0x00E0 && cp <= 0x00F6) || (cp >= 0x00F8 && cp <= 0x00FE)) return cp - 32;
  if (cp == 0x0111 || cp == 0x01A1 || cp == 0x01B0) return cp - 1;
  if (cp >= 0x0100 && cp <= 0x017F && (cp & 1u) != 0) return cp - 1;
  if (cp >= 0x1EA0 && cp <= 0x1EF9 && (cp & 1u) != 0) return cp - 1;
  return cp;
}

void formatUpperVi(const char* src, char* dst, const size_t cap) {
  if (cap == 0) return;
  dst[0] = '\0';
  if (!src || src[0] == '\0') return;
  char norm[256];
  normalizeUtf8Stack(src, norm, sizeof(norm));
  size_t pos = 0;
  const auto* p = reinterpret_cast<const unsigned char*>(norm);
  while (const uint32_t cp = utf8NextCodepoint(&p)) {
    char cpBuf[4];
    const size_t n = static_cast<size_t>(utf8AppendCodepoint(cpBuf, toUpperViCodepoint(cp)) - cpBuf);
    if (pos + n >= cap) break;
    memcpy(dst + pos, cpBuf, n);
    pos += n;
  }
  dst[pos] = '\0';
}

void formatUpperAuthor(const std::string& rawAuthor, char* dst, const size_t cap) {
  if (cap == 0) return;
  dst[0] = '\0';
  if (rawAuthor.empty()) return;
  std::string trimmed = rawAuthor;
  const size_t sep = trimmed.find(" · ");
  if (sep != std::string::npos) trimmed.resize(sep);
  formatUpperVi(trimmed.c_str(), dst, cap);
}

int measureText(const GfxRenderer& renderer, const AtelierFont& font, const char* text, const int8_t trackingPx) {
  char buf[256];
  normalizeUtf8Stack(text, buf, sizeof(buf));
  return measureFinalWith(pickTextMeasure(renderer, font, buf), buf, trackingPx);
}

void drawText(const GfxRenderer& renderer, const int x, const int yTop, const AtelierFont& font, const char* text,
              const AtelierTone tone, const int8_t trackingPx, const int maxWidth, const int align,
              const bool ellipsisIfOverflow, const bool forceFallback) {
  if (!text || text[0] == '\0' || maxWidth <= 0) return;
  char buf[256];
  normalizeUtf8Stack(text, buf, sizeof(buf));

  TextMeasure m = pickTextMeasure(renderer, font, buf);
  if (forceFallback) m.useFallback = true;
  if (ellipsisIfOverflow) truncateTailWithEllipsis(m, buf, sizeof(buf), trackingPx, maxWidth);

  const int totalW = measureFinalWith(m, buf, trackingPx);
  int startX = x;
  if (align == 1) {
    startX = x + (maxWidth - totalW) / 2;
  } else if (align == 2) {
    startX = x + maxWidth - totalW;
  }

  if (m.useFallback) {
    const ScopedTextHookSuspend suspend(renderer);
    // Center the real EpdFont line box inside the Atelier line box; hook-free
    // metrics keep the renderer's own SD-redirect centering correct too.
    const int fbY = yTop + (font.lineHeight - renderer.getLineHeight(m.fallback.fontId)) / 2;
    renderer.drawText(m.fallback.fontId, startX, fbY, buf, tone != AtelierTone::PaperOnInk, m.fallback.style);
    return;
  }

  const int baselineY = yTop + font.ascender;
  int32_t penFp4 = 0;
  int count = 0;
  const auto* p = reinterpret_cast<const unsigned char*>(buf);
  while (const uint32_t cp = utf8NextCodepoint(&p)) {
    const AtelierGlyph* g = findAtelierGlyph(font, cp);
    if (!g) continue;
    if (count > 0 && trackingPx != 0) penFp4 += static_cast<int32_t>(trackingPx) << 4;
    const int gx0 = startX + static_cast<int>((penFp4 + 8) >> 4) + g->left;
    const int gy0 = baselineY - g->top;
    const uint8_t* bmp = font.bitmaps + g->offset;
    for (int gy = 0; gy < g->height; ++gy) {
      for (int gx = 0; gx < g->width; ++gx) {
        const int idx = gy * g->width + gx;
        const uint8_t q = (bmp[idx >> 2] >> ((3 - (idx & 3)) * 2)) & 0x3u;
        if (q == 0) continue;
        paintPixel(renderer, gx0 + gx, gy0 + gy, shadeForTone(tone, q));
      }
    }
    penFp4 += g->advFp4;
    ++count;
  }
}

WrappedAtelierLines wrapText(const GfxRenderer& renderer, const AtelierFont& font, const char* text,
                             const int maxWidth, const int maxLines, const bool ellipsisIfTruncated) {
  if (!text || text[0] == '\0' || maxLines <= 0) return {};
  char norm[256];
  normalizeUtf8Stack(text, norm, sizeof(norm));
  const TextMeasure m = pickTextMeasure(renderer, font, norm);
  const auto measure = [](const char* line, const void* context) {
    return measureProbeWith(*static_cast<const TextMeasure*>(context), line, 0);
  };
  auto res = wrapAtelierLines(measure, &m, norm, maxWidth, maxLines);
  res.useFallback = m.useFallback;
  if (ellipsisIfTruncated) {
    const auto measureTail = [](const char* line, const void* context) {
      const auto& metrics = *static_cast<const TextMeasure*>(context);
      // A Latin prefix must not switch away from the original fallback font.
      return metrics.useFallback ? measureProbeWith(metrics, line, 0)
                                 : measureText(*metrics.renderer, *metrics.font, line, 0);
    };
    ellipsizeAtelierTail(measureTail, &m, res, maxWidth);
  }
  return res;
}

void paintPixel(const GfxRenderer& renderer, const int x, const int y, const uint8_t shade) {
  if (x < 0 || y < 0 || x >= renderer.getScreenWidth() || y >= renderer.getScreenHeight()) return;
  const auto mode = renderer.getRenderMode();
  if (renderer.grayPlanesAreAbsolute()) {
    const auto px = grayPlanePixel(static_cast<uint8_t>(3 - shade), mode == GfxRenderer::GRAYSCALE_MSB, true);
    if (px.write) renderer.drawPixel(x, y, px.black);
    return;
  }
  renderer.drawPixel(
      x, y, grayPlaneBitState(shade, mode == GfxRenderer::BW, mode == GfxRenderer::GRAYSCALE_LSB, !sGrayscalePassActive));
}

void fillRect(const GfxRenderer& renderer, const int x, const int y, const int w, const int h, const uint8_t shade) {
  if (w <= 0 || h <= 0) return;
  if (renderer.grayPlanesAreAbsolute()) {
    for (int py = y; py < y + h; ++py) {
      for (int px = x; px < x + w; ++px) paintPixel(renderer, px, py, shade);
    }
    return;
  }
  const auto mode = renderer.getRenderMode();
  renderer.fillRect(x, y, w, h,
                    grayPlaneBitState(shade, mode == GfxRenderer::BW, mode == GfxRenderer::GRAYSCALE_LSB));
}

void drawLineAA(const GfxRenderer& renderer, const float x0, const float y0, const float x1, const float y1,
                const float halfWidth, const uint8_t fgShade, const uint8_t bgShade) {
  const int minX = static_cast<int>(std::floor(std::min(x0, x1) - halfWidth - 1.5f));
  const int maxX = static_cast<int>(std::ceil(std::max(x0, x1) + halfWidth + 1.5f));
  const int minY = static_cast<int>(std::floor(std::min(y0, y1) - halfWidth - 1.5f));
  const int maxY = static_cast<int>(std::ceil(std::max(y0, y1) + halfWidth + 1.5f));
  const float vx = x1 - x0;
  const float vy = y1 - y0;
  const float len2 = vx * vx + vy * vy;
  for (int py = minY; py <= maxY; ++py) {
    for (int px = minX; px <= maxX; ++px) {
      const float cx = px + 0.5f;
      const float cy = py + 0.5f;
      const float t = len2 > 1e-4f ? std::clamp(((cx - x0) * vx + (cy - y0) * vy) / len2, 0.0f, 1.0f) : 0.0f;
      const float dist = std::hypotf(cx - (x0 + t * vx), cy - (y0 + t * vy));
      const float cov = std::clamp(halfWidth + 0.5f - dist, 0.0f, 1.0f);
      if (cov <= 0.14f) continue;
      const float blended = bgShade + cov * (static_cast<float>(fgShade) - bgShade);
      paintPixel(renderer, px, py, static_cast<uint8_t>(std::clamp<int>(std::lround(blended), 0, 3)));
    }
  }
}

void fillCapsule(const GfxRenderer& renderer, const int x, const int y, const int w, const int h) {
  const float r = h * 0.5f;
  const float cy = y + r;
  const float cxL = std::min(x + w * 0.5f, x + r);
  const float cxR = std::max(x + w * 0.5f, x + w - r);
  for (int py = y - 1; py <= y + h; ++py) {
    for (int px = x - 1; px <= x + w; ++px) {
      const float sx = px + 0.5f;
      const float sy = py + 0.5f;
      const float qx = std::clamp(sx, cxL, cxR);
      const float d = std::hypotf(sx - qx, sy - cy) - r;
      const float cov = std::clamp(0.5f - d, 0.0f, 1.0f);
      if (cov >= 0.84f) {
        paintPixel(renderer, px, py, 3);
      } else if (cov >= 0.48f) {
        paintPixel(renderer, px, py, 2);
      } else if (cov >= 0.16f) {
        paintPixel(renderer, px, py, 1);
      }
    }
  }
}

void strokeCapsule(const GfxRenderer& renderer, const int x, const int y, const int w, const int h,
                   const uint8_t maxShade, const float thickness) {
  const float r = h * 0.5f - thickness * 0.5f;
  const float cy = y + h * 0.5f;
  const float cxL = std::min(x + w * 0.5f, x + h * 0.5f);
  const float cxR = std::max(x + w * 0.5f, x + w - h * 0.5f);
  for (int py = y - 1; py <= y + h; ++py) {
    for (int px = x - 1; px <= x + w; ++px) {
      const float sx = px + 0.5f;
      const float sy = py + 0.5f;
      const float qx = std::clamp(sx, cxL, cxR);
      const float ring = std::fabs(std::hypotf(sx - qx, sy - cy) - r) - thickness * 0.5f;
      const float cov = std::clamp(0.5f - ring, 0.0f, 1.0f);
      const auto shade = static_cast<uint8_t>(std::lround(cov * maxShade));
      if (shade > 0) paintPixel(renderer, px, py, shade);
    }
  }
}

void drawCapsuleButton(const GfxRenderer& renderer, const int x, const int y, const int w, const int h, const char* label,
                       const AtelierFont& font, const bool filled, const bool drawOutline, const bool outerFocusRing,
                       const bool ellipsis) {
  if (filled) {
    fillCapsule(renderer, x, y, w, h);
    if (outerFocusRing) strokeCapsule(renderer, x - 3, y - 3, w + 6, h + 6, 3, 1.0f);
  } else if (drawOutline) {
    strokeCapsule(renderer, x, y, w, h, 3, outerFocusRing ? 1.5f : 1.0f);
  }
  drawText(renderer, x, y + (h - font.lineHeight) / 2, font, label ? label : "",
           filled ? AtelierTone::PaperOnInk : AtelierTone::Ink, 0, w, 1, ellipsis);
}

void fillCircle(const GfxRenderer& renderer, const float cx, const float cy, const float radius, const uint8_t shade,
                const uint8_t bgShade) {
  const int minX = static_cast<int>(std::floor(cx - radius - 1));
  const int maxX = static_cast<int>(std::ceil(cx + radius + 1));
  const int minY = static_cast<int>(std::floor(cy - radius - 1));
  const int maxY = static_cast<int>(std::ceil(cy + radius + 1));
  for (int py = minY; py <= maxY; ++py) {
    for (int px = minX; px <= maxX; ++px) {
      const float d = std::hypotf((px + 0.5f) - cx, (py + 0.5f) - cy) - radius;
      const float cov = std::clamp(0.5f - d, 0.0f, 1.0f);
      if (bgShade != 0 && cov <= 0.14f) continue;
      const uint8_t s = static_cast<uint8_t>(
          std::clamp<int>(std::lround(bgShade + cov * (static_cast<float>(shade) - bgShade)), 0, 3));
      if (bgShade == 0 && s == 0) continue;
      paintPixel(renderer, px, py, s);
    }
  }
}

void drawWifiGlyph(const GfxRenderer& renderer, const int x, const int y) {
  const float cx = x + 7.0f;
  const float cy = y + 9.2f;
  for (int py = y - 1; py <= y + 11; ++py) {
    for (int px = x - 1; px <= x + 14; ++px) {
      const float sx = px + 0.5f;
      const float sy = py + 0.5f;
      const float dx = sx - cx;
      const float dy = sy - cy;
      if (dy > 0.8f) continue;
      const float dist = std::hypotf(dx, dy);
      if (std::fabs(dx) > (-dy) * 1.15f) continue;
      float d = 99.0f;
      if (dist <= 1.6f) {
        d = dist - 0.9f;
      } else {
        d = std::min({std::fabs(dist - 4.0f), std::fabs(dist - 6.6f), std::fabs(dist - 9.2f)}) - 0.55f;
      }
      const float cov = std::clamp(0.5f - d, 0.0f, 1.0f);
      if (cov >= 0.45f) {
        paintPixel(renderer, px, py, 2);
      } else if (cov >= 0.16f) {
        paintPixel(renderer, px, py, 1);
      }
    }
  }
}

void drawTocIcon(const GfxRenderer& renderer, const int x, const int y) {
  drawLineAA(renderer, x + 3.0f, y + 4.5f, x + 13.0f, y + 4.5f, 0.7f, 3);
  drawLineAA(renderer, x + 3.0f, y + 8.0f, x + 13.0f, y + 8.0f, 0.7f, 3);
  drawLineAA(renderer, x + 3.0f, y + 11.5f, x + 9.0f, y + 11.5f, 0.7f, 3);
}

void drawSlidersIcon(const GfxRenderer& renderer, const int x, const int y) {
  drawLineAA(renderer, x + 3.0f, y + 5.0f, x + 13.0f, y + 5.0f, 0.7f, 3);
  drawLineAA(renderer, x + 6.0f, y + 3.0f, x + 6.0f, y + 7.0f, 0.7f, 3);
  drawLineAA(renderer, x + 3.0f, y + 11.0f, x + 13.0f, y + 11.0f, 0.7f, 3);
  drawLineAA(renderer, x + 10.0f, y + 9.0f, x + 10.0f, y + 13.0f, 0.7f, 3);
}

void drawSearchIcon(const GfxRenderer& renderer, const int x, const int y) {
  // Minimalist 16x16 magnifying glass
  const float cx = x + 7.0f;
  const float cy = y + 7.0f;
  const float r = 4.2f;
  for (int py = y + 1; py <= y + 13; ++py) {
    for (int px = x + 1; px <= x + 13; ++px) {
      const float ring = std::fabs(std::hypotf((px + 0.5f) - cx, (py + 0.5f) - cy) - r) - 0.65f;
      const float cov = std::clamp(0.5f - ring, 0.0f, 1.0f);
      const auto s = static_cast<uint8_t>(std::lround(cov * 3));
      if (s > 0) paintPixel(renderer, px, py, s);
    }
  }
  drawLineAA(renderer, x + 10.1f, y + 10.1f, x + 13.5f, y + 13.5f, 0.7f, 3);
}

void drawRefreshIcon(const GfxRenderer& renderer, const int x, const int y) {
  // Minimalist 16x16 circular arrow
  const float cx = x + 8.0f;
  const float cy = y + 8.0f;
  const float r = 4.5f;
  for (int py = y + 1; py <= y + 14; ++py) {
    for (int px = x + 1; px <= x + 14; ++px) {
      const float dx = (px + 0.5f) - cx;
      const float dy = (py + 0.5f) - cy;
      if (dx > 0.8f && dy < -0.5f && dy > -3.5f) continue;  // clean arc gap
      const float ring = std::fabs(std::hypotf(dx, dy) - r) - 0.6f;
      const float cov = std::clamp(0.5f - ring, 0.0f, 1.0f);
      const auto s = static_cast<uint8_t>(std::lround(cov * 3));
      if (s > 0) paintPixel(renderer, px, py, s);
    }
  }
  drawLineAA(renderer, x + 9.5f, y + 3.0f, x + 12.2f, y + 4.8f, 0.65f, 3);
  drawLineAA(renderer, x + 12.2f, y + 4.8f, x + 9.2f, y + 6.4f, 0.65f, 3);
}

void drawRightArrow(const GfxRenderer& renderer, const int x, const int y, const uint8_t fgShade,
                    const uint8_t bgShade) {
  drawLineAA(renderer, x + 3.0f, y + 8.0f, x + 13.0f, y + 8.0f, 0.75f, fgShade, bgShade);
  drawLineAA(renderer, x + 9.0f, y + 4.0f, x + 13.0f, y + 8.0f, 0.75f, fgShade, bgShade);
  drawLineAA(renderer, x + 9.0f, y + 12.0f, x + 13.0f, y + 8.0f, 0.75f, fgShade, bgShade);
}

void drawChevron(const GfxRenderer& renderer, const float tipX, const float midY, const float dx, const float dy,
                 const float halfWidth, const uint8_t shade) {
  drawLineAA(renderer, tipX + dx, midY - dy, tipX, midY, halfWidth, shade);
  drawLineAA(renderer, tipX, midY, tipX + dx, midY + dy, halfWidth, shade);
}

void drawChevronRight(const GfxRenderer& renderer, const int x, const int y) {
  drawChevron(renderer, x + 9.5f, y + 7.5f, -4.5f, 4.5f, 0.65f, 3);
}

void drawToggleSwitch(const GfxRenderer& renderer, const int x, const int y, const bool checked) {
  constexpr int W = 38;
  constexpr int H = 22;
  if (checked) {
    fillCapsule(renderer, x, y, W, H);
    fillCircle(renderer, x + W - H * 0.5f, y + H * 0.5f, 7.2f, 0, 3);
  } else {
    strokeCapsule(renderer, x, y, W, H, 1, 1.2f);
    fillCircle(renderer, x + H * 0.5f, y + H * 0.5f, 6.8f, 2);
  }
}

void AtelierFocusRefreshTracker::render(GfxRenderer& renderer, const bool focusEligible, const int fullKey,
                                        const bool cleanOnForceFull, void (*paintPass)(void*), void* ctx) {
  const bool focusOnly = hasFullGrayscaleFrame && pendingFocusOnly && lastFullKey == fullKey && focusEligible;
  const auto plan = planAtelierFocusRefresh(focusMovesSinceClean, !focusOnly, cleanOnForceFull);
  pendingFocusOnly = false;
  renderGrayscalePass(renderer, plan.useHalfRefresh ? HalDisplay::HALF_REFRESH : HalDisplay::FAST_REFRESH, paintPass,
                      ctx, plan.bwFastOnly);
  hasFullGrayscaleFrame = true;
  focusMovesSinceClean = plan.nextCounter;
  lastFullKey = fullKey;
}

GfxRenderer::UiTextOverrideHook textHook(const bool ditherAsGrayscalePlanes) {
  GfxRenderer::UiTextOverrideHook hook{};
  hook.active = true;
  hook.ditherAsGrayscalePlanes = ditherAsGrayscalePlanes;
  hook.lineHeight = &hookLineHeight;
  hook.ascender = &hookAscender;
  hook.measureWidth = &hookMeasureWidth;
  hook.drawText = &hookDrawText;
  return hook;
}

void renderGrayscalePass(GfxRenderer& renderer, const HalDisplay::RefreshMode refresh, void (*paintPass)(void*),
                         void* ctx, const bool bwFastOnly) {
  if (!paintPass) return;
  const auto prevHook = renderer.uiTextOverrideHook();
  const bool prevActive = sGrayscalePassActive;
  if (bwFastOnly) {
    sGrayscalePassActive = false;
    renderer.setUiTextOverrideHook(textHook(false));
    renderer.setRenderMode(GfxRenderer::BW);
    paintPass(ctx);
    renderer.displayBuffer(HalDisplay::FAST_REFRESH);
    renderer.setUiTextOverrideHook(prevHook);
    sGrayscalePassActive = prevActive;
    return;
  }
  sGrayscalePassActive = true;
  renderer.setUiTextOverrideHook(textHook(true));

  renderer.setRenderMode(GfxRenderer::BW);
  paintPass(ctx);
  renderer.displayGrayscaleBase(refresh);

  if (renderer.storeBwBuffer()) {
    renderer.clearScreen(0x00);
    renderer.setRenderMode(GfxRenderer::GRAYSCALE_LSB);
    paintPass(ctx);
    renderer.copyGrayscaleLsbBuffers();

    renderer.clearScreen(0x00);
    renderer.setRenderMode(GfxRenderer::GRAYSCALE_MSB);
    paintPass(ctx);
    renderer.copyGrayscaleMsbBuffers();

    renderer.displayGrayBuffer();
    renderer.setRenderMode(GfxRenderer::BW);
    renderer.restoreBwBuffer();
  }

  renderer.setUiTextOverrideHook(prevHook);
  sGrayscalePassActive = prevActive;
}

void drawStatusBar(const GfxRenderer& renderer, const int x, const int y, const int w, const int h,
                   const bool backButton) {
  char clockText[16] = "09:00";
  if (halClock.isAvailable()) {
    char liveClock[12]{};
    if (halClock.formatTime(liveClock, sizeof(liveClock), SETTINGS.clockFormat == 1)) {
      snprintf(clockText, sizeof(clockText), "%s", liveClock);
    }
  }
  drawText(renderer, x, y + (h - kAtelierFont12Bold.lineHeight) / 2, kAtelierFont12Bold, clockText, AtelierTone::Ink, 0,
           90, 0, false);

  char rightLabel[32]{};
  formatUpperVi(backButton ? tr(STR_ATELIER_BACK) : tr(STR_ATELIER_MENU), rightLabel, sizeof(rightLabel));
  const int rightW = measureText(renderer, kAtelierFont10Bold, rightLabel, 1);
  const int rightX = x + w - rightW;
  drawText(renderer, rightX, y + (h - kAtelierFont10Bold.lineHeight) / 2, kAtelierFont10Bold, rightLabel,
           AtelierTone::Ink, 1, rightW + 4, 0, false);

  int rightBlockLeft = rightX;
  if (backButton) {
    const float axR = static_cast<float>(rightX - 5);
    const float ayM = y + h * 0.5f;
    drawLineAA(renderer, axR - 9.0f, ayM, axR, ayM, 0.7f, 3);
    drawLineAA(renderer, axR - 9.0f, ayM, axR - 5.5f, ayM - 3.5f, 0.7f, 3);
    drawLineAA(renderer, axR - 9.0f, ayM, axR - 5.5f, ayM + 3.5f, 0.7f, 3);
    rightBlockLeft = rightX - 16;
    HeaderBackTapTarget::set(rightBlockLeft - 14, 6, (x + w) - (rightBlockLeft - 14), 46);
  } else {
    HeaderBackTapTarget::clear();
  }

  const uint16_t pct = std::min<uint16_t>(100, powerManager.getBatteryPercentage());
  char pctText[12]{};
  snprintf(pctText, sizeof(pctText), "%u%%", static_cast<unsigned>(pct));
  const int pctW = measureText(renderer, kAtelierFont11Reg, pctText, 0);
  const int pctX = rightBlockLeft - 12 - pctW;
  drawText(renderer, pctX, y + (h - kAtelierFont11Reg.lineHeight) / 2, kAtelierFont11Reg, pctText, AtelierTone::Muted, 0,
           pctW + 4, 0, false);

  drawWifiGlyph(renderer, pctX - 10 - 14, y + (h - 11) / 2);
}

void drawHeader(const GfxRenderer& renderer, const char* title, const char* rightMeta, const bool backButton,
                const char* eyebrowCode, const int countFallback) {
  drawStatusBar(renderer, MARGIN_X, 20, CONTENT_W, 28, backButton);

  char leftBuf[192]{};
  if (title && title[0] != '\0') {
    char upperTitle[160]{};
    formatUpperVi(title, upperTitle, sizeof(upperTitle));
    if (eyebrowCode && eyebrowCode[0] != '\0') {
      snprintf(leftBuf, sizeof(leftBuf), "%s — %s", eyebrowCode, upperTitle);
    } else {
      snprintf(leftBuf, sizeof(leftBuf), "%s", upperTitle);
    }
  } else if (eyebrowCode && eyebrowCode[0] != '\0') {
    snprintf(leftBuf, sizeof(leftBuf), "%s", eyebrowCode);
  }

  char rightBuf[64]{};
  if (rightMeta && rightMeta[0] != '\0') {
    if (rightMeta[0] == '/') {
      snprintf(rightBuf, sizeof(rightBuf), "%s", rightMeta);
    } else {
      char upperMeta[64]{};
      formatUpperVi(rightMeta, upperMeta, sizeof(upperMeta));
      snprintf(rightBuf, sizeof(rightBuf), "%s", upperMeta);
    }
  } else if (countFallback >= 0) {
    snprintf(rightBuf, sizeof(rightBuf), tr(STR_ATELIER_COUNT_ITEMS_FMT), countFallback);
  }

  static constexpr int kMaxRightW = 180;
  static constexpr int kMinHeaderGap = 16;
  truncateLeadingPathEllipsis(renderer, rightBuf, sizeof(rightBuf), kMaxRightW);
  const int rawRightW = rightBuf[0] != '\0' ? measureText(renderer, kAtelierFont10Bold, rightBuf, 1) : 0;
  const int rightW = std::min(kMaxRightW, rawRightW);
  if (rightW > 0) {
    drawText(renderer, MARGIN_X + CONTENT_W - rightW, 60, kAtelierFont10Bold, rightBuf, AtelierTone::Muted, 1, rightW,
             2, true);
  }
  if (leftBuf[0] != '\0') {
    const int maxLeftW = rightW > 0 ? std::max(0, CONTENT_W - rightW - kMinHeaderGap) : CONTENT_W;
    drawText(renderer, MARGIN_X, 60, kAtelierFont10Bold, leftBuf, AtelierTone::Ink, 1, maxLeftW, 0, true);
  }
  fillRect(renderer, MARGIN_X, HEADER_RULE_Y, CONTENT_W, 1, 3);
}

void drawTabBar(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, const fui::TabItem* tabs,
                const uint16_t count, const fui::ActionId action) {
  if (!tabs || count == 0) return;
  // Region is 44 px for touch; the band is drawn in its top 38 px.
  constexpr int16_t kTabHitH = 44;
  const fui::Rect taken = screen.takeTop(kTabHitH);
  const int16_t barY = std::max<int16_t>(taken.y, HEADER_RULE_Y + 2);
  const int16_t barH = 38;
  fillRect(renderer, MARGIN_X, barY + barH - 1, CONTENT_W, 1, 1);

  const int slotW = CONTENT_W / count;
  for (uint16_t i = 0; i < count; ++i) {
    const int tabX = MARGIN_X + i * slotW;
    const char* raw = tabs[i].label ? tabs[i].label : "";
    const AtelierFont& font = tabs[i].selected ? kAtelierFont12Bold : kAtelierFont12Reg;
    const AtelierTone tone = tabs[i].selected ? AtelierTone::Ink : AtelierTone::Muted;
    const bool hasArrow = tabs[i].indicator == fui::TabIndicator::Down || tabs[i].indicator == fui::TabIndicator::Up;
    const int labelW = measureText(renderer, font, raw, 0);
    const int arrowReserve = hasArrow ? 12 : 0;
    const int totalW = std::min(slotW - 12, labelW + arrowReserve);
    const int drawLabelW = std::min(labelW, std::max(10, slotW - 12 - arrowReserve));
    const int startX = tabX + (slotW - totalW) / 2;
    const int textY = barY + (barH - font.lineHeight) / 2 - 1;
    drawText(renderer, startX, textY, font, raw, tone, 0, drawLabelW, 0, true);

    if (hasArrow) {
      const uint8_t shade = tabs[i].selected ? 3 : 2;
      const float ax = static_cast<float>(startX + drawLabelW + 7);
      const float ayTop = barY + barH * 0.5f - 4.5f;
      const float ayBot = barY + barH * 0.5f + 3.5f;
      drawLineAA(renderer, ax, ayTop, ax, ayBot, 0.65f, shade);
      if (tabs[i].indicator == fui::TabIndicator::Down) {
        drawLineAA(renderer, ax - 3.0f, ayBot - 3.0f, ax, ayBot, 0.65f, shade);
        drawLineAA(renderer, ax + 3.0f, ayBot - 3.0f, ax, ayBot, 0.65f, shade);
      } else {
        drawLineAA(renderer, ax - 3.0f, ayTop + 3.0f, ax, ayTop, 0.65f, shade);
        drawLineAA(renderer, ax + 3.0f, ayTop + 3.0f, ax, ayTop, 0.65f, shade);
      }
    }

    if (tabs[i].selected) {
      const int uw = std::min(slotW - 12, totalW + 12);
      const int ux = tabX + (slotW - uw) / 2;
      fillRect(renderer, ux, barY + barH - 2, uw, 2, 3);
    }

    screen.frame().hit(fui::Rect{static_cast<int16_t>(tabX), barY, static_cast<int16_t>(slotW), kTabHitH}, action,
                       tabs[i].value, fui::InputTouch, fui::StateNormal);
  }
}

void drawEditorialList(UiAppHost::UiScreen& screen, const GfxRenderer& renderer, fui::ListProps& props,
                       fui::ListNav& nav, const bool hasBottomNav) {
  const fui::Rect body = screen.body();
  const int listTop = std::max<int>(body.y, LIST_TOP_MIN_Y);
  const int maxBottom = hasBottomNav ? LIST_BOTTOM_MAX_Y : 784;
  const int listBottom = std::min<int>(body.bottom(), maxBottom);
  if (props.count == 0 || listBottom <= listTop) {
    nav.drawnCount = props.count;
    nav.onListRendered(0, 0, false);
    return;
  }

  const uint16_t top = props.topIndex < props.count ? props.topIndex : 0;
  int y = listTop;
  int drawnRows = 0;
  bool selectedDrawn = false;

  for (uint16_t idx = top; idx < props.count && y < listBottom; ++idx) {
    fui::ListItem item{};
    if (props.rowProvider) {
      props.rowProvider(props.rowProviderCtx, idx, item);
    } else if (props.items) {
      if (idx < props.itemsWindowFirst) break;
      const uint16_t localIdx = static_cast<uint16_t>(idx - props.itemsWindowFirst);
      if (props.itemsWindowCount > 0 && localIdx >= props.itemsWindowCount) break;
      item = props.items[localIdx];
    } else {
      break;
    }

    const int sectionH = (item.sectionHeading && item.sectionHeading[0] != '\0') ? 26 : 0;
    const bool hasReveal = props.reveal && props.reveal->index == static_cast<int16_t>(idx);
    char upperDel[32]{};
    int delW = 58;
    int rightReserved = 24;
    if (hasReveal) {
      formatUpperVi(tr(STR_DELETE), upperDel, sizeof(upperDel));
      delW = std::clamp(measureText(renderer, kAtelierFont11Bold, upperDel, 0) + 22, 58, 96);
      rightReserved = delW + 12;
    } else if (item.toggle) {
      rightReserved = 48;
    } else if (item.value && item.value[0] != '\0') {
      const int valW = std::min(132, measureText(renderer, kAtelierFont12Reg, item.value, 0));
      rightReserved = valW + 30;
    }

    const bool isSel = static_cast<int16_t>(idx) == props.selectedIndex;
    const auto wrapLayout = makeAtelierRowSelectionLayout(MARGIN_X, 0, CONTENT_W, 0, isSel, rightReserved);
    const int maxLabelLines = std::clamp<int>(props.labelText.maxLines > 0 ? props.labelText.maxLines : 1, 1, 2);
    const WrappedAtelierLines labelLines =
        wrapText(renderer, kAtelierFont16Med, item.label ? item.label : "", wrapLayout.textWidth, maxLabelLines);

    const bool hasSub = item.subtitle && item.subtitle[0] != '\0';
    WrappedAtelierLines subLines{};
    if (hasSub) {
      const int maxSubLines = std::clamp<int>(props.subtitleText.maxLines > 0 ? props.subtitleText.maxLines : 1, 1, 2);
      subLines = wrapText(renderer, kAtelierFont12Reg, item.subtitle, wrapLayout.textWidth, maxSubLines);
    }

    const int labelCount = std::max(1, labelLines.count);
    const int subCount = hasSub ? std::max(1, subLines.count) : 0;
    const int textBlockH = labelCount * 20 + (subCount > 0 ? (3 + subCount * 16) : 0);
    const int bodyH = std::max(hasSub ? 72 : 56, 16 + textBlockH + 16);
    const int rowH = sectionH + bodyH;

    if (y + rowH > listBottom && drawnRows > 0) break;

    if (sectionH > 0) {
      char upperSec[64]{};
      formatUpperVi(item.sectionHeading, upperSec, sizeof(upperSec));
      drawText(renderer, MARGIN_X, y + 6, kAtelierFont10Bold, upperSec, AtelierTone::Muted, 1, CONTENT_W, 0, true);
      fillRect(renderer, MARGIN_X, y + 23, CONTENT_W, 1, 1);
    }

    const int ry = y + sectionH;
    const auto sel = makeAtelierRowSelectionLayout(MARGIN_X, ry, CONTENT_W, bodyH, isSel, rightReserved);
    if (sel.selected) {
      fillRect(renderer, sel.barX, sel.barY, sel.barWidth, sel.barHeight, 3);
    }

    char idxBuf[12]{};
    snprintf(idxBuf, sizeof(idxBuf), "%02u", static_cast<unsigned>(idx + 1));
    const AtelierFont& idxFont = sel.selected ? kAtelierFont13Bold : kAtelierFont15Light;
    drawText(renderer, sel.indexX, ry + (bodyH - idxFont.lineHeight) / 2, idxFont, idxBuf,
             sel.selected ? AtelierTone::Ink : AtelierTone::Muted, 0, 32, 0, false);

    if (hasReveal) {
      const int delH = 30;
      const int delX = MARGIN_X + CONTENT_W - delW;
      const int delY = ry + (bodyH - delH) / 2;
      fillCapsule(renderer, delX, delY, delW, delH);
      drawText(renderer, delX, delY + (delH - kAtelierFont11Bold.lineHeight) / 2, kAtelierFont11Bold, upperDel,
               AtelierTone::PaperOnInk, 0, delW, 1, false);
      screen.frame().hit(fui::Rect{static_cast<int16_t>(delX), static_cast<int16_t>(ry), static_cast<int16_t>(delW),
                                   static_cast<int16_t>(bodyH)},
                         props.reveal->action, props.reveal->index, fui::InputTouch, fui::StateNormal);
    } else if (item.toggle) {
      drawToggleSwitch(renderer, MARGIN_X + CONTENT_W - 38, ry + (bodyH - 22) / 2, item.toggleChecked);
    } else {
      const int chevX = MARGIN_X + CONTENT_W - 16;
      drawChevronRight(renderer, chevX, ry + (bodyH - 14) / 2);
      if (item.value && item.value[0] != '\0') {
        const int maxValW = 132;
        const int drawValW = std::min(maxValW, measureText(renderer, kAtelierFont12Reg, item.value, 0));
        drawText(renderer, chevX - 10 - drawValW, ry + (bodyH - kAtelierFont12Reg.lineHeight) / 2, kAtelierFont12Reg,
                 item.value, AtelierTone::Muted, 0, drawValW + 2, 2, true);
      }
    }

    int textY = ry + (bodyH - 1 - textBlockH) / 2;
    for (int li = 0; li < labelLines.count; ++li) {
      drawText(renderer, sel.textX, textY, kAtelierFont16Med, labelLines.lines[li], AtelierTone::Ink, 0, sel.textWidth,
               0, li + 1 == labelLines.count, labelLines.useFallback);
      textY += 20;
    }
    if (hasSub) {
      textY += 3;
      for (int si = 0; si < subLines.count; ++si) {
        drawText(renderer, sel.textX, textY, kAtelierFont12Reg, subLines.lines[si], AtelierTone::Muted, 0,
                 sel.textWidth, 0, si + 1 == subLines.count, subLines.useFallback);
        textY += 16;
      }
    }

    fillRect(renderer, MARGIN_X, ry + bodyH - 1, CONTENT_W, 1, 1);

    if (!item.isHeader && !hasReveal) {
      screen.frame().hit(fui::Rect{MARGIN_X, static_cast<int16_t>(ry), CONTENT_W, static_cast<int16_t>(bodyH)},
                         props.action, item.actionValue, props.inputMask,
                         item.enabled ? fui::StateNormal : fui::StateDisabled);
    }

    y += rowH;
    ++drawnRows;
    if (isSel) selectedDrawn = true;
  }

  if (props.scrollIndicator && drawnRows > 0 && props.count > drawnRows) {
    const int barX = MARGIN_X + CONTENT_W - 2;
    const int barH = std::max(24, listBottom - listTop);
    if (sGrayscalePassActive) {
      fillRect(renderer, barX + 1, listTop, 1, barH, 1);
    }
    const int thumbH = std::max(18, barH * drawnRows / props.count);
    const int maxOff = std::max(0, barH - thumbH);
    const int denom = std::max(1, static_cast<int>(props.count) - drawnRows);
    const int thumbY = listTop + maxOff * static_cast<int>(top) / denom;
    fillRect(renderer, barX, thumbY, 2, thumbH, 3);
  }

  nav.drawnCount = props.count;
  nav.onListRendered(top, drawnRows, selectedDrawn);
  nav.visibleRows = std::max(1, drawnRows);
}

void drawBottomNav(const GfxRenderer& renderer, const int activeTab, UiAppHost::UiScreen* screen,
                   const fui::ActionId action, const int* actionValues) {
  fillRect(renderer, MARGIN_X, BOTTOM_NAV_Y, CONTENT_W, 1, 1);
  const char* const tabLabels[4] = {tr(STR_NAV_HOME), tr(STR_LIBRARY), tr(STR_NAV_FILES), tr(STR_SETTINGS_TITLE)};
  constexpr int colW = CONTENT_W / 4;
  constexpr int hitColW = 480 / 4;
  constexpr int16_t hitY = BOTTOM_NAV_HIT_Y;
  constexpr int16_t hitH = 800 - hitY;
  for (int i = 0; i < 4; ++i) {
    const int tabX = MARGIN_X + i * colW;
    if (screen && actionValues && actionValues[i] >= 0) {
      screen->frame().hit(fui::Rect{static_cast<int16_t>(i * hitColW), hitY, static_cast<int16_t>(hitColW), hitH},
                          action, static_cast<int16_t>(actionValues[i]), fui::InputTouch, fui::StateNormal);
    }
    const bool active = i == activeTab;
    const AtelierFont& font = active ? kAtelierFont11Bold : kAtelierFont11Reg;
    const AtelierTone tone = active ? AtelierTone::Ink : AtelierTone::Muted;
    drawText(renderer, tabX, BOTTOM_NAV_Y + 12, font, tabLabels[i], tone, 0, colW, 1, true);
    if (active) {
      fillCircle(renderer, tabX + colW * 0.5f, BOTTOM_NAV_Y + 34.0f, 2.0f, 3);
    }
  }
}

bool handleBottomNavTap(const int tx, const int ty, const int currentTab, void (*goHomeFn)(), void (*goBackFn)()) {
  if (ty < BOTTOM_NAV_HIT_Y || ty >= 800 || tx < 0 || tx >= 480) {
    return false;
  }
  const int tab = tx / (480 / 4);
  if (tab < 0 || tab >= 4) return false;
  if (tab == currentTab) return true;
  if (tab == 0) {
    sPendingBottomNavTab = -1;
    if (goHomeFn) {
      goHomeFn();
    } else if (goBackFn) {
      goBackFn();
    }
    return true;
  }
  sPendingBottomNavTab = tab;
  if (goHomeFn) goHomeFn();
  return true;
}

int takePendingBottomNavTab() {
  const int tab = sPendingBottomNavTab;
  sPendingBottomNavTab = -1;
  return tab;
}

void drawScrubberTrack(const GfxRenderer& renderer, const int x, const int y, const int w, const int h,
                       const int permille, const bool circleKnob, const bool allowGrayscale) {
  if (w <= 2 || h <= 0) return;
  const int trackY = y + h / 2;
  const int clamped = std::clamp(permille, 0, 1000);
  const int travel = std::max(0, w - 2);
  const int tickX = x + (travel * clamped + 500) / 1000;
  const int fillW = std::max(0, tickX - x);

  if (allowGrayscale || sGrayscalePassActive || renderer.getRenderMode() != GfxRenderer::BW) {
    fillRect(renderer, x, trackY, w, 1, 1);
    if (fillW > 0) fillRect(renderer, x, trackY, fillW, 1, 3);
  } else {
    for (int px = x; px < x + w; px += 2) {
      renderer.drawPixel(px, trackY, true);
    }
    if (fillW > 0) fillRect(renderer, x, trackY, fillW, 2, 3);
  }

  if (circleKnob) {
    fillCircle(renderer, static_cast<float>(tickX + 1), static_cast<float>(trackY), 5.0f, 3);
  } else {
    fillRect(renderer, tickX, y, 2, h, 3);
  }
}

void drawReaderChrome(const GfxRenderer& renderer, const char* chapterTitle, const int currentPage, const int pageCount,
                      const int bookPercent, const bool isBookmarked, const bool pageCountEstimated,
                      const bool showTop, const bool showPageCount, const bool showProgressBar, const bool showBattery,
                      const bool showClock, const bool allowGrayscale) {
  constexpr int readerX = 40;
  constexpr int readerW = 400;

  if (showTop) {
    char pctBuf[20]{};
    const int pct = std::clamp(bookPercent, 0, 100);
    if (isBookmarked) {
      snprintf(pctBuf, sizeof(pctBuf), "• %d%%", pct);
    } else {
      snprintf(pctBuf, sizeof(pctBuf), "%d%%", pct);
    }
    const int pctW = measureText(renderer, kAtelierFont10Bold, pctBuf, 1);
    drawText(renderer, readerX + readerW - pctW - 4, 18, kAtelierFont10Bold, pctBuf, AtelierTone::Muted, 1, pctW + 4,
             2, false);

    if (chapterTitle && chapterTitle[0] != '\0') {
      char upperTitle[128]{};
      formatUpperVi(chapterTitle, upperTitle, sizeof(upperTitle));
      const int maxLeftW = std::max(40, readerW - pctW - 16);
      drawText(renderer, readerX, 18, kAtelierFont10Bold, upperTitle, AtelierTone::Muted, 1, maxLeftW, 0, true);
    }
    fillRect(renderer, readerX, 42, readerW, 1, 1);
  }

  if (showProgressBar) {
    drawScrubberTrack(renderer, readerX, 743, readerW, 11, std::clamp(bookPercent, 0, 100) * 10, false,
                      allowGrayscale);
  }

  if (showPageCount) {
    char pageBuf[40]{};
    if (pageCount > 0) {
      snprintf(pageBuf, sizeof(pageBuf),
               pageCountEstimated ? tr(STR_ATELIER_PAGE_OF_EST_FMT) : tr(STR_ATELIER_PAGE_OF_FMT), currentPage,
               pageCount);
    } else {
      snprintf(pageBuf, sizeof(pageBuf), tr(STR_ATELIER_PAGE_FMT), currentPage);
    }
    drawText(renderer, readerX, 766, kAtelierFont11Reg, pageBuf, AtelierTone::Muted, 0, 210, 0, true);
  }

  char clockText[12]{};
  const bool hasClock = showClock && halClock.isAvailable() &&
                        halClock.formatTime(clockText, sizeof(clockText), SETTINGS.clockFormat == 1);
  char rightBuf[28]{};
  if (hasClock && showBattery) {
    snprintf(rightBuf, sizeof(rightBuf), "%s · %u%%", clockText,
             static_cast<unsigned>(std::min<uint16_t>(100, powerManager.getBatteryPercentage())));
  } else if (hasClock) {
    snprintf(rightBuf, sizeof(rightBuf), "%s", clockText);
  } else if (showBattery) {
    snprintf(rightBuf, sizeof(rightBuf), "%u%%",
             static_cast<unsigned>(std::min<uint16_t>(100, powerManager.getBatteryPercentage())));
  }
  if (rightBuf[0] != '\0') {
    drawText(renderer, readerX + 210, 766, kAtelierFont11Reg, rightBuf, AtelierTone::Muted, 0, readerW - 210, 2,
             false);
  }
}

void drawEditorialMessageScreen(const GfxRenderer& renderer, const char* eyebrowCode, const char* topRight,
                                const char* headline, const char* subline, const char* footerLeft,
                                const char* footerRight) {
  if (eyebrowCode && eyebrowCode[0] != '\0') {
    drawText(renderer, MARGIN_X, 34, kAtelierFont10Bold, eyebrowCode, AtelierTone::Muted, 1, 260, 0, true);
  }
  if (topRight && topRight[0] != '\0') {
    drawText(renderer, MARGIN_X + 260, 34, kAtelierFont10Bold, topRight, AtelierTone::Muted, 1, CONTENT_W - 260, 2,
             true);
  }

  fillRect(renderer, MARGIN_X, 292, 48, 1, 3);
  int y = 312;
  if (headline && headline[0] != '\0') {
    const WrappedAtelierLines largeLines = wrapText(renderer, kAtelierFont34Light, headline, CONTENT_W, 3);
    const bool useLarge = largeLines.count <= 2 && measureText(renderer, kAtelierFont34Light, largeLines.lines[0], 0) <= CONTENT_W;
    const AtelierFont& headFont = useLarge ? kAtelierFont34Light : kAtelierFont27Light;
    const WrappedAtelierLines lines = useLarge ? largeLines : wrapText(renderer, kAtelierFont27Light, headline, CONTENT_W, 4);
    const int step = headFont.lineHeight + 2;
    for (int i = 0; i < lines.count; ++i) {
      drawText(renderer, MARGIN_X, y, headFont, lines.lines[i], AtelierTone::Ink, 0, CONTENT_W, 0,
               i + 1 == lines.count, lines.useFallback);
      y += step;
    }
  }

  if (subline && subline[0] != '\0') {
    y += 14;
    const WrappedAtelierLines subLines = wrapText(renderer, kAtelierFont15Light, subline, CONTENT_W, 3);
    const int subStep = kAtelierFont15Light.lineHeight + 3;
    for (int i = 0; i < subLines.count; ++i) {
      drawText(renderer, MARGIN_X, y, kAtelierFont15Light, subLines.lines[i], AtelierTone::Muted, 0, CONTENT_W, 0,
               i + 1 == subLines.count, subLines.useFallback);
      y += subStep;
    }
  }

  fillRect(renderer, MARGIN_X, BOTTOM_NAV_Y, CONTENT_W, 1, 1);
  if (footerLeft && footerLeft[0] != '\0') {
    drawText(renderer, MARGIN_X, 756, kAtelierFont11Reg, footerLeft, AtelierTone::Muted, 0, 240, 0, true);
  }
  if (footerRight && footerRight[0] != '\0') {
    drawText(renderer, MARGIN_X + 240, 756, kAtelierFont10Bold, footerRight, AtelierTone::Muted, 1, CONTENT_W - 240, 2,
             true);
  }
}

void drawNoveOsSplashScreen(const GfxRenderer& renderer, const char* eyebrowCode, const char* statusLine,
                            const char* subHint, const char* footerLeft, const char* footerRight) {
  if (!isPortrait480x800(renderer)) return;

  static constexpr char kDeviceLabel[] = "XTEINK X4 PRO";
  const int deviceLabelW = measureText(renderer, kAtelierFont10Bold, kDeviceLabel, 1);
  if (eyebrowCode && eyebrowCode[0] != '\0') {
    const int maxLeftW = std::max(0, CONTENT_W - deviceLabelW - 12);
    drawText(renderer, MARGIN_X, 36, kAtelierFont10Bold, eyebrowCode, AtelierTone::Muted, 1, maxLeftW, 0, true);
  }
  drawText(renderer, MARGIN_X, 36, kAtelierFont10Bold, kDeviceLabel, AtelierTone::Muted, 1, CONTENT_W, 2, true);
  fillRect(renderer, MARGIN_X, 60, CONTENT_W, 1, 3);

  constexpr int logoX = (480 - NoveOsBranding::LOCKUP_WIDTH) / 2;
  constexpr int logoY = 196;
  for (int row = 0; row < NoveOsBranding::LOCKUP_HEIGHT; ++row) {
    for (int col = 0; col < NoveOsBranding::LOCKUP_WIDTH; ++col) {
      const int idx = row * NoveOsBranding::LOCKUP_WIDTH + col;
      const uint8_t byte = NoveOsBranding::NOVEOS_LOCKUP_2BPP[idx >> 2];
      const uint8_t shade = (byte >> (6 - (idx & 3) * 2)) & 0x03u;
      if (shade > 0) paintPixel(renderer, logoX + col, logoY + row, shade);
    }
  }

  constexpr int ruleY = logoY + NoveOsBranding::LOCKUP_HEIGHT + 28;
  fillRect(renderer, 216, ruleY, 48, 1, 3);
  if (statusLine && statusLine[0] != '\0') {
    drawText(renderer, MARGIN_X, ruleY + 18, kAtelierFont14Reg, statusLine, AtelierTone::Ink, 0, CONTENT_W, 1, true);
  }
  if (subHint && subHint[0] != '\0') {
    drawText(renderer, MARGIN_X, ruleY + 46, kAtelierFont12Reg, subHint, AtelierTone::Muted, 0, CONTENT_W, 1, true);
  }

  fillRect(renderer, MARGIN_X, 736, CONTENT_W, 1, 3);
  const int footerRightW = (footerRight && footerRight[0] != '\0') ? measureText(renderer, kAtelierFont10Bold, footerRight, 1) : 0;
  if (footerLeft && footerLeft[0] != '\0') {
    const int maxLeftW = footerRightW > 0 ? std::max(0, CONTENT_W - footerRightW - 12) : CONTENT_W;
    drawText(renderer, MARGIN_X, 750, kAtelierFont10Bold, footerLeft, AtelierTone::Muted, 1, maxLeftW, 0, true);
  }
  if (footerRight && footerRight[0] != '\0') {
    drawText(renderer, MARGIN_X, 750, kAtelierFont10Bold, footerRight, AtelierTone::Muted, 1, CONTENT_W, 2, true);
  }
}

void renderOptionSheet(const GfxRenderer& renderer, const AtelierOptionSheetSpec& spec, const AtelierHitAdder addHit,
                       void* hitCtx, const freeink::ui::ActionId actionOption,
                       const freeink::ui::ActionId actionChrome) {
  if (!spec.options || spec.optionCount <= 0) return;
  const int screenH = renderer.getScreenHeight();
  const int screenW = renderer.getScreenWidth();

  const auto beginSheet = [&](const int sheetH) {
    const int sheetY = screenH - sheetH;
    fillRect(renderer, 0, sheetY, screenW, sheetH, 0);
    fillRect(renderer, MARGIN_X, sheetY, CONTENT_W, 1, 3);
    if (addHit) {
      addHit(hitCtx,
             fui::Rect{0, static_cast<int16_t>(sheetY), static_cast<int16_t>(screenW), static_cast<int16_t>(sheetH)},
             actionChrome, 0);
      // Nav strip under the sheet: value 1 marks it as dismiss-on-release.
      // Registered before option/button hits so those still win the
      // newest-first scan inside the band.
      addHit(hitCtx,
             fui::Rect{0, BOTTOM_NAV_HIT_Y, static_cast<int16_t>(screenW),
                       static_cast<int16_t>(screenH - BOTTOM_NAV_HIT_Y)},
             actionChrome, 1);
    }
    return sheetY;
  };

  if (spec.optionCount == 2) {
    const WrappedAtelierLines titleLines =
        (spec.title && spec.title[0] != '\0') ? wrapText(renderer, kAtelierFont27Light, spec.title, CONTENT_W, 2)
                                              : WrappedAtelierLines{};
    const WrappedAtelierLines headLines =
        (spec.headline && spec.headline[0] != '\0') ? wrapText(renderer, kAtelierFont16Med, spec.headline, CONTENT_W, 2)
                                                    : WrappedAtelierLines{};
    const WrappedAtelierLines msgLines =
        (spec.message && spec.message[0] != '\0') ? wrapText(renderer, kAtelierFont14Reg, spec.message, CONTENT_W, 4)
                                                  : WrappedAtelierLines{};

    int contentH = 24 + 18 + 44 + 34;
    if (titleLines.count > 0) contentH += titleLines.count * (kAtelierFont27Light.lineHeight + 2) + 8;
    if (headLines.count > 0) contentH += headLines.count * (kAtelierFont16Med.lineHeight + 2) + 6;
    if (msgLines.count > 0) contentH += msgLines.count * (kAtelierFont14Reg.lineHeight + 2) + 6;

    const int sheetY = beginSheet(std::min(screenH - 100, contentH));
    int y = sheetY + 24;
    const auto drawLinesBlock = [&](const WrappedAtelierLines& lines, const AtelierFont& font, const AtelierTone tone,
                                    const int gapAfter) {
      for (int i = 0; i < lines.count; ++i) {
        drawText(renderer, MARGIN_X, y, font, lines.lines[i], tone, 0, CONTENT_W, 0, i + 1 == lines.count,
                 lines.useFallback);
        y += font.lineHeight + 2;
      }
      if (lines.count > 0) y += gapAfter;
    };
    drawLinesBlock(titleLines, kAtelierFont27Light, AtelierTone::Ink, 8);
    drawLinesBlock(headLines, kAtelierFont16Med, AtelierTone::Ink, 6);
    drawLinesBlock(msgLines, kAtelierFont14Reg, AtelierTone::Muted, 0);

    const int btnY = screenH - 34 - 44;
    const int primary = (spec.primaryIndex == 0 || spec.primaryIndex == 1) ? spec.primaryIndex : 1;
    const int secondary = 1 - primary;
    const char* secLabel = spec.options[secondary] ? spec.options[secondary] : "";
    const char* priLabel = spec.options[primary] ? spec.options[primary] : "";

    const int secW = std::clamp(measureText(renderer, kAtelierFont13Reg, secLabel, 0) + 36, 112, 184);
    const int secX = MARGIN_X;
    const bool secSel = spec.selectedIndex == secondary;
    drawCapsuleButton(renderer, secX, btnY, secW, 44, secLabel, kAtelierFont13Reg, false, secSel, secSel);
    if (addHit) {
      addHit(hitCtx, fui::Rect{static_cast<int16_t>(secX), static_cast<int16_t>(btnY), static_cast<int16_t>(secW), 44},
             actionOption, static_cast<int16_t>(secondary));
    }

    const int ctaW = std::clamp(measureText(renderer, kAtelierFont13Bold, priLabel, 0) + 48, 152, 224);
    const int ctaX = MARGIN_X + CONTENT_W - ctaW;
    drawCapsuleButton(renderer, ctaX, btnY, ctaW, 44, priLabel, kAtelierFont13Bold, true, false,
                      spec.selectedIndex == primary);
    if (addHit) {
      addHit(hitCtx, fui::Rect{static_cast<int16_t>(ctaX), static_cast<int16_t>(btnY), static_cast<int16_t>(ctaW), 44},
             actionOption, static_cast<int16_t>(primary));
    }
    return;
  }

  const int visibleCount = std::min(spec.optionCount, 9);
  const int rowH = spec.optionCount <= 6 ? 54 : 46;
  int topIdx = 0;
  if (spec.optionCount > visibleCount && spec.selectedIndex >= visibleCount) {
    topIdx = std::min(spec.optionCount - visibleCount, spec.selectedIndex - visibleCount + 1);
  }

  const bool hasTitle = spec.title && spec.title[0] != '\0';
  const bool hasHead = spec.headline && spec.headline[0] != '\0';
  const bool hasMsg = spec.message && spec.message[0] != '\0';
  const int headerH = 20 + (hasTitle ? 30 : 0) + (hasHead ? 22 : 0) + (hasMsg ? 22 : 0) + 12;
  const int sheetY = beginSheet(std::min(screenH - 80, headerH + visibleCount * rowH + 24));

  int y = sheetY + 18;
  char countBuf[24]{};
  snprintf(countBuf, sizeof(countBuf), tr(STR_ATELIER_COUNT_ITEMS_FMT), spec.optionCount);
  const int countW = measureText(renderer, kAtelierFont10Bold, countBuf, 1);
  if (hasTitle) {
    drawText(renderer, MARGIN_X + CONTENT_W - countW, y + 5, kAtelierFont10Bold, countBuf, AtelierTone::Muted, 1,
             countW + 4, 2, false);
    drawText(renderer, MARGIN_X, y, kAtelierFont20Light, spec.title, AtelierTone::Ink, 0, CONTENT_W - countW - 12, 0,
             true);
    y += 30;
  }
  if (hasHead) {
    drawText(renderer, MARGIN_X, y, kAtelierFont13Reg, spec.headline, AtelierTone::Ink, 0, CONTENT_W, 0, true);
    y += 22;
  }
  if (hasMsg) {
    drawText(renderer, MARGIN_X, y, kAtelierFont13Reg, spec.message, AtelierTone::Muted, 0, CONTENT_W, 0, true);
    y += 22;
  }
  y += 6;
  fillRect(renderer, MARGIN_X, y, CONTENT_W, 1, 3);
  y += 1;

  for (int vi = 0; vi < visibleCount && y + rowH <= screenH - 12; ++vi) {
    const int idx = topIdx + vi;
    const bool isSel = (idx == spec.selectedIndex);
    const auto sel = makeAtelierRowSelectionLayout(MARGIN_X, y, CONTENT_W, rowH, isSel, 16);
    if (sel.selected) {
      fillRect(renderer, sel.barX, sel.barY, sel.barWidth, sel.barHeight, 3);
    }
    char idxBuf[12]{};
    snprintf(idxBuf, sizeof(idxBuf), "%02d", idx + 1);
    const AtelierFont& idxFont = isSel ? kAtelierFont13Bold : kAtelierFont15Light;
    drawText(renderer, sel.indexX, y + (rowH - idxFont.lineHeight) / 2, idxFont, idxBuf,
             isSel ? AtelierTone::Ink : AtelierTone::Muted, 0, 32, 0, false);

    const char* label = spec.options[idx] ? spec.options[idx] : "";
    drawText(renderer, sel.textX, y + (rowH - kAtelierFont16Med.lineHeight) / 2, kAtelierFont16Med, label,
             AtelierTone::Ink, 0, sel.textWidth, 0, true);
    fillRect(renderer, MARGIN_X, y + rowH - 1, CONTENT_W, 1, 1);
    if (addHit) {
      addHit(hitCtx,
             fui::Rect{MARGIN_X, static_cast<int16_t>(y), CONTENT_W, static_cast<int16_t>(rowH)}, actionOption,
             static_cast<int16_t>(idx));
    }
    y += rowH;
  }
}

void drawSubHeader(const GfxRenderer& renderer, const char* leftText, const char* rightText) {
  constexpr int y = HEADER_RULE_Y + 8;
  const int rightW = (rightText && rightText[0] != '\0') ? std::min(200, measureText(renderer, kAtelierFont11Reg, rightText, 0))
                                                         : 0;
  if (rightW > 0) {
    drawText(renderer, MARGIN_X + CONTENT_W - rightW, y, kAtelierFont11Reg, rightText, AtelierTone::Muted, 0,
             rightW + 2, 2, true);
  }
  if (leftText && leftText[0] != '\0') {
    const int maxLeftW = rightW > 0 ? std::max(40, CONTENT_W - rightW - 12) : CONTENT_W;
    drawText(renderer, MARGIN_X, y, kAtelierFont11Reg, leftText, AtelierTone::Muted, 0, maxLeftW, 0, true);
  }
  fillRect(renderer, MARGIN_X, HEADER_RULE_Y + 32, CONTENT_W, 1, 1);
}

void drawFooterHint(const GfxRenderer& renderer, const char* text) {
  fillRect(renderer, MARGIN_X, BOTTOM_NAV_Y, CONTENT_W, 1, 1);
  if (text && text[0] != '\0') {
    drawText(renderer, MARGIN_X, 756, kAtelierFont11Reg, text, AtelierTone::Muted, 0, CONTENT_W, 1, true);
  }
}

void paintStatusScreenContent(const GfxRenderer& renderer, const AtelierStatusScreenSpec& spec) {
  drawHeader(renderer, spec.title, spec.rightMeta, spec.backButton, spec.eyebrowCode);
  const bool hasSub =
      (spec.subtitle && spec.subtitle[0] != '\0') || (spec.subtitleRight && spec.subtitleRight[0] != '\0');
  if (hasSub) drawSubHeader(renderer, spec.subtitle, spec.subtitleRight);

  const int topY = hasSub ? HEADER_RULE_Y + 33 : HEADER_RULE_Y + 1;
  const int bottomY = (spec.footerHint && spec.footerHint[0] != '\0') ? BOTTOM_NAV_Y : 784;

  const WrappedAtelierLines headLines =
      (spec.headline && spec.headline[0] != '\0') ? wrapText(renderer, kAtelierFont20Light, spec.headline, CONTENT_W, 3)
                                                  : WrappedAtelierLines{};
  const WrappedAtelierLines bodyLines =
      (spec.body && spec.body[0] != '\0') ? wrapText(renderer, kAtelierFont14Reg, spec.body, CONTENT_W, 4)
                                          : WrappedAtelierLines{};
  const WrappedAtelierLines detailLines =
      (spec.detail && spec.detail[0] != '\0') ? wrapText(renderer, kAtelierFont13Reg, spec.detail, CONTENT_W, 3)
                                              : WrappedAtelierLines{};
  const bool hasProg = spec.progressPct >= 0;
  const bool hasProgLbl = hasProg && spec.progressLabel && spec.progressLabel[0] != '\0';

  int blockH = 0;
  if (headLines.count > 0) blockH += headLines.count * (kAtelierFont20Light.lineHeight + 4);
  if (bodyLines.count > 0) blockH += (blockH > 0 ? 12 : 0) + bodyLines.count * (kAtelierFont14Reg.lineHeight + 3);
  if (detailLines.count > 0) blockH += (blockH > 0 ? 10 : 0) + detailLines.count * (kAtelierFont13Reg.lineHeight + 3);
  if (hasProg) blockH += (blockH > 0 ? 22 : 0) + 6 + (hasProgLbl ? kAtelierFont12Reg.lineHeight + 10 : 0);

  int y = topY + std::max(24, (bottomY - topY - blockH) / 2);
  auto paintLines = [&](const WrappedAtelierLines& lines, const AtelierFont& font, const AtelierTone tone,
                        const int gapStep) {
    for (int i = 0; i < lines.count; ++i) {
      drawText(renderer, MARGIN_X, y, font, lines.lines[i], tone, 0, CONTENT_W, 1, i + 1 == lines.count,
               lines.useFallback);
      y += font.lineHeight + gapStep;
    }
  };
  if (headLines.count > 0) {
    paintLines(headLines, kAtelierFont20Light, AtelierTone::Ink, 4);
    if (bodyLines.count > 0 || detailLines.count > 0 || hasProg) y += 12;
  }
  if (bodyLines.count > 0) {
    paintLines(bodyLines, kAtelierFont14Reg, AtelierTone::Muted, 3);
    if (detailLines.count > 0 || hasProg) y += 10;
  }
  if (detailLines.count > 0) {
    paintLines(detailLines, kAtelierFont13Reg, AtelierTone::Muted, 3);
    if (hasProg) y += 22;
  } else if (hasProg && (headLines.count > 0 || bodyLines.count > 0)) {
    y += 12;
  }

  if (hasProg) {
    constexpr int trackW = 320;
    const int trackX = MARGIN_X + (CONTENT_W - trackW) / 2;
    const int pct = std::clamp(spec.progressPct, 0, 100);
    fillRect(renderer, trackX, y + 2, trackW, 1, 1);
    const int fillW = trackW * pct / 100;
    if (fillW > 0) fillRect(renderer, trackX, y, fillW, 4, 3);
    y += 14;
    if (hasProgLbl) {
      drawText(renderer, MARGIN_X, y, kAtelierFont12Reg, spec.progressLabel, AtelierTone::Muted, 0, CONTENT_W, 1, true);
    }
  }

  if (spec.footerHint && spec.footerHint[0] != '\0') {
    drawFooterHint(renderer, spec.footerHint);
  }
}

void drawStatusScreen(GfxRenderer& renderer, const AtelierStatusScreenSpec& spec,
                      const HalDisplay::RefreshMode refresh, const bool bwFastOnly) {
  struct Ctx {
    GfxRenderer* renderer;
    const AtelierStatusScreenSpec* spec;
  } ctx{&renderer, &spec};
  renderGrayscalePass(
      renderer, refresh,
      [](void* raw) {
        auto* c = static_cast<Ctx*>(raw);
        c->renderer->clearScreen();
        paintStatusScreenContent(*c->renderer, *c->spec);
      },
      &ctx, bwFastOnly);
}

}  // namespace AtelierChrome

#endif

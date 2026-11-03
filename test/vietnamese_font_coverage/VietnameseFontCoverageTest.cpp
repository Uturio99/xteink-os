// WP1 font feasibility — Vietnamese coverage (Tier H host test).
// Strategies (see docs/design/specs/M2-ADDENDUM.md §4):
//   A: shipped built-in NotoSans bitmaps (fallback floor)
//   B: minimal bundled UI Serif (generated headers in ./generated)
//   C: SDK vector engine (TtfFont + stb_truetype) on NotoSerif-Regular.ttf
// Also reports F1 exact const-data flash bytes per font and mirrors the
// firmware glyph-arena profiles (SdCardFontSystem.cpp: 1 MB/4096 with PSRAM,
// 32 KB/768 without) as HOST-DERIVED diagnostics.

#include <EpdFont.h>

#include <FontArena.h>
#include <TtfFont.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

// --- Strategy A: shipped built-ins (regular faces of every UI size) ------
#include "builtinFonts/notosans_12_regular.h"
#include "builtinFonts/notosans_14_regular.h"
#include "builtinFonts/notosans_16_regular.h"
#include "builtinFonts/notosans_18_regular.h"

// --- Strategy B: generated bundled UI Serif at M1 typography sizes --------
#include "notoserif_12_regular.h"
#include "notoserif_13_regular.h"
#include "notoserif_15_regular.h"
#include "notoserif_17_regular.h"
#include "notoserif_30_regular.h"
#include "notoserif_34_regular.h"

namespace {

// Host-test stubs following test/inflate_reader/InflateReaderTest.cpp:15-16:
// the checksum entry points are never called on the exercised paths.
extern "C" uint32_t uzlib_adler32(const void*, unsigned int, uint32_t) { std::abort(); }
extern "C" uint32_t uzlib_crc32(const void*, unsigned int, uint32_t) { std::abort(); }


using freeink::font::Arena;
using freeink::font::TtfFont;

// The codepoint set M1 Vietnamese UI must render. Mirrors the intervals in
// lib/EpdFont/scripts/fontconvert.py:57-68 plus M1 punctuation and currency.
std::vector<uint32_t> vietnameseSet() {
  std::vector<uint32_t> cps;
  for (uint32_t cp = 0x1EA0; cp <= 0x1EF9; ++cp) cps.push_back(cp);  // VN Extended
  for (uint32_t cp : {0x01A0u, 0x01A1u, 0x01AFu, 0x01B0u}) cps.push_back(cp);  // Ơ ơ Ư ư
  for (uint32_t cp = 0x0300; cp <= 0x036F; ++cp) cps.push_back(cp);  // combining marks (NFD)
  for (uint32_t cp = 0x00C0; cp <= 0x017F; ++cp) cps.push_back(cp);  // Latin-1 + Ext-A
  for (uint32_t cp = 0x20; cp <= 0x7E; ++cp) cps.push_back(cp);  // ASCII
  for (uint32_t cp : {0x2010u, 0x2011u, 0x2013u, 0x2014u, 0x2018u, 0x2019u,
                      0x201Cu, 0x201Du, 0x2026u, 0x00ABu, 0x00BBu, 0x20ABu}) {
    cps.push_back(cp);  // M1 punctuation + ₫ (dong)
  }
  return cps;
}

// NFC and NFD spellings of the same sentence (ữ ơ ạ have canonical NFD forms).
const char* kNfc = "Hữu ích, ơi ơ, đã rõ.";
const char* kNfd = "Hu\xCC\x9B\xCC\x83 i\xCC\x81" "ch, o\xCC\x9B" "i o\xCC\x9B, d\xCC\xA3 r\xCC\x89" "o\xCC\x82.";  // NFD(u+horn+tilde, i+acute, o+horn, d+dot, r+hook, o+circumflex)

std::vector<uint32_t> codepointsOf(const char* utf8) {
  std::vector<uint32_t> out;
  const uint8_t* p = reinterpret_cast<const uint8_t*>(utf8);
  while (*p) {
    if (*p < 0x80) {
      out.push_back(*p++);
    } else {
      uint32_t cp = 0;
      int extra = (*p & 0xE0) == 0xC0 ? 1 : (*p & 0xF0) == 0xE0 ? 2 : 3;
      cp = *p++ & (0xFF >> (extra + 1));
      for (int i = 0; i < extra && *p; ++i) cp = (cp << 6) | (*p++ & 0x3F);
      out.push_back(cp);
    }
  }
  return out;
}

void expectCovered(const EpdFontData* face, const char* label) {
  EpdFont font(face);
  std::vector<uint32_t> missing;
  for (uint32_t cp : vietnameseSet()) {
    if (!font.hasCodepoint(cp)) missing.push_back(cp);
  }
  if (!missing.empty()) {
    std::string hex;
    char buf[16];
    for (size_t i = 0; i < missing.size() && i < 12; ++i) {
      snprintf(buf, sizeof(buf), "U+%04X ", missing[i]);
      hex += buf;
    }
    FAIL() << label << ": missing " << missing.size() << " codepoints, first: " << hex;
  }
  SUCCEED();
}

// F1: exact const-data bytes a font costs in flash (bitmaps + tables + struct).
#define FONT_FLASH_BYTES(name)                                                  \
  (sizeof(name##Bitmaps) + sizeof(name##Glyphs) + sizeof(name##Intervals) +    \
   sizeof(name##Groups) + sizeof(name##KernLeftCodepoints) +                   \
   sizeof(name##KernLeftClassIds) + sizeof(name##KernRightCodepoints) +        \
   sizeof(name##KernRightClassIds) + sizeof(name##KernRowOffsets) +             \
   sizeof(name##KernSparseCols) + sizeof(name##KernSparseValues) +              \
   sizeof(name##LigaturePairs) + sizeof(EpdFontData))

std::vector<uint8_t> loadFile(const char* path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return {};
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

}  // namespace

// ---------------------------------------------------------------------------
// Strategy A — shipped built-in NotoSans bitmaps
// ---------------------------------------------------------------------------

TEST(A_BuiltinSansCoverage, VietnameseCompletePerUiSize) {
  expectCovered(&notosans_12_regular, "notosans_12_regular");
  expectCovered(&notosans_14_regular, "notosans_14_regular");
  expectCovered(&notosans_16_regular, "notosans_16_regular");
  expectCovered(&notosans_18_regular, "notosans_18_regular");
}

TEST(A_BuiltinSansCoverage, NfcAndNfdFixtureCodepointsPresent) {
  EpdFont font16(&notosans_16_regular);
  for (const char* fixture : {kNfc, kNfd}) {
    for (uint32_t cp : codepointsOf(fixture)) {
      EXPECT_TRUE(font16.hasCodepoint(cp)) << "missing U+" << std::hex << cp;
    }
  }
}

TEST(A_BuiltinSansCoverage, PrecomposedGlyphDecompressesNonEmpty) {
  // 2-bit compressed fonts: force the decompression path on a stacked-diacritic glyph.
  EpdFont font16(&notosans_16_regular);
  EpdGlyph solid;
  const EpdGlyph* g = font16.getGlyphMetrics(0x1EAD, solid);  // ậ
  ASSERT_NE(g, nullptr);
  EXPECT_GT(g->width, 0);
  EXPECT_GT(g->height, 0);
  EXPECT_GT(g->dataLength, 0);
}

// ---------------------------------------------------------------------------
// Strategy B — minimal bundled UI Serif at M1 typography sizes
// ---------------------------------------------------------------------------

TEST(B_BundledSerifCoverage, VietnameseCompletePerM1Size) {
  expectCovered(&notoserif_12_regular, "notoserif_12_regular");  // micro
  expectCovered(&notoserif_13_regular, "notoserif_13_regular");  // secondary
  expectCovered(&notoserif_15_regular, "notoserif_15_regular");  // body
  expectCovered(&notoserif_17_regular, "notoserif_17_regular");  // section
  expectCovered(&notoserif_30_regular, "notoserif_30_regular");  // screenTitle
  expectCovered(&notoserif_34_regular, "notoserif_34_regular");  // display
}

TEST(B_BundledSerifCoverage, NfcAndNfdFixtureCodepointsPresent) {
  EpdFont font15(&notoserif_15_regular);
  for (const char* fixture : {kNfc, kNfd}) {
    for (uint32_t cp : codepointsOf(fixture)) {
      EXPECT_TRUE(font15.hasCodepoint(cp)) << "missing U+" << std::hex << cp;
    }
  }
}

// ---------------------------------------------------------------------------
// Strategy C — SDK vector engine on NotoSerif-Regular.ttf
// ---------------------------------------------------------------------------

TEST(C_VectorSerifCoverage, VietnameseComplete) {
  std::vector<uint8_t> ttf = loadFile(WP1_SERIF_TTF);
  ASSERT_FALSE(ttf.empty()) << "serif TTF not found: " << WP1_SERIF_TTF;

  std::vector<uint8_t> arenaBuf(1024 * 1024);  // firmware PSRAM profile (1 MB)
  Arena arena(arenaBuf.data(), arenaBuf.size());
  TtfFont font;
  ASSERT_TRUE(font.init(ttf.data(), static_cast<uint32_t>(ttf.size()), arena));

  std::vector<uint32_t> missing;
  for (uint32_t cp : vietnameseSet()) {
    if (!font.hasGlyph(cp)) missing.push_back(cp);
  }
  EXPECT_TRUE(missing.empty());
  if (!missing.empty()) {
    for (size_t i = 0; i < missing.size() && i < 12; ++i) {
      printf("[C] missing glyph: U+%04X\n", missing[i]);
    }
  }
}

TEST(C_VectorSerifCoverage, RasterizesAtReaderAndUiSizes) {
  std::vector<uint8_t> ttf = loadFile(WP1_SERIF_TTF);
  ASSERT_FALSE(ttf.empty());
  std::vector<uint8_t> arenaBuf(1024 * 1024);
  Arena arena(arenaBuf.data(), arenaBuf.size());
  TtfFont font;
  ASSERT_TRUE(font.init(ttf.data(), static_cast<uint32_t>(ttf.size()), arena));

  const uint16_t sizes[] = {12, 13, 15, 16, 17, 18, 19, 22, 30, 34};
  const uint32_t sample[] = {0x1EAD, 0x1EEF, 0x01B0, 0x01A1, 0x0303, 0x20AB, 'a', 0x1EF9};
  for (uint16_t size : sizes) {
    for (uint32_t cp : sample) {
      const auto* glyph = font.rasterize(cp, size);
      ASSERT_NE(glyph, nullptr) << "size " << size << " cp U+" << std::hex << cp;
      EXPECT_GT(glyph->width, 0) << "size " << size << " cp U+" << std::hex << cp;
      bool ink = false;
      for (int i = 0; i < glyph->width * glyph->height && !ink; ++i) {
        ink = glyph->pixels[i] != 0;
      }
      EXPECT_TRUE(ink) << "empty bitmap: size " << size << " cp U+" << std::hex << cp;
    }
  }
}

TEST(C_VectorArenaDiagnostics, HostDerivedHighWater) {
  // HOST-DERIVED numbers (not e-paper behavior). Mirrors SdCardFontSystem.cpp
  // cache profiles: 1 MB / 4096 glyphs with PSRAM; 32 KB without. The X4 Pro
  // has PSRAM, so the acceptance gate applies to the 1 MB profile; the 32 KB
  // profile is record-only (its flush-on-full degradation on non-PSRAM boards
  // is documented behavior, see SdCardFontSystem.cpp:622-627).
  std::vector<uint8_t> ttf = loadFile(WP1_SERIF_TTF);
  ASSERT_FALSE(ttf.empty());

  {  // X4 Pro profile: gates.
    std::vector<uint8_t> arenaBuf(1024 * 1024);
    Arena arena(arenaBuf.data(), arenaBuf.size());
    TtfFont font;
    ASSERT_TRUE(font.init(ttf.data(), static_cast<uint32_t>(ttf.size()), arena));
    for (uint32_t cp : vietnameseSet()) {
      (void)font.advance(cp, 19, 0);
      (void)font.rasterize(cp, 19);
    }
    printf("[C] X4 Pro profile: capacity=1048576 used=%zu highWater=%zu failedAllocSize=%zu\n",
           arena.used(), arena.highWater(), arena.failedAllocSize());
    EXPECT_EQ(arena.failedAllocSize(), 0);
    EXPECT_LT(arena.highWater(), arena.capacity());
    RecordProperty("x4pro_arena_high_water", std::to_string(arena.highWater()));
  }
  {  // No-PSRAM profile: diagnostic only.
    std::vector<uint8_t> arenaBuf(32 * 1024);
    Arena arena(arenaBuf.data(), arenaBuf.size());
    TtfFont font;
    ASSERT_TRUE(font.init(ttf.data(), static_cast<uint32_t>(ttf.size()), arena));
    size_t refused = 0;
    for (uint32_t cp : vietnameseSet()) {
      (void)font.advance(cp, 19, 0);
      if (font.rasterize(cp, 19) == nullptr) ++refused;
    }
    printf("[C] no-PSRAM profile: capacity=32768 used=%zu highWater=%zu failedAllocSize=%zu rasterNull=%zu (pathological full-set sweep)\n",
           arena.used(), arena.highWater(), arena.failedAllocSize(), refused);
    EXPECT_LE(arena.highWater(), arena.capacity());
  }
}

// ---------------------------------------------------------------------------
// Fallback floor — built-ins must cover everything serif paths might miss
// ---------------------------------------------------------------------------

TEST(FallbackFloor, BuiltInSansCoversFullVietnameseSet) {
  EpdFont fonts[] = {EpdFont(&notosans_12_regular), EpdFont(&notosans_14_regular),
                     EpdFont(&notosans_16_regular), EpdFont(&notosans_18_regular)};
  std::vector<uint32_t> uncovered;
  for (uint32_t cp : vietnameseSet()) {
    bool any = false;
    for (EpdFont& f : fonts) any = any || f.hasCodepoint(cp);
    if (!any) uncovered.push_back(cp);
  }
  EXPECT_TRUE(uncovered.empty());
  for (size_t i = 0; i < uncovered.size() && i < 12; ++i) {
    printf("[F] no built-in face covers U+%04X\n", uncovered[i]);
  }
}

// ---------------------------------------------------------------------------
// F1 — exact const-data flash bytes (printed for the WP1 report)
// ---------------------------------------------------------------------------

TEST(F1_FlashBytes, ReportConstDataPerFont) {
  const size_t serif[] = {FONT_FLASH_BYTES(notoserif_12_regular), FONT_FLASH_BYTES(notoserif_13_regular),
                          FONT_FLASH_BYTES(notoserif_15_regular), FONT_FLASH_BYTES(notoserif_17_regular),
                          FONT_FLASH_BYTES(notoserif_30_regular), FONT_FLASH_BYTES(notoserif_34_regular)};
  size_t serifTotal = 0;
  const int sizes[] = {12, 13, 15, 17, 30, 34};
  for (size_t i = 0; i < 6; ++i) {
    printf("[F1] notoserif_%d_regular = %zu bytes\n", sizes[i], serif[i]);
    serifTotal += serif[i];
  }
  printf("[F1] bundled UI serif TOTAL (regular only, 6 sizes) = %zu bytes\n", serifTotal);
  printf("[F1] builtin notosans_16_regular (methodology check) = %zu bytes\n",
         static_cast<size_t>(FONT_FLASH_BYTES(notosans_16_regular)));
  EXPECT_GT(serifTotal, 0);
  EXPECT_GT(FONT_FLASH_BYTES(notosans_16_regular), 33000);  // header declares 33164 bitmap bytes
}

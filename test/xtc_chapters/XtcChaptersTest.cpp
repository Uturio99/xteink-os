#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <vector>

#include "HalStorage.h"
#include "XtcParser.h"

namespace {

constexpr uint32_t kXtcMagic = 0x00435458;  // "XTC\0" little-endian
constexpr size_t kChapterRecord = 96;

void putU16(std::vector<uint8_t>& b, size_t off, uint16_t v) { std::memcpy(b.data() + off, &v, sizeof(v)); }
void putU32(std::vector<uint8_t>& b, size_t off, uint32_t v) { std::memcpy(b.data() + off, &v, sizeof(v)); }
void putU64(std::vector<uint8_t>& b, size_t off, uint64_t v) { std::memcpy(b.data() + off, &v, sizeof(v)); }

// Builds minimal in-memory XTC images against the production parser:
// [56-byte header][chapter records][page table]. hasMetadata stays 0 so
// open() skips the title/author block; dataOffset/thumbOffset stay 0.
struct XtcImage {
  std::vector<uint8_t> bytes;

  void header(uint16_t pageCount, uint8_t hasChapters, size_t chapterOffset, size_t pageTableOffset) {
    bytes.assign(56, 0);
    putU32(bytes, 0x00, kXtcMagic);
    bytes[0x04] = 1;  // version 1.0
    putU16(bytes, 0x06, pageCount);
    bytes[0x0B] = hasChapters;
    putU64(bytes, 0x18, pageTableOffset);
    putU32(bytes, 0x30, static_cast<uint32_t>(chapterOffset));
  }

  void addChapter(const std::string& name, uint16_t startPage1, uint16_t endPage1) {
    const size_t at = bytes.size();
    bytes.resize(at + kChapterRecord, 0);
    std::memcpy(bytes.data() + at, name.data(), std::min(name.size(), size_t(80)));
    putU16(bytes, at + 0x50, startPage1);
    putU16(bytes, at + 0x52, endPage1);
  }

  void addZeroTerminator() { bytes.resize(bytes.size() + kChapterRecord, 0); }

  void addGarbageRecords(size_t count) {
    const size_t at = bytes.size();
    bytes.resize(at + count * kChapterRecord, 0xFF);
  }

  void addPageTable(uint16_t pageCount) {
    const size_t at = bytes.size();
    for (uint16_t i = 0; i < pageCount; i++) {
      const size_t entry = at + static_cast<size_t>(i) * 16;
      bytes.resize(entry + 16, 0);
      putU64(bytes, entry + 0x00, 0x1000 + i);
      putU32(bytes, entry + 0x08, 48000);
      putU16(bytes, entry + 0x0C, 480);
      putU16(bytes, entry + 0x0E, 800);
    }
  }
};

void install(const std::string& path, const XtcImage& image) { Storage.setFile(path, image.bytes); }

xtc::XtcParser& openParser(const std::string& path) {
  static xtc::XtcParser parser;  // fresh state each call via close+open
  parser.close();
  EXPECT_EQ(parser.open(path.c_str()), xtc::XtcError::OK);
  return parser;
}

TEST(XtcChapters, ParsesNormalChapterTable) {
  Storage.reset();
  XtcImage image;
  image.header(10, 1, 56, 56 + 7 * kChapterRecord);
  image.addChapter("One", 1, 3);
  image.addChapter("Two", 4, 6);
  image.addChapter("Three", 7, 9);
  image.addChapter(std::string(80, 'A'), 10, 10);
  image.addZeroTerminator();
  image.addGarbageRecords(2);
  image.addPageTable(10);
  install("normal.xtc", image);

  auto& parser = openParser("normal.xtc");
  EXPECT_TRUE(parser.hasChapters());
  const auto& chapters = parser.getChapters();

  ASSERT_EQ(chapters.size(), 4u);
  EXPECT_EQ(chapters[0].name, "One");
  EXPECT_EQ(chapters[0].startPage, 0);
  EXPECT_EQ(chapters[0].endPage, 2);
  EXPECT_EQ(chapters[1].startPage, 3);
  EXPECT_EQ(chapters[1].endPage, 5);
  EXPECT_EQ(chapters[2].startPage, 6);
  EXPECT_EQ(chapters[2].endPage, 8);
  EXPECT_EQ(chapters[3].name.size(), 80u);
  EXPECT_EQ(chapters[3].startPage, 9);
  EXPECT_EQ(chapters[3].endPage, 9);

  // The zero terminator ends the table; the two garbage records behind it are
  // never read (reads stop after the terminator record at 56 + 5*96).
  EXPECT_LE(Storage.readEndReached("normal.xtc"), 56 + 5 * kChapterRecord);
  // Whole-book page info stays available alongside the chapters.
  xtc::PageInfo info;
  EXPECT_TRUE(parser.getPageInfo(0, info));
  EXPECT_EQ(info.width, 480);
}

TEST(XtcChapters, ZeroChapterFlagAndOffsetsYieldNoChapters) {
  Storage.reset();
  XtcImage noFlag;
  noFlag.header(4, 0, 56, 56);
  noFlag.addPageTable(4);
  install("noflag.xtc", noFlag);
  auto& parser = openParser("noflag.xtc");
  EXPECT_FALSE(parser.hasChapters());
  EXPECT_TRUE(parser.getChapters().empty());

  XtcImage zeroOffset;
  zeroOffset.header(4, 1, 0, 56);
  zeroOffset.addPageTable(4);
  install("zerooffset.xtc", zeroOffset);
  parser = openParser("zerooffset.xtc");
  EXPECT_TRUE(parser.getChapters().empty());

  XtcImage belowHeader;
  belowHeader.header(4, 1, 8, 56);
  belowHeader.addPageTable(4);
  install("belowheader.xtc", belowHeader);
  parser = openParser("belowheader.xtc");
  EXPECT_TRUE(parser.getChapters().empty());
  // Both early exits stop after reading the flag byte and the 8-byte offset
  // field; the chapter region is never touched.
  EXPECT_LE(Storage.readEndReached("zerooffset.xtc"), 0x38u);
  EXPECT_LE(Storage.readEndReached("belowheader.xtc"), 0x38u);
}

TEST(XtcChapters, ParsesTableAtTheChapterLimit) {
  Storage.reset();
  XtcImage image;
  image.header(1024, 1, 56, 56 + 1024 * kChapterRecord);
  for (uint16_t i = 0; i < 1024; i++) {
    image.addChapter("C" + std::to_string(i + 1), i + 1, i + 1);
  }
  image.addPageTable(1024);
  install("atlimit.xtc", image);

  auto& parser = openParser("atlimit.xtc");
  const auto& chapters = parser.getChapters();
  ASSERT_EQ(chapters.size(), 1024u);
  EXPECT_EQ(chapters.front().name, "C1");
  EXPECT_EQ(chapters.front().startPage, 0);
  EXPECT_EQ(chapters.back().name, "C1024");
  EXPECT_EQ(chapters.back().endPage, 1023);
}

TEST(XtcChapters, RejectsOverLimitTableBeforeReadingIt) {
  Storage.reset();
  XtcImage image;
  // 1,026 record slots of junk between the header and a valid page table.
  image.header(10, 1, 56, 56 + 1026 * kChapterRecord);
  image.bytes.resize(56 + 1026 * kChapterRecord, 0x5A);
  image.addPageTable(10);
  install("overlimit.xtc", image);

  auto& parser = openParser("overlimit.xtc");
  const auto& chapters = parser.getChapters();
  EXPECT_TRUE(chapters.empty());
  EXPECT_FALSE(parser.hasChapters());

  // The rejection happens before the table seek: only the flag byte and the
  // 8-byte offset field (ending at 0x38) were read, no bulk iteration.
  EXPECT_LE(Storage.readEndReached("overlimit.xtc"), 0x38u);

  // Whole-book progress and page information survive the chapter rejection.
  EXPECT_EQ(parser.getPageCount(), 10);
  xtc::PageInfo info;
  EXPECT_TRUE(parser.getPageInfo(0, info));
  EXPECT_EQ(info.width, 480);
}

TEST(XtcChapters, RejectsInvalidChapterOffsets) {
  Storage.reset();
  XtcImage tailGap;
  tailGap.header(10, 1, 56 + 160 + 50, 56);  // chapter gap is only 50 bytes
  tailGap.addPageTable(10);
  tailGap.bytes.resize(56 + 160 + 50, 0);  // file ends inside the gap
  install("tailgap.xtc", tailGap);
  auto& parser = openParser("tailgap.xtc");
  EXPECT_TRUE(parser.getChapters().empty());
  EXPECT_LE(Storage.readEndReached("tailgap.xtc"), 0x38u);

  XtcImage pastEof;
  pastEof.header(10, 1, 5000, 56);
  pastEof.addPageTable(10);
  install("pasteof.xtc", pastEof);
  parser = openParser("pasteof.xtc");
  EXPECT_TRUE(parser.getChapters().empty());
  EXPECT_LE(Storage.readEndReached("pasteof.xtc"), 0x38u);
  EXPECT_EQ(parser.getPageCount(), 10);
}

TEST(XtcChapters, DropsChaptersOnTruncatedRecordRead) {
  Storage.reset();
  XtcImage image;
  image.header(5, 1, 56 + 80, 56);
  image.addPageTable(5);
  image.addChapter("One", 1, 2);
  image.addChapter("Two", 3, 4);
  image.addChapter("Three", 5, 5);
  install("torn.xtc", image);
  // Simulate a torn card read partway through the second record.
  Storage.setFailReadBeyond("torn.xtc", 56 + 80 + kChapterRecord + 48);

  auto& parser = openParser("torn.xtc");
  EXPECT_TRUE(parser.getChapters().empty());
  EXPECT_FALSE(parser.hasChapters());

  // Reading and page info stay available after the failed chapter load.
  EXPECT_EQ(parser.getPageCount(), 5);
  xtc::PageInfo info;
  EXPECT_TRUE(parser.getPageInfo(0, info));
  EXPECT_EQ(info.width, 480);
}

}  // namespace

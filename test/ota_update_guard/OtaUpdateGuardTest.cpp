#include <gtest/gtest.h>

#include <cstring>
#include <string>

#include "ReleaseJsonParser.h"
#include "network/FirmwareBoardTag.h"

namespace {

bool isValidOtaCandidate(const ReleaseJsonParser& parser) {
  if (!parser.foundTag() || !parser.foundFirmware()) return false;
  if (parser.getFirmwareSize() == 0) return false;
  const char* url = parser.getFirmwareUrl();
  return url != nullptr && std::strncmp(url, "https://", 8) == 0;
}

TEST(OtaUpdateGuard, BoardTagMatchesX4Pro) {
  ASSERT_EQ(board_tag::boardNameLen(), 5u);
  EXPECT_EQ(std::string(board_tag::boardName(), board_tag::boardNameLen()), "x4pro");

  const std::string matching = "prefix\xE9\x00CROSSPOINT-BOARD-V1:x4pro;suffix";
  board_tag::Scanner whole;
  whole.feed(reinterpret_cast<const uint8_t*>(matching.data()), matching.size());
  EXPECT_FALSE(whole.mismatch());

  board_tag::Scanner byteByByte;
  for (char c : matching) {
    byteByByte.feed(reinterpret_cast<const uint8_t*>(&c), 1);
  }
  EXPECT_FALSE(byteByByte.mismatch());
}

TEST(OtaUpdateGuard, BoardTagAllowsUntaggedLegacyImage) {
  const uint8_t payload[] = {0xE9, 0x04, 0x02, 0x10, 'C', 'R', 'O', 'S', 'S', 'P', 'O', 'I', 'N', 'T', 0x00, 0xFF};
  board_tag::Scanner scanner;
  scanner.feed(payload, sizeof(payload));
  EXPECT_FALSE(scanner.mismatch());
}

TEST(OtaUpdateGuard, BoardTagRejectsOtherBoardsAcrossSplits) {
  for (const char* other : {"sticky", "x4", "x4c", "papermono", "metalio_eink4"}) {
    const std::string payload = std::string("hdrCROSSPOINT-BOARD-V1:") + other + ";tail";
    for (size_t split = 0; split <= payload.size(); ++split) {
      board_tag::Scanner scanner;
      if (split > 0) scanner.feed(reinterpret_cast<const uint8_t*>(payload.data()), split);
      if (split < payload.size()) {
        scanner.feed(reinterpret_cast<const uint8_t*>(payload.data() + split), payload.size() - split);
      }
      EXPECT_TRUE(scanner.mismatch()) << "board=" << other << " split=" << split;
      EXPECT_STREQ(scanner.foundName(), other) << "board=" << other << " split=" << split;
    }
  }
}

TEST(OtaUpdateGuard, RejectsInsecureUrlAndZeroSizeBinaryForX4Pro) {
  const char* insecureJson = R"({
    "tag_name": "1.7.0",
    "assets": [
      {"name": "crosspoint-1.7.0-x4.bin", "browser_download_url": "https://example.com/x4.bin", "size": 1000000},
      {"name": "crosspoint-1.7.0-x4pro.bin", "browser_download_url": "http://example.com/x4pro.bin", "size": 1200000}
    ]
  })";
  ReleaseJsonParser insecureParser;
  insecureParser.setFirmwareAssetName("crosspoint-1.7.0-x4pro.bin");
  insecureParser.feed(insecureJson, std::strlen(insecureJson));
  EXPECT_TRUE(insecureParser.foundFirmware());
  EXPECT_FALSE(isValidOtaCandidate(insecureParser));

  const char* zeroSizeJson = R"({
    "tag_name": "1.7.0",
    "assets": [
      {"name": "crosspoint-1.7.0-x4pro.bin", "browser_download_url": "https://example.com/x4pro.bin", "size": 0}
    ]
  })";
  ReleaseJsonParser zeroParser;
  zeroParser.setFirmwareAssetName("crosspoint-1.7.0-x4pro.bin");
  zeroParser.feed(zeroSizeJson, std::strlen(zeroSizeJson));
  EXPECT_TRUE(zeroParser.foundFirmware());
  EXPECT_FALSE(isValidOtaCandidate(zeroParser));

  const char* validJson = R"({
    "tag_name": "1.7.0",
    "assets": [
      {"name": "crosspoint-1.7.0-sticky.bin", "browser_download_url": "https://example.com/sticky.bin", "size": 1200000},
      {"name": "crosspoint-1.7.0-x4pro.bin", "browser_download_url": "https://example.com/x4pro.bin", "size": 1200000}
    ]
  })";
  ReleaseJsonParser validParser;
  validParser.setFirmwareAssetName("crosspoint-1.7.0-x4pro.bin");
  validParser.feed(validJson, std::strlen(validJson));
  EXPECT_TRUE(isValidOtaCandidate(validParser));
  EXPECT_STREQ(validParser.getFirmwareUrl(), "https://example.com/x4pro.bin");
  EXPECT_EQ(validParser.getFirmwareSize(), 1200000u);
}

}  // namespace

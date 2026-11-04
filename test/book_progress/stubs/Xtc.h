#pragma once
#include <string>
#include <vector>
namespace xtc { struct ChapterInfo { uint32_t startPage; uint32_t endPage; std::string name; }; }
class Xtc {
  std::string path; std::vector<xtc::ChapterInfo> chapters;
 public:
  explicit Xtc(std::string p, const char*) : path(std::move(p)) { if (path == "chapters.xtc") chapters = {{0, 2, "One"}, {3, 5, "Two"}}; }
  bool load() const { return path != "bad.xtc"; }
  std::string getCachePath() const { return "/cache/" + path; }
  uint32_t getPageCount() const { return 6; }
  uint8_t calculateProgress(uint32_t p) const { return static_cast<uint8_t>(p * 100 / 6); }
  bool hasChapters() const { return !chapters.empty(); }
  const std::vector<xtc::ChapterInfo>& getChapters() const { return chapters; }
};

#pragma once
#include "HalStorage.h"
#include <string>
struct Toc { std::string title = "Chapter"; };
class Epub {
  std::string path;
 public:
  explicit Epub(std::string p, const char*) : path(std::move(p)) {}
  bool load(bool buildIfMissing, bool) const {
    const bool txtOrMd = path.ends_with(".txt") || path.ends_with(".md");
    if (txtOrMd) {
      const auto source = fake::files.find(path);
      const auto cache = fake::files.find("/cache/" + path + "/valid");
      if (source == fake::files.end() || cache == fake::files.end() || cache->second.size() != source->second.size()) return buildIfMissing;
    }
    return path != "bad.epub";
  }
  std::string getCachePath() const { return "/cache/" + path; }
  int getSpineItemsCount() const { return 3; }
  size_t getBookSize() const { return 300; }
  int getTocIndexForSpineIndex(int i) const { return i; }
  int getTocItemsCount() const { return 3; }
  Toc getTocItem(int) const { return {}; }
  float calculateProgress(int spine, float fraction) const { return (spine + fraction) / 3.0f; }
};

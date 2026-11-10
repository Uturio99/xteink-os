#pragma once

#include <string>
#include <utility>
#include <vector>

class Epub {
 public:
  static inline std::vector<std::pair<bool, bool>> loadRequests;
  explicit Epub(std::string value, const char*) : path(std::move(value)) {}
  bool load(bool buildIfMissing, bool skipLoadingCss) {
    loadRequests.emplace_back(buildIfMissing, skipLoadingCss);
    return path != "bad.epub";
  }
  std::string getCachePath() const { return "/cache/" + path; }
  int getSpineItemsCount() const { return 3; }
  size_t getBookSize() const { return 300; }
  int getTocIndexForSpineIndex(int spine) const { return spine; }
  int getTocItemsCount() const { return 3; }
  float calculateProgress(int spine, float fraction) const { return (spine + fraction) / 3.0f; }

 private:
  std::string path;
};

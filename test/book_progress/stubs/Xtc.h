#pragma once

#include <cstdint>
#include <string>

class Xtc {
 public:
  explicit Xtc(std::string value, const char*) : path(std::move(value)) {}
  bool load() const { return path != "bad.xtc"; }
  std::string getCachePath() const { return "/cache/" + path; }
  uint32_t getPageCount() const { return 6; }
  uint8_t calculateProgress(uint32_t page) const { return static_cast<uint8_t>((page + 1) * 100 / 6); }

 private:
  std::string path;
};

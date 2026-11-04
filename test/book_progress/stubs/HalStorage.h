#pragma once
#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
namespace fake { inline std::map<std::string, std::vector<uint8_t>> files; }
class HalFile {
  std::vector<uint8_t>* bytes = nullptr; size_t pos = 0;
 public:
  explicit operator bool() const { return bytes != nullptr; }
  int read(void* out, size_t n) { if (!bytes) return -1; n = std::min(n, bytes->size() - std::min(pos, bytes->size())); std::copy_n(bytes->data() + std::min(pos, bytes->size()), n, static_cast<uint8_t*>(out)); pos += n; return static_cast<int>(n); }
  friend class HalStorage;
};
class HalStorage {
 public:
  static HalStorage& getInstance() { static HalStorage s; return s; }
  bool openFileForRead(const char*, const std::string& path, HalFile& f) { auto i = fake::files.find(path); if (i == fake::files.end()) return false; f.bytes = &i->second; return true; }
};
#define Storage HalStorage::getInstance()

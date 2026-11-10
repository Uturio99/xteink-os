#pragma once

#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace fake {
inline std::map<std::string, std::vector<uint8_t>> files;
}

class HalFile {
 public:
  int read(void* output, size_t size) {
    if (!bytes) return -1;
    size = std::min(size, bytes->size() - std::min(offset, bytes->size()));
    std::copy_n(bytes->data() + std::min(offset, bytes->size()), size, static_cast<uint8_t*>(output));
    offset += size;
    return static_cast<int>(size);
  }

 private:
  std::vector<uint8_t>* bytes = nullptr;
  size_t offset = 0;
  friend class HalStorage;
};

class HalStorage {
 public:
  static HalStorage& getInstance() {
    static HalStorage storage;
    return storage;
  }
  bool openFileForRead(const char*, const std::string& path, HalFile& file) {
    const auto found = fake::files.find(path);
    if (found == fake::files.end()) return false;
    file.bytes = &found->second;
    return true;
  }
};

#define Storage HalStorage::getInstance()

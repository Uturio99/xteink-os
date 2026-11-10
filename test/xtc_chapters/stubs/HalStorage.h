#pragma once

#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

// Host stub for the SDK's HalStorage/HalFile. Files are in-memory byte
// vectors. Every read() records its end position per path so tests can prove
// how far into a crafted file the parser actually reached, and failReadBeyond
// simulates a torn read at a byte offset.
inline std::map<std::string, size_t>& halReadStats() {
  static std::map<std::string, size_t> stats;
  return stats;
}

class HalFile {
  friend class HalStorage;

 public:
  bool close() {
    if (!hasImpl) {
      return false;
    }
    open = false;
    return true;
  }

  bool isOpen() const { return hasImpl && open; }

  int read(void* dst, size_t len) {
    if (!isOpen() || !data) {
      return 0;
    }
    const size_t end = position + len;
    if (end > data->size() || end > failReadBeyond) {
      record(end);
      return 0;
    }
    std::memcpy(dst, data->data() + position, len);
    record(end);
    position = end;
    return static_cast<int>(len);
  }

  uint64_t fileSize64() { return data ? data->size() : 0; }

  bool seek(size_t pos) { return seekAt(pos); }
  bool seek64(uint64_t pos) { return seekAt(static_cast<size_t>(pos)); }

 private:
  void record(size_t end) {
    auto& stats = halReadStats();
    const auto it = stats.find(path);
    if (it != stats.end() && end > it->second) {
      it->second = end;
    }
  }
  bool seekAt(size_t pos) {
    if (!isOpen() || !data || pos > data->size()) {
      return false;
    }
    position = pos;
    return true;
  }

  bool hasImpl = false;
  bool open = false;
  std::string path;
  const std::vector<uint8_t>* data = nullptr;
  size_t position = 0;
  size_t failReadBeyond = static_cast<size_t>(-1);
};

class HalStorage {
 public:
  static HalStorage& getInstance() {
    static HalStorage instance;
    return instance;
  }

  bool openFileForRead(const char*, const char* path, HalFile& file) {
    const auto it = files.find(path);
    file.hasImpl = true;
    file.open = it != files.end();
    file.path = path;
    file.data = file.open ? &it->second.bytes : nullptr;
    file.position = 0;
    file.failReadBeyond = file.open ? it->second.failReadBeyond : 0;
    halReadStats()[path] = 0;
    return file.open;
  }

  void setFile(const std::string& path, std::vector<uint8_t> bytes) { files[path].bytes = std::move(bytes); }
  void setFailReadBeyond(const std::string& path, size_t offset) { files[path].failReadBeyond = offset; }
  size_t readEndReached(const std::string& path) const {
    const auto& stats = halReadStats();
    const auto it = stats.find(path);
    return it == stats.end() ? 0 : it->second;
  }
  void reset() {
    files.clear();
    halReadStats().clear();
  }

 private:
  struct Entry {
    std::vector<uint8_t> bytes;
    size_t failReadBeyond = static_cast<size_t>(-1);
  };
  std::map<std::string, Entry> files;
};

#define Storage HalStorage::getInstance()

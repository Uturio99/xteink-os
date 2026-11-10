#include "BookProgress.h"

#include <Epub.h>
#include <FsHelpers.h>
#include <HalStorage.h>
#include <Logging.h>
#include <Memory.h>
#include <Xtc.h>

#include <algorithm>

namespace {
uint32_t readLe32(const uint8_t* p) {
  return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}
}  // namespace

namespace {
BookMilestone loadBookMilestoneImpl(const std::string& path, const bool allowTextProgress) {
  BookMilestone result;
  uint8_t data[10]{};
  if (FsHelpers::hasReflowableBookExtension(path)) {
    if (!allowTextProgress && !FsHelpers::hasEpubExtension(path)) return result;
    // Metadata objects exceed the stack budget; only the featured book is loaded, once per entry.
    auto epub = makeUniqueNoThrow<Epub>(path, "/.crosspoint");
    if (!epub) {
      LOG_ERR("HOME", "OOM: progress metadata");
      return result;
    }
    if (!epub->load(false, true)) return result;
    HalFile file;
    if (!Storage.openFileForRead("HOME", epub->getCachePath() + "/progress.bin", file)) return result;
    const int size = file.read(data, sizeof(data));
    if (size != 4 && size != 6 && size != 10) return result;
    const int spine = data[0] | (data[1] << 8);
    const int page = data[2] | (data[3] << 8);
    const int total = size >= 6 ? data[4] | (data[5] << 8) : 0;
    if (epub->getSpineItemsCount() <= 0 || epub->getBookSize() == 0) return result;
    if (spine == epub->getSpineItemsCount()) {
      result.percentage = 100;
      return result;
    }
    if (spine > epub->getSpineItemsCount()) return result;
    if (FsHelpers::hasEpubExtension(path) && page != UINT16_MAX) {
      result.page = page + 1;
      if (total > 0 && page < total) result.pageCount = total;
      const int tocIndex = epub->getTocIndexForSpineIndex(spine);
      if (tocIndex >= 0 && tocIndex < epub->getTocItemsCount()) result.chapterNumber = tocIndex + 1;
    }
    const float fraction = total > 0 && page != UINT16_MAX ? std::clamp(float(page) / total, 0.0f, 1.0f) : 0;
    result.percentage = std::clamp(static_cast<int>(epub->calculateProgress(spine, fraction) * 100 + 0.5f), 0, 100);
    return result;
  }
  if (FsHelpers::hasXtcExtension(path)) {
    auto xtc = makeUniqueNoThrow<Xtc>(path, "/.crosspoint");
    if (!xtc) {
      LOG_ERR("HOME", "OOM: XTC progress metadata");
      return result;
    }
    if (!xtc->load()) return result;
    HalFile file;
    if (!Storage.openFileForRead("HOME", xtc->getCachePath() + "/progress.bin", file) || file.read(data, 4) != 4)
      return result;
    const uint32_t page = readLe32(data);
    if (xtc->getPageCount() == 0) return result;
    result.percentage = page >= xtc->getPageCount() ? 100 : xtc->calculateProgress(page);
    return result;
  }
  return result;
}
}  // namespace

int loadBookProgress(const std::string& path) { return loadBookMilestoneImpl(path, true).percentage; }

BookMilestone loadBookMilestone(const std::string& path) { return loadBookMilestoneImpl(path, false); }

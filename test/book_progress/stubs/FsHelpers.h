#pragma once

#include <string_view>

namespace FsHelpers {
inline bool hasEpubExtension(std::string_view path) { return path.ends_with(".epub"); }
inline bool hasTxtExtension(std::string_view path) { return path.ends_with(".txt"); }
inline bool hasMarkdownExtension(std::string_view path) { return path.ends_with(".md"); }
inline bool hasReflowableBookExtension(std::string_view path) {
  return hasEpubExtension(path) || hasTxtExtension(path) || hasMarkdownExtension(path);
}
inline bool hasXtcExtension(std::string_view path) { return path.ends_with(".xtc") || path.ends_with(".xtch"); }
}  // namespace FsHelpers

#pragma once
#include <string_view>
namespace FsHelpers {
inline bool hasReflowableBookExtension(std::string_view p) {
  return p.ends_with(".epub") || p.ends_with(".txt") || p.ends_with(".md");
}
inline bool hasXtcExtension(std::string_view p) { return p.ends_with(".xtc") || p.ends_with(".xtch"); }
}  // namespace FsHelpers

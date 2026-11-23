#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>

struct WrappedAtelierLines {
  char lines[4][128]{};
  int count = 0;
  bool truncated = false;
  // Keep the whole-string font choice for every wrapped line.
  bool useFallback = false;
};

using AtelierMeasureFn = int (*)(const char* text, const void* context);

inline int atelierDropLastCodepoint(const char* text, const int len) {
  int lead = len - 1;
  while (lead > 0 && (static_cast<unsigned char>(text[lead]) & 0xC0u) == 0x80u) --lead;
  return lead;
}

// Input is normalized UTF-8; metrics and font ownership stay with the caller.
inline WrappedAtelierLines wrapAtelierLines(const AtelierMeasureFn measure, const void* context, const char* text,
                                           const int maxWidth, const int maxLines) {
  WrappedAtelierLines res{};
  if (!text || text[0] == '\0' || maxLines <= 0) return res;
  const char* p = text;
  while (*p && res.count < maxLines && res.count < 4) {
    while (*p == ' ') ++p;
    if (!*p) break;
    const char* lineStart = p;
    const char* lastFit = p;
    const char* scan = p;
    char probe[128]{};
    while (*scan) {
      const char* nextSpace = scan;
      while (*nextSpace && *nextSpace != ' ') ++nextSpace;
      const size_t candLen = static_cast<size_t>(nextSpace - lineStart);
      if (candLen >= sizeof(probe)) break;
      memcpy(probe, lineStart, candLen);
      probe[candLen] = '\0';
      if (measure(probe, context) <= maxWidth) {
        lastFit = nextSpace;
        if (*nextSpace == ' ') {
          scan = nextSpace + 1;
          continue;
        }
        scan = nextSpace;
        break;
      }
      break;
    }
    if (lastFit == lineStart) {
      const char* wordEnd = lineStart;
      while (*wordEnd && *wordEnd != ' ') ++wordEnd;
      const char* cur = lineStart;
      const char* best = lineStart;
      while (cur < wordEnd) {
        const char* next = cur + 1;
        while (next < wordEnd && (static_cast<unsigned char>(*next) & 0xC0u) == 0x80u) ++next;
        const size_t n = static_cast<size_t>(next - lineStart);
        if (n >= sizeof(probe)) break;
        memcpy(probe, lineStart, n);
        probe[n] = '\0';
        if (measure(probe, context) > maxWidth && best > lineStart) break;
        best = next;
        cur = next;
      }
      lastFit = best > lineStart ? best : wordEnd;
    }
    const size_t outLen = std::min(sizeof(res.lines[0]) - 1, static_cast<size_t>(lastFit - lineStart));
    memcpy(res.lines[res.count], lineStart, outLen);
    res.lines[res.count][outLen] = '\0';
    ++res.count;
    p = lastFit;
  }
  while (*p == ' ') ++p;
  res.truncated = *p != '\0';
  return res;
}

inline void ellipsizeAtelierTail(const AtelierMeasureFn measure, const void* context, WrappedAtelierLines& wrapped,
                                const int maxWidth) {
  static constexpr char ELLIPSIS[] = "\xE2\x80\xA6";
  constexpr size_t ELLIPSIS_LEN = sizeof(ELLIPSIS) - 1;
  if (!wrapped.truncated || wrapped.count <= 0) return;
  char* line = wrapped.lines[wrapped.count - 1];
  int len = static_cast<int>(std::strlen(line));
  while (true) {
    if (static_cast<size_t>(len) + ELLIPSIS_LEN < sizeof(wrapped.lines[0])) {
      memcpy(line + len, ELLIPSIS, sizeof(ELLIPSIS));
      if (maxWidth > 0 && measure(line, context) <= maxWidth) return;
      line[len] = '\0';
    }
    if (len == 0) return;
    len = atelierDropLastCodepoint(line, len);
    line[len] = '\0';
  }
}

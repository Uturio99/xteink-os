#pragma once

#include <string>

struct BookMilestone {
  int percentage = -1;
  int chapterNumber = -1;
  int page = -1;
  int pageCount = -1;
  std::string chapterTitle;
};

// Saved reading percentage, or -1 when unavailable. Never builds reading caches.
int loadBookProgress(const std::string& path);

// Saved reading position and percentage, using existing caches only.
BookMilestone loadBookMilestone(const std::string& path);

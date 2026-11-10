#pragma once

#include <string>

struct BookMilestone {
  int percentage = -1;
  int chapterNumber = -1;
  int page = -1;
  int pageCount = -1;
};

// Saved reading percentage, or -1 when unavailable. Never builds reading caches.
int loadBookProgress(const std::string& path);

// Saved numeric position and percentage for Home. Does not validate or rebuild TXT/MD caches.
BookMilestone loadBookMilestone(const std::string& path);

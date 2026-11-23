"""Reader exit must not re-acquire the render lock (run with python3 test/test_reader_exit_lock.py).

ActivityManager calls Activity::onExit() while its RenderLock is already held,
and renderingMutex is a non-recursive FreeRTOS mutex. An onExit() that takes
another RenderLock therefore self-deadlocks the loop task. This extracts the
production onExit() body and runs it against a model of that contract.
"""

from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "src/activities/reader/EpubReaderActivity.cpp").read_text(encoding="utf-8")
manager = (ROOT / "src/activities/ActivityManager.cpp").read_text(encoding="utf-8")


def function_body(text, signature):
    start = text.index(signature)
    brace = text.index("{", start)
    depth = 0
    for index in range(brace, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return text[start : index + 1]
    raise AssertionError(f"unbalanced braces after {signature!r}")


# Contract: both exit paths take the render lock before destroying the activity.
loop = function_body(manager, "void ActivityManager::loop()")
pop_branch = loop.split("if (pendingAction == PendingAction::Pop)", 1)[1].split(
    "} else if (pendingActivity)", 1
)[0]
assert pop_branch.index("RenderLock lock;") < pop_branch.index("exitActivity(lock);")
replace_branch = loop.split("} else if (pendingActivity)", 1)[1].split(
    "} else if (pendingAction == PendingAction::Push)", 1
)[0]
assert replace_branch.index("RenderLock lock;") < replace_branch.index("exitActivity(lock);")

exit_activity = function_body(manager, "void ActivityManager::exitActivity(const RenderLock& lock)")
assert "lock must be held by the caller" in exit_activity
assert exit_activity.index("currentActivity->onExit()") < exit_activity.index("currentActivity.reset()")

on_exit = function_body(source, "void EpubReaderActivity::onExit()")
# Drop comment text so a comment that names RenderLock is not mistaken for an
# acquisition; then look for the actual construct.
on_exit_code = "\n".join(line.split("//", 1)[0] for line in on_exit.splitlines())
# The state must still be invalidated on exit; the caller's lock covers it.
assert "invalidateNextPageCache()" in on_exit_code
assert "ReaderActivity::onExit()" in on_exit_code
assert "RenderLock" not in on_exit_code, (
    "EpubReaderActivity::onExit must not acquire RenderLock: ActivityManager::exitActivity() "
    "already holds renderingMutex, which is non-recursive, so onExit() would self-deadlock"
)

harness = (
    r'''
#include <cstdio>
#include <memory>

// ActivityManager model: renderingMutex is created with xSemaphoreCreateMutex()
// (non-recursive). exitActivity() runs on the loop task with it held and calls
// onExit() on that same task.
static bool mutexHeld = false;
static int mutexOwner = -1;
static constexpr int kLoopTask = 0;
static bool selfDeadlock = false;

struct RenderLock {
  bool isLocked = false;
  RenderLock() {
    if (mutexHeld && mutexOwner == kLoopTask) {
      // xSemaphoreTake() by the holder of a non-recursive mutex blocks forever.
      selfDeadlock = true;
      return;
    }
    mutexHeld = true;
    mutexOwner = kLoopTask;
    isLocked = true;
  }
  ~RenderLock() {
    if (isLocked) {
      mutexHeld = false;
      mutexOwner = -1;
    }
  }
};

struct Page {};

struct ReaderActivity {
  void onExit();
};
void ReaderActivity::onExit() {}

struct EpubReaderActivity : ReaderActivity {
  int cachedNextSpineIndex = -1;
  int cachedNextPageIndex = -1;
  std::unique_ptr<Page> cachedNextPage;
  void invalidateNextPageCache() {
    cachedNextSpineIndex = -1;
    cachedNextPageIndex = -1;
    cachedNextPage.reset();
  }
  void onExit();
};

'''
    + on_exit
    + r'''

int main() {
  EpubReaderActivity activity;
  activity.cachedNextSpineIndex = 3;
  activity.cachedNextPageIndex = 4;
  activity.cachedNextPage = std::make_unique<Page>();

  {
    // ActivityManager::loop(): RenderLock is held when exitActivity() runs onExit().
    RenderLock managerLock;
    activity.onExit();
  }

  if (selfDeadlock) {
    std::puts("FAIL: onExit re-acquired the non-recursive rendering mutex (self-deadlock)");
    return 1;
  }
  if (activity.cachedNextPage || activity.cachedNextSpineIndex != -1 ||
      activity.cachedNextPageIndex != -1) {
    std::puts("FAIL: onExit did not invalidate the lookahead page cache");
    return 1;
  }
  std::puts("reader exit lock check passed");
  return 0;
}
'''
)

with tempfile.TemporaryDirectory() as directory:
    cpp = Path(directory) / "reader_exit_lock.cpp"
    executable = Path(directory) / "reader_exit_lock"
    cpp.write_text(harness, encoding="utf-8")
    subprocess.run(["c++", "-std=c++20", "-Wall", "-Wextra", "-Werror", str(cpp), "-o", str(executable)],
                   check=True)
    subprocess.run([str(executable)], check=True)

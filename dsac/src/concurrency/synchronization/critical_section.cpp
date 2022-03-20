#include <dsac/concurrency/synchronization/critical_section.hpp>

namespace dsac::syncing {
void CriticalSection::Lock() {
  while (locked_.exchange(true)) {
    // see: mesi protocol https://www.scss.tcd.ie/Jeremy.Jones/VivioJS/caches/MESIHelp.htm
    while (locked_.load()) {
    }
  }
}
void CriticalSection::Unlock() {
  locked_.store(false);
}
}  // namespace dsac::synchronization
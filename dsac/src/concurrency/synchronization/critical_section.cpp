#include <dsac/concurrency/synchronization/critical_section.hpp>

namespace dsac {

void critical_section::lock() {
  while (locked_.exchange(true)) {
    // see: mesi protocol
    // https://www.scss.tcd.ie/Jeremy.Jones/VivioJS/caches/MESIHelp.htm
    while (locked_.load()) {
    }
  }
}
void critical_section::unlock() {
  locked_.store(false);
}

}  // namespace dsac

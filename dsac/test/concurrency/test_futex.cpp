#include "catch2/catch.hpp"

#include <chrono>
#include <dsac/concurrency/synchronization/futex.hpp>
#include <memory>
#include <thread>

TEST_CASE("Проверка корректности выполнения Futex", "[futex_work]") {
  using namespace dsac::syncing;

  SECTION("Блокировка и ожидание значения в разделяемом состоянии") {
    Futex           futex;
    std::unique_ptr shared_state = std::make_unique<int>(0);

    std::thread child([&]() {
      futex.Waiting(shared_state.get(), 10);
      REQUIRE(*shared_state == 10);

      *shared_state = 20;
      futex.WakeUp(shared_state.get());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    *shared_state = 10;
    futex.WakeUp(shared_state.get());

    futex.Waiting(shared_state.get(), 20);
    REQUIRE(*shared_state == 20);

    child.join();
  }
}
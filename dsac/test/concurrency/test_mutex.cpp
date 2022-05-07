#include "catch2/catch.hpp"

#include <dsac/concurrency/synchronization/mutex.hpp>
#include <memory>
#include <thread>

TEST_CASE("Проверка корректности выполнения mutex", "[mutex_work]") {
  SECTION("Проверка корректности изменения объекта несколькими потоками") {
    dsac::mutex mutex;

    int         counter = 0;
    std::thread adder([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        dsac::unique_lock guard(mutex);
        counter += 1;
      }
    });

    std::thread subtractor([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        dsac::unique_lock guard(mutex);
        counter -= 1;
      }
    });

    adder.join();
    subtractor.join();

    REQUIRE(counter == 0);
  }
}

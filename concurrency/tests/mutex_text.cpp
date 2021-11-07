#include <catch2/catch.hpp>

#include <concurrency/syncing/mutex.hpp>
#include <thread>
#include <memory>

TEST_CASE("Проверка корректности выполнения Mutex", "[mutex_work]") {
  using namespace algo::syncing;

  SECTION("Проверка корректности изменения объекта несколькими потоками") {
    Mutex mutex;

    int counter = 0;
    std::thread adder([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        mutex.Lock();
        counter += 1;
        mutex.Unlock();
      }
    });

    std::thread subtractor([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        mutex.Lock();
        counter -= 1;
        mutex.Unlock();
      }
    });

    adder.join();
    subtractor.join();

    REQUIRE(counter == 0);
  }
}

#include <catch2/catch.hpp>

#include <concurrency/syncing/critical_section.hpp>
#include <thread>
#include <memory>

TEST_CASE("Проверка корректности выполнения CriticalSection", "[critical_section_work]") {
  using namespace algo::syncing;

  SECTION("Проверка корректности изменения объекта несколькими потоками") {
    CriticalSection critical_section;

    int counter = 0;
    std::thread adder([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        critical_section.Lock();
        counter += 1;
        critical_section.Unlock();
      }
    });

    std::thread subtractor([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        critical_section.Lock();
        counter -= 1;
        critical_section.Unlock();
      }
    });

    adder.join();
    subtractor.join();

    REQUIRE(counter == 0);
  }
}

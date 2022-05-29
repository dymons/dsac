#include "catch2/catch.hpp"

#include <dsac/concurrency/synchronization/critical_section.hpp>
#include <thread>

TEST_CASE(
    "Проверка корректности выполнения critical_section", "[critical_section]") {
   SECTION("Проверка корректности изменения объекта несколькими потоками") {
    dsac::critical_section critical_section;

    int         counter = 0;
    std::thread adder([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        critical_section.lock();
        counter += 1;
        critical_section.unlock();
      }
    });

    std::thread subtractor([&]() {
      for (int i{}; i < 1'000'000; ++i) {
        critical_section.lock();
        counter -= 1;
        critical_section.unlock();
      }
    });

    adder.join();
    subtractor.join();

    REQUIRE(counter == 0);
  }
}

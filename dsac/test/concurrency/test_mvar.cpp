#include "catch2/catch.hpp"

#include <dsac/concurrency/synchronization/mvar.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

TEST_CASE(
    "Проверка корректности выполнения примитива синхронизации MVar", "[mvar]") {
  using namespace dsac::syncing;

  static_assert(!std::is_copy_constructible<MVar<int>>::value);
  static_assert(!std::is_move_constructible<MVar<int>>::value);

  SECTION("Создание пустого объекта по умолчанию") {
    MVar<int> mvar;
    REQUIRE(mvar.IsEmpty());
  }
  SECTION("Создание объекта MVar с исходным значением") {
    MVar<int> mvar{10};
    REQUIRE_FALSE(mvar.IsEmpty());
    REQUIRE(mvar.ReadOnly() == 10);
  }
  SECTION("Проверка ожидания значения в MVar на методе Take()") {
    MVar<int> mvar;

    std::thread([&mvar] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      mvar.Put(10);
    }).detach();

    REQUIRE(mvar.IsEmpty());
    REQUIRE(mvar.Take() == 10);
    REQUIRE(mvar.IsEmpty());
  }
}

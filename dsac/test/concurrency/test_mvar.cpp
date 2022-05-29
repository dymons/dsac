#include <catch2/catch.hpp>

#include <dsac/concurrency/synchronization/mvar.hpp>

#include <chrono>
#include <thread>

TEST_CASE(
    "Проверка корректности выполнения примитива синхронизации mvar", "[mvar]") {
  static_assert(!std::is_copy_constructible<dsac::mvar<int>>::value);
  static_assert(!std::is_move_constructible<dsac::mvar<int>>::value);

  SECTION("Создание пустого объекта по умолчанию") {
    dsac::mvar<int> mvar;
    REQUIRE(mvar.is_empty());
  }
  SECTION("Создание объекта mvar с исходным значением") {
    dsac::mvar<int> mvar{10};
    REQUIRE_FALSE(mvar.is_empty());
    REQUIRE(mvar.read_only() == 10);
  }
  SECTION("Проверка ожидания значения в mvar на методе take()") {
    dsac::mvar<int> mvar;

    std::thread([&mvar] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      mvar.put(10);
    }).detach();

    REQUIRE(mvar.is_empty());
    REQUIRE(mvar.take() == 10);
    REQUIRE(mvar.is_empty());
  }
}

#include <catch2/catch.hpp>

#include <concurrency/futures/try.hpp>

TEST_CASE("Проверка корректности примитива Try", "[try]") {
  using namespace algo::concurrency;

  SECTION("Проверка корректности хранения значений в примитиве Try") {
    Try<int> try_value{10};
    REQUIRE(try_value.HasValue());
    REQUIRE_FALSE(try_value.HasException());
    REQUIRE(try_value.Get() == 10);
  }

  SECTION("Проверка корректности хранения исключений в примитиве Try") {
    std::exception_ptr exception;
    try {
      throw std::logic_error{""};
    } catch (...) {
      exception = std::current_exception();
    }

    Try<int> try_exception{std::move(exception)};
    REQUIRE_FALSE(try_exception.HasValue());
    REQUIRE(try_exception.HasException());
  }
}

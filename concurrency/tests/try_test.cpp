#include <catch2/catch.hpp>

#include <concurrency/futures/try.hpp>

TEST_CASE("Проверка корректности примитива Try", "[try]") {
  using namespace algo::futures;

  SECTION("Проверка корректности хранения значений в примитиве Try") {
    Try<int> try_value{10};
    REQUIRE(try_value.HasValue());
    REQUIRE_FALSE(try_value.HasException());
    REQUIRE(try_value.ValueOrThrow() == 10);
  }

  SECTION("Проверка корректности хранения исключений в примитиве Try") {
    Try<int> try_exception{std::make_exception_ptr(std::logic_error{""})};
    REQUIRE_FALSE(try_exception.HasValue());
    REQUIRE(try_exception.HasException());
  }
}

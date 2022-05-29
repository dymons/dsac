#include "catch2/catch.hpp"

#include <dsac/concurrency/futures/result.hpp>

TEST_CASE("Проверка корректности примитива result", "[try]") {
  SECTION("Проверка корректности хранения значений в примитиве result") {
    dsac::result<int> try_value{10};
    REQUIRE(try_value.has_value());
    REQUIRE_FALSE(try_value.has_exception());
    REQUIRE(try_value.value_or_throw() == 10);
  }

  SECTION("Проверка корректности хранения исключений в примитиве result") {
    dsac::result<int> try_exception{std::make_exception_ptr(std::logic_error{""})};
    REQUIRE_FALSE(try_exception.has_value());
    REQUIRE(try_exception.has_exception());
  }
}

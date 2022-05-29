#include <catch2/catch.hpp>

#include <dsac/concurrency/futures/result.hpp>

TEST_CASE("Testcases for checking correct storing user-defined data or exception", "[result]") {
  SECTION("Store user-defined data") {
    dsac::result<int> result{10};
    REQUIRE(result.has_value());
    REQUIRE_FALSE(result.has_exception());
    REQUIRE(result.value_or_throw() == 10);
  }
  SECTION("Store exception") {
    dsac::result<int> try_exception{std::make_exception_ptr(std::logic_error{""})};
    REQUIRE_FALSE(try_exception.has_value());
    REQUIRE(try_exception.has_exception());
  }
}

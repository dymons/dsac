#include <catch2/catch.hpp>

#include <dsac/concurrency/fibers/go.hpp>

using dsac::go;

TEST_CASE("Execution of entry routine in the scheduler", "[fibers][default]") {
  auto executed = bool{};

  // clang-format off
  go([&] -> void {
    executed = true;
  });
  // clang-format on

  REQUIRE(executed);
}

TEST_CASE("Execution of the child routine in the scheduler", "[fibers][default]") {
  auto executed = bool{};

  // clang-format off
  go([&] -> void {
    go([&] -> void {
      executed = true;
    });
  });
  // clang-format on

  REQUIRE(executed);
}

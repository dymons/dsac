#include <catch2/catch.hpp>

#include <dsac/concurrency/fibers/go.hpp>

using dsac::go;

TEST_CASE("Fiber scheduler", "[fibers]") {
  auto executed = bool{};

  go([&] -> void { executed = true; });

  REQUIRE(executed);
}

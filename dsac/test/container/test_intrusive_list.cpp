#include <catch2/catch.hpp>

#include <dsac/container/intrusive/list.hpp>

namespace {

class fiber : dsac::intrusive::list_node_base<fiber> {};

}  // namespace

TEST_CASE("Intrusive list", "[intrusive][list]") {
  dsac::intrusive::list<fiber> fibers;
  REQUIRE(fibers.empty());
}

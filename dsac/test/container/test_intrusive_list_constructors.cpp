#include <catch2/catch.hpp>

#include <dsac/container/intrusive/list.hpp>

namespace {

class fiber final : public dsac::intrusive::list_node_base<fiber> {};

}  // namespace

TEST_CASE(
    "Given empty list when check is list empty then list is empty", "[constructors][intrusive-list][happy-path]"
) {
  auto fibers = dsac::intrusive::list<fiber>{};
  REQUIRE(fibers.empty());
}

TEST_CASE(
    "Given empty list when push a new element to list then list is not empty",
    "[constructors][intrusive-list][happy-path]"
) {
  //  Arrange
  auto fibers = dsac::intrusive::list<fiber>{};
  REQUIRE(fibers.empty());

  SECTION("Store an entry fiber to the intrusive list") {
    //  Act
    auto entry_fiber = fiber{};
    fibers.push_back(&entry_fiber);

    // Assert
    SECTION("Check that the entry fiber was stored to the intrusive list") {
      REQUIRE_FALSE(fibers.empty());
    }
    SECTION("Check that the entry fiber can be obtained from from the intrusive list") {
      auto* pop_entry_fiber = fibers.pop_front();
      REQUIRE(pop_entry_fiber == &entry_fiber);
      REQUIRE(fibers.empty());
    }
  }
}

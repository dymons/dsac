#include <catch2/catch.hpp>

#include <dsac/functional/refinement_type.hpp>

namespace {

template <std::size_t N>
struct should_be_less_then final {
  auto operator()(std::size_t const& value) -> bool {
    return value < N;
  };
};

}  // namespace

TEST_CASE("Refinement type should be constructable", "[refinement_type][default]") {
  using register_value = dsac::refinement_type<std::size_t, struct RegisterValue, should_be_less_then<256z>>;

  SECTION("Construct refinement type by using user constructor") {
    [[maybe_unused]] auto value = register_value{1z};
  }
}

TEST_CASE("Refinement type should throw by refinements", "[refinement_type][default]") {
  using register_value = dsac::refinement_type<std::size_t, struct RegisterValue, should_be_less_then<256z>>;

  SECTION("Refinement type should throw by condition should_be_less_then") {
    REQUIRE_THROWS_AS(register_value{257z}, dsac::runtime_refinement_error);
  }
}

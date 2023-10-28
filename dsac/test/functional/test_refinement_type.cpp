#include <catch2/catch.hpp>

#include <dsac/functional/refinement_type.hpp>

namespace {

template <std::size_t N>
struct should_be_less_then final {
  auto operator()(std::size_t const& value) -> bool {
    return value < N;
  };
};

template <std::size_t N>
struct should_be_greater_then final {
  auto operator()(std::size_t const& value) -> bool {
    return value > N;
  };
};

}  // namespace

TEST_CASE("Refinement type should be constructable", "[refinement_type][default]") {
  SECTION("Construct refinement type with empty refinements") {
    using register_value        = dsac::refinement_type<std::size_t, struct RegisterValue>;
    [[maybe_unused]] auto value = register_value{1z};
  }
  SECTION("Construct refinement type with some refinements") {
    using register_value        = dsac::refinement_type<std::size_t, struct RegisterValue, should_be_less_then<256z>>;
    [[maybe_unused]] auto value = register_value{1z};
  }
}

TEST_CASE("Refinement type should throw by refinements", "[refinement_type][default]") {
  // clang-format off
  using register_value =
      dsac::refinement_type<
          std::size_t,
          struct RegisterValue,

          // Refinements
          should_be_greater_then<100z>,
          should_be_less_then<256z>
      >;
  // clang-format on

  SECTION("Refinement type should throw by condition should_be_less_then") {
    REQUIRE_THROWS_AS(register_value{257z}, dsac::runtime_refinement_error);
  }
  SECTION("Refinement type should throw by condition should_be_greater_then") {
    REQUIRE_THROWS_AS(register_value{99z}, dsac::runtime_refinement_error);
  }
}

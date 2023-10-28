#include <catch2/catch.hpp>

#include <dsac/functional/refinement_type.hpp>

namespace {

template <int N>
struct should_be_less_then final {
  auto operator()(const int value) noexcept -> bool {
    return value < N;
  };
};

template <int N>
struct should_be_greater_then final {
  auto operator()(const int value) noexcept -> bool {
    return value > N;
  };
};

struct should_be_positive final {
  auto operator()(const int value) noexcept -> bool {
    return value >= 0;
  };
};

}  // namespace

TEST_CASE("Testcases are checked constructable refinement type", "[refinement_type][default]") {
  SECTION("Construct refinement type with empty refinements") {
    using register_value        = dsac::refinement_type<int, struct RegisterValue>;
    [[maybe_unused]] auto value = register_value{1};
  }
  SECTION("Construct refinement type with some refinements") {
    using register_value        = dsac::refinement_type<int, struct RegisterValue, should_be_less_then<256>>;
    [[maybe_unused]] auto value = register_value{1};
  }
}

TEST_CASE("Testcases are checked refinements", "[refinement_type][default]") {
  SECTION("Refinement type should be at the range [100, 256]") {
    // clang-format off
    using register_value =
        dsac::refinement_type<
            int,
            struct RegisterValue,

            // Refinements
            should_be_greater_then<100>,
            should_be_less_then<256>
        >;
    // clang-format on

    SECTION("Refinement type should throw by condition should_be_less_then") {
      REQUIRE_THROWS_AS(register_value{257}, dsac::runtime_refinement_error);
    }
    SECTION("Refinement type should throw by condition should_be_greater_then") {
      REQUIRE_THROWS_AS(register_value{99}, dsac::runtime_refinement_error);
    }
  }

  SECTION("Refinement type should be positive") {
    // clang-format off
    using register_value =
        dsac::refinement_type<
            int,
            struct RegisterValue,

            // Refinements
            should_be_positive
        >;
    // clang-format on

    SECTION("Refinement type should throw by condition should_be_positive") {
      REQUIRE_THROWS_AS(register_value{-1}, dsac::runtime_refinement_error);
    }
  }
}

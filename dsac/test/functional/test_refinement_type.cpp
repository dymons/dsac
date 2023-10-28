#include <catch2/catch.hpp>

#include <dsac/functional/refinement_type.hpp>

TEST_CASE("Refinement type should be constructable", "[refinement_type][default]") {
  using register_value = dsac::refinement_type<std::int32_t, struct RegisterValue>;
}

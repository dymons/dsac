#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

namespace {

auto const kEmptySnapshots = dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>>{};

}  // namespace

TEST_CASE("Construct cluster using hydrate method with empty snapshots", "[cluster][hydrate-constructor]") {
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(kEmptySnapshots);

  // We assume that if there are no snapshots of the system, then the system is in a consistent state.
  REQUIRE(cluster.is_consistent());
}

TEST_CASE("Construct cluster with inconsistent snapshots", "[cluster][hydrate-constructor]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>> snapshots{
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})},
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})},
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})}};
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(snapshots);

  // If there is not a single current value on the cluster, then we assume that the cluster is in a consistent state.
  REQUIRE(cluster.is_consistent());
  REQUIRE_THROWS_AS((void)cluster.get_latest_value(), dsac::domain::not_found_latest_value);
  REQUIRE_THROWS_AS((void)cluster.get_latest_timestamp(), dsac::domain::not_found_latest_timestamp);
}

TEST_CASE("Construct cluster with consistent snapshots", "[cluster][hydrate-constructor]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>> snapshots{
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(1, 1UL)},
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(1, 1UL)},
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(1, 1UL)}};
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(snapshots);

  REQUIRE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value() == 1);
  REQUIRE(cluster.get_latest_timestamp() == 1UL);
}

TEST_CASE("Construct cluster with some consistent snapshots", "[cluster][hydrate-constructor]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>> snapshots{
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(1, 1UL)},
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(1, 1UL)},
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})}};
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(snapshots);

  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value() == 1);
  REQUIRE(cluster.get_latest_timestamp() == 1UL);
}
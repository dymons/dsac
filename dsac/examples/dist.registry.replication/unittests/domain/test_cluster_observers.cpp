#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

TEST_CASE("Cluster is consistent, get up-to-date register value", "[cluster][observers]") {
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

TEST_CASE("Cluster is inconsistent, get up-to-date register value", "[cluster][observers]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>> snapshots{
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(1, 1UL)},
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(2, 2UL)},
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(2, 2UL)}};
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(snapshots);

  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value() == 2);
  REQUIRE(cluster.get_latest_timestamp() == 2UL);
}

TEST_CASE("Cluster is inconsistent with exceptions, get up-to-date register value", "[cluster][observers]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>> snapshots{
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(2, 2UL)},
      dsac::result<dsac::domain::register_value_object>{
          dsac::domain::register_value_object::register_value_object(2, 2UL)},
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})}};
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(snapshots);

  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value() == 2);
  REQUIRE(cluster.get_latest_timestamp() == 2UL);
}

TEST_CASE("Cluster is consistent with only exceptions, do not get up-to-date register value", "[cluster][observers]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_value_object>> snapshots{
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})},
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})},
      dsac::result<dsac::domain::register_value_object>{std::make_exception_ptr(std::logic_error{""})}};
  dsac::domain::cluster_value_object cluster = dsac::domain::cluster_value_object::hydrate(snapshots);

  REQUIRE(cluster.is_consistent());
  REQUIRE_THROWS_AS((void)cluster.get_latest_value(), dsac::domain::not_found_latest_value);
  REQUIRE_THROWS_AS((void)cluster.get_latest_timestamp(), dsac::domain::not_found_latest_timestamp);
}
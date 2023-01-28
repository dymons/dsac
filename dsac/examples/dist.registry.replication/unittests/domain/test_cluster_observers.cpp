#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/exception/cluster_exception.hpp>

using dsac::dynamic_array;
using dsac::make_unexpected;
using dsac::domain::cluster_value_object;
using dsac::domain::register_timestamp;
using dsac::domain::register_value;
using dsac::domain::register_value_object;
using dsac::domain::snapshot;
using dsac::domain::snapshot_error;

TEST_CASE("Cluster is consistent, get up-to-date register value", "[cluster][observers]") {
  dynamic_array<snapshot> snapshots{
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}},
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}},
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}}  //
  };

  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);
  REQUIRE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value().get() == 1);
  REQUIRE(cluster.get_latest_timestamp().get() == 1UL);
}

TEST_CASE("Cluster is inconsistent, get up-to-date register value", "[cluster][observers]") {
  dynamic_array<snapshot> snapshots{
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}},
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}},
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}}  //
  };

  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);
  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value().get() == 2);
  REQUIRE(cluster.get_latest_timestamp().get() == 2UL);
}

TEST_CASE("Cluster is inconsistent with exceptions, get up-to-date register value", "[cluster][observers]") {
  dynamic_array<snapshot> snapshots{
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}},
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}},
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)}  //
  };

  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);
  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value().get() == 2);
  REQUIRE(cluster.get_latest_timestamp().get() == 2UL);
}

TEST_CASE("Cluster is consistent with only exceptions, do not get up-to-date register value", "[cluster][observers]") {
  dynamic_array<snapshot> snapshots{
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)},
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)},
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)}  //
  };

  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);
  REQUIRE(cluster.is_consistent());
  REQUIRE_THROWS_AS((void)cluster.get_latest_value(), dsac::domain::not_found_latest_value);
  REQUIRE_THROWS_AS((void)cluster.get_latest_timestamp(), dsac::domain::not_found_latest_timestamp);
}
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

TEST_CASE(
    "Given equivalent snapshots when restore cluster then cluster is consistent",
    "[observers][restore_from_snapshots][happy-path]"
) {
  // Arrange
  dynamic_array<snapshot> snapshots{
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}},
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}},
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}}  //
  };

  // Act
  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);

  // Assert
  REQUIRE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value().get() == 1);
  REQUIRE(cluster.get_latest_timestamp().get() == 1UL);
}

TEST_CASE(
    "Given not equivalent snapshots when restore cluster then cluster is not consistent",
    "[observers][restore_from_snapshots][happy-path]"
) {
  // Arrange
  dynamic_array<snapshot> snapshots{
      snapshot{register_value_object{register_value{1}, register_timestamp{1UL}}},
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}},
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}}  //
  };

  // Act
  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);

  // Assert
  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value().get() == 2);
  REQUIRE(cluster.get_latest_timestamp().get() == 2UL);
}

TEST_CASE(
    "Given not equivalent snapshots with one error when restore cluster then cluster is not consistent",
    "[observers][restore_from_snapshots][happy-path]"
) {
  // Arrange
  dynamic_array<snapshot> snapshots{
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}},
      snapshot{register_value_object{register_value{2}, register_timestamp{2UL}}},
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)}  //
  };

  // Act
  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);

  // Assert
  REQUIRE_FALSE(cluster.is_consistent());
  REQUIRE(cluster.get_latest_value().get() == 2);
  REQUIRE(cluster.get_latest_timestamp().get() == 2UL);
}

TEST_CASE(
    "Given equivalent snapshots with errors when restore cluster then cluster is consistent",
    "[observers][restore_from_snapshots][happy-path]"
) {
  // Arrange
  dynamic_array<snapshot> snapshots{
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)},
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)},
      snapshot{make_unexpected(snapshot_error::snapshot_not_found)}  //
  };

  // Act
  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(snapshots);

  // Assert
  REQUIRE(cluster.is_consistent());
  REQUIRE_THROWS_AS((void)cluster.get_latest_value(), dsac::domain::not_found_latest_value);
  REQUIRE_THROWS_AS((void)cluster.get_latest_timestamp(), dsac::domain::not_found_latest_timestamp);
}
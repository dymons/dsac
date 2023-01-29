#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

namespace {

auto const kEmptySnapshots = dsac::dynamic_array<dsac::domain::snapshot>{};

}  // namespace

using dsac::dynamic_array;
using dsac::make_unexpected;
using dsac::domain::cluster_value_object;
using dsac::domain::register_timestamp;
using dsac::domain::register_value;
using dsac::domain::register_value_object;
using dsac::domain::snapshot;
using dsac::domain::snapshot_error;

TEST_CASE(
    "Given empty snapshots when construct cluster then cluster is constructed",
    "[constructors][restore-from-snapshots][happy-path]"
) {
  // Arrange & Act
  cluster_value_object cluster = cluster_value_object::restore_from_snapshots(kEmptySnapshots);

  // Assert. We assume that if there are no snapshots of the system, then the system is in a consistent state.
  REQUIRE(cluster.is_consistent());
}

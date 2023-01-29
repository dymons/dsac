#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

namespace {

auto const kEmptySnapshots = dsac::dynamic_array<dsac::domain::snapshot>{};

}  // namespace

TEST_CASE(
    "Given empty snapshots when construct cluster then cluster is destroyed",  //
    "[destructor][restore_from_snapshots][happy-path]"
) {
  // Arrange & Act & Assert
  [[maybe_unused]] dsac::domain::cluster_value_object cluster =
      dsac::domain::cluster_value_object::restore_from_snapshots(kEmptySnapshots);
}
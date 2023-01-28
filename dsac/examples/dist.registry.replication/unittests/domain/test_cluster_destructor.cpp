#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

namespace {

auto const kEmptySnapshots = dsac::dynamic_array<dsac::domain::snapshot>{};

}  // namespace

TEST_CASE("Destroy cluster with empty snapshots", "[cluster][destruction]") {
  [[maybe_unused]] dsac::domain::cluster_value_object cluster =
      dsac::domain::cluster_value_object::restore_from_snapshots(kEmptySnapshots);
}
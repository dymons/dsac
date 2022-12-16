#include <catch2/catch.hpp>

#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

TEST_CASE("Construct cluster using hydrate method with empty snapshots", "[cluster][hydrate-constructor]") {
  dsac::dynamic_array<dsac::result<dsac::domain::register_dto>> empty_snapshots;
  dsac::domain::cluster_dto cluster_sto = dsac::domain::cluster_dto::hydrate(empty_snapshots);

  // We assume that if there are no snapshots of the system, then the system is in a consistent state.
  REQUIRE(cluster_sto.is_consistent());
}


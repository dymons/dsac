#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/read_and_write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

namespace dsac::application::command::coordinator {

using write_command         = application::command::coordinator::write_register_command;
using write_command_handler = application::command::coordinator::write_register_command_handler;
using cluster_dto           = domain::cluster_dto;
using register_dto          = domain::register_dto;

auto read_and_write_register_command_handler::handle() const -> std::optional<register_dto> try {
  auto const cluster_snapshot = cluster_dto::make_snapshot(executor_, quorum_policy_);

  if (not cluster_snapshot.is_consistent()) {
    write_command_handler{executor_, quorum_policy_}.handle(
        write_command::hydrate(cluster_snapshot.get_latest_value(), cluster_snapshot.get_latest_timestamp()));
  }

  return register_dto::hydrate(cluster_snapshot.get_latest_value(), cluster_snapshot.get_latest_timestamp());
} catch ([[maybe_unused]] dsac::domain::cluster_exception const& what) {
  return std::nullopt;
}

}  // namespace dsac::application::command::coordinator
#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/read_and_write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/exception/cluster_exception.hpp>

namespace dsac::application::command::coordinator {

using write_command         = application::command::coordinator::write_register_command;
using write_command_handler = application::command::coordinator::write_register_command_handler;

auto read_and_write_register_command_handler::handle() & -> std::optional<domain::register_value_object> try {
  auto const cluster_snapshot = domain::cluster_value_object::restore_from_replicas(replicas_, quorum_policy_);

  if (not cluster_snapshot.is_consistent()) {
    write_command_handler command_handler{replicas_, quorum_policy_};
    command_handler.handle(write_command{
        .new_register_value = domain::register_value_object(
            domain::register_value{cluster_snapshot.get_latest_value()},
            domain::register_timestamp{cluster_snapshot.get_latest_timestamp()}
        )});
  }

  return domain::register_value_object(
      domain::register_value{cluster_snapshot.get_latest_value()},
      domain::register_timestamp{cluster_snapshot.get_latest_timestamp()}
  );
} catch ([[maybe_unused]] dsac::domain::cluster_exception const& what) {
  return std::nullopt;
}

}  // namespace dsac::application::command::coordinator
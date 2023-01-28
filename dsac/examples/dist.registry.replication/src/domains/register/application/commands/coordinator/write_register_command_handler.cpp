#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>

namespace dsac::application::command::coordinator {

auto write_register_command_handler::handle(write_register_command const& command) & -> void {
  domain::cluster_value_object::store_to_replicas(command.new_register_value, replicas_, quorum_policy_);
}

}  // namespace dsac::application::command::coordinator
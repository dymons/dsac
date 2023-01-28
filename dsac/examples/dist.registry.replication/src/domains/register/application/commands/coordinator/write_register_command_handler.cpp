#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac::application::command::coordinator {

using domain::register_value_object;
using presentation::web::register_replica_client;

auto write_register_command_handler::handle(write_register_command const& command) & -> void {
  dynamic_array<future<void*>> responses;
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor_);
    responses.push_back(replica->write(command.new_register_value));
  }

  std::size_t const                    quorum           = quorum_policy_->quorum(responses.size());
  future<dynamic_array<result<void*>>> quorum_future    = first_n(std::move(responses), quorum);
  dynamic_array<result<void*>>         quorum_responses = std::move(quorum_future).get().value_or_throw();
  for (result<void*> const& response : quorum_responses) {
    response.value_or_throw();
  }
}

}  // namespace dsac::application::command::coordinator
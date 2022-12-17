#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/read_and_write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac::application::command::coordinator {

using application::command::coordinator::write_register_command;
using application::command::coordinator::write_register_command_handler;
using domain::register_dto;
using presentation::web::register_replica_client;

namespace {

using dsac::domain::cluster_dto;

auto create_cluster_snapshot(auto&& executor, auto&& quorum_policy) -> cluster_dto {
  dynamic_array<future<register_dto>> responses;
  std::ranges::transform(
      register_replica_client::factory::get_registered_keys(),
      std::back_inserter(responses),
      [&executor](auto&& replica_name) {
        return register_replica_client::factory::construct(replica_name, executor)->read();
      });

  auto const quorum        = quorum_policy->quorum(responses.size());
  auto       quorum_future = first_n(std::move(responses), quorum);

  return cluster_dto::hydrate(std::move(quorum_future).get().value_or_throw());
}

auto make_write_phase_if_need(auto&& cluster_snapshot, auto&& executor, auto&& quorum_policy) -> void {
  if (not cluster_snapshot.is_consistent()) {
    write_register_command write_register_command{};
    write_register_command.value     = cluster_snapshot.get_latest_value();
    write_register_command.timestamp = cluster_snapshot.get_latest_timestamp();

    write_register_command_handler write_register_command_handler{executor, quorum_policy};
    write_register_command_handler.handle(write_register_command);
  }
}

}  // namespace

auto read_and_write_register_command_handler::handle() const -> std::optional<register_dto> try {
  auto const cluster_snapshot = create_cluster_snapshot(executor_, quorum_policy_);

  make_write_phase_if_need(cluster_snapshot, executor_, quorum_policy_);

  return register_dto::hydrate(cluster_snapshot.get_latest_value(), cluster_snapshot.get_latest_timestamp());
} catch ([[maybe_unused]] dsac::domain::cluster_exception const& what) {
  return std::nullopt;
}

}  // namespace dsac::application::command::coordinator
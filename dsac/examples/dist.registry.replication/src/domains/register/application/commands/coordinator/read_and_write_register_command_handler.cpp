#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/read_and_write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/application/commands/coordinator/write_register_command_handler.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac::application::command::coordinator {

using application::command::coordinator::write_register_command;
using application::command::coordinator::write_register_command_handler;
using domain::register_dto;
using presentation::web::register_replica_client;

namespace {

auto create_cluster_snapshot(executor_base_ref const& executor, domain::policy::quorum_policy_ref const& quorum_policy)
    -> dynamic_array<result<register_dto>> {
  dynamic_array<future<register_dto>> responses;
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor);
    responses.push_back(replica->read());
  }

  std::size_t const                           quorum        = quorum_policy->quorum(responses.size());
  future<dynamic_array<result<register_dto>>> quorum_future = first_n(std::move(responses), quorum);

  return std::move(quorum_future).get().value_or_throw();
}

auto choose_latest_snapshot(dynamic_array<result<register_dto>> const& snapshots) -> std::optional<register_dto> {
  if (snapshots.empty()) {
    return std::nullopt;
  }

  auto latest_snapshot = snapshots.begin();
  for (auto begin = std::begin(snapshots), end = std::end(snapshots); begin != end; ++begin) {
    if (*begin > *latest_snapshot || !latest_snapshot->has_value()) {
      latest_snapshot = begin;
    }
  }

  if (!latest_snapshot->has_value()) {
    return std::nullopt;
  }

  return latest_snapshot->value_or_throw();
}

auto make_write_phase_if_need(
    dynamic_array<result<register_dto>> const& snapshots,
    register_dto const&                        latest_snapshot,
    executor_base_ref const&                   executor,
    domain::policy::quorum_policy_ref const&   quorum_policy) -> void {
  auto const is_cluster_consistent = std::ranges::all_of(snapshots, [&latest_snapshot](auto&& snapshot) {
    return snapshot.has_value() && (snapshot.value_or_throw() == latest_snapshot);
  });
  if (not is_cluster_consistent) {
    write_register_command write_register_command{};
    write_register_command.value     = latest_snapshot.get_value();
    write_register_command.timestamp = latest_snapshot.get_timestamp();

    write_register_command_handler write_register_command_handler{executor, quorum_policy};
    write_register_command_handler.handle(write_register_command);
  }
}

}  // namespace

auto read_and_write_register_command_handler::handle() const -> std::optional<register_dto> {
  auto const snapshots       = create_cluster_snapshot(executor_, quorum_policy_);
  auto const latest_snapshot = choose_latest_snapshot(snapshots);
  if (latest_snapshot.has_value()) {
    make_write_phase_if_need(snapshots, latest_snapshot.value(), executor_, quorum_policy_);
  }

  return latest_snapshot;
}

}  // namespace dsac::application::command::coordinator
#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <algorithm>

namespace dsac::domain {

namespace {

using domain::register_value_object;
using presentation::web::register_replica_client;

auto choose_latest_snapshot(dynamic_array<result<register_value_object>> const& snapshots)
    -> std::optional<register_value_object> {
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

}  // namespace

not_found_latest_value::not_found_latest_value()
  : cluster_exception("Couldn't extract the current register value") {
}

not_found_latest_timestamp::not_found_latest_timestamp()
  : cluster_exception("Couldn't extract the current register timestamp") {
}

cluster_value_object cluster_value_object::hydrate(dynamic_array<result<register_value_object>> snapshots) {
  auto latest_snapshot = choose_latest_snapshot(snapshots);
  return {std::move(snapshots), latest_snapshot};
}

auto cluster_value_object::is_consistent() const -> bool {
  if (not latest_snapshot_.has_value()) {
    return true;
  }
  return std::ranges::all_of(snapshots_, [this](auto&& snapshot) {
    return snapshot.has_value() && (snapshot.value_or_throw() == latest_snapshot_);
  });
}

auto cluster_value_object::get_latest_value() const -> std::int32_t {
  if (not latest_snapshot_.has_value()) [[unlikely]] {
    throw not_found_latest_value{};
  }
  return latest_snapshot_->get_value();
}

auto cluster_value_object::get_latest_timestamp() const -> std::size_t {
  if (not latest_snapshot_.has_value()) [[unlikely]] {
    throw not_found_latest_timestamp{};
  }
  return latest_snapshot_->get_timestamp();
}

auto cluster_value_object::make_snapshot(executor_base_ref executor, policy::quorum_policy_ref quorum_policy)
    -> cluster_value_object {
  dynamic_array<future<register_value_object>> responses;
  std::ranges::transform(
      register_replica_client::factory::get_registered_keys(),
      std::back_inserter(responses),
      [&executor](auto&& replica_name) {
        return register_replica_client::factory::construct(replica_name, executor)->read();
      }
  );

  auto const quorum        = quorum_policy->quorum(responses.size());
  auto       quorum_future = first_n(std::move(responses), quorum);

  return cluster_value_object::hydrate(std::move(quorum_future).get().value_or_throw());
}

}  // namespace dsac::domain

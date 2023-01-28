#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/details/cast.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/exception/cluster_exception.hpp>
#include <examples/dist.registry.replication/src/domains/register/presentation/web/register_replica_client.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/concurrency/futures/result.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <algorithm>

namespace dsac::domain {

namespace {

using presentation::web::register_replica_client;

auto get_fresh_snapshot(dynamic_array<snapshot> const& snapshots) -> snapshot {
  if (snapshots.empty()) {
    return snapshot{make_unexpected(snapshot_error::snapshot_not_found)};
  }

  auto fsnapshot(std::begin(snapshots));
  for (auto bsnapshot(std::begin(snapshots)), esnapshot(std::end(snapshots)); bsnapshot != esnapshot; ++bsnapshot) {
    if (not bsnapshot->get().has_value()) {
      continue;
    }
    if (not fsnapshot->get().has_value() || bsnapshot->get()->get_value() > fsnapshot->get()->get_value()) {
      fsnapshot = bsnapshot;
    }
  }

  return snapshot{fsnapshot->get()};
}

}  // namespace

cluster_value_object::cluster_value_object(dynamic_array<snapshot> snapshots)
  : snapshots_(std::move(snapshots))
  , fresh_snapshot_(get_fresh_snapshot(snapshots_)) {
}

auto cluster_value_object::is_consistent() const -> bool {
  if (not fresh_snapshot_.get().has_value()) {
    return true;
  }
  return std::ranges::all_of(snapshots_, [this](snapshot const& snapshot) {
    return snapshot.get() == fresh_snapshot_.get();
  });
}

auto cluster_value_object::get_latest_value() const -> register_value {
  if (not fresh_snapshot_.get().has_value()) [[unlikely]] {
    throw not_found_latest_value{};
  }
  return register_value{fresh_snapshot_.get()->get_value()};
}

auto cluster_value_object::get_latest_timestamp() const -> register_timestamp {
  if (not fresh_snapshot_.get().has_value()) [[unlikely]] {
    throw not_found_latest_timestamp{};
  }
  return register_timestamp{fresh_snapshot_.get()->get_timestamp()};
}

auto cluster_value_object::make_snapshot(executor_base_ref executor, const policy::quorum_policy_ref& quorum_policy)
    -> cluster_value_object {
  dynamic_array<future<register_value_object>> responses;
  std::ranges::transform(
      register_replica_client::factory::get_registered_keys(),
      std::back_inserter(responses),
      [&executor](std::string const& replica_name) {
        return register_replica_client::factory::construct(replica_name, executor)->read();
      }
  );

  auto const quorum        = quorum_policy->quorum(responses.size());
  auto       quorum_future = first_n(std::move(responses), quorum);
  auto const snapshots     = std::move(quorum_future).get().value_or_throw();

  return cluster_value_object(details::cast<dynamic_array<snapshot>>(snapshots));
}

}  // namespace dsac::domain

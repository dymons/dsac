#include <examples/dist.registry.replication/src/domains/register/domain/cluster.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/details/cast.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/exception/cluster_exception.hpp>

#include <dsac/concurrency/futures/combine/first_n.hpp>
#include <dsac/concurrency/futures/result.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <algorithm>

namespace dsac::domain {

namespace {

auto get_fresh_snapshot(dynamic_array<snapshot> const& snapshots) -> snapshot {
  if (snapshots.empty()) {
    return snapshot{make_unexpected(snapshot_error::snapshot_not_found)};
  }

  auto fsnapshot(std::begin(snapshots));
  for (auto bsnapshot(std::begin(snapshots)), esnapshot(std::end(snapshots)); bsnapshot != esnapshot; ++bsnapshot) {
    if (not bsnapshot->get().has_value()) {
      continue;
    }
    if (not fsnapshot->get().has_value() || bsnapshot->get()->get_value().get() > fsnapshot->get()->get_value().get()) {
      fsnapshot = bsnapshot;
    }
  }

  return snapshot{fsnapshot->get()};
}

}  // namespace

cluster_value_object::cluster_value_object(dynamic_array<snapshot> snapshots, snapshot fresh_snapshot_)
  : snapshots_(std::move(snapshots))
  , fresh_snapshot_(fresh_snapshot_) {
}

auto cluster_value_object::restore_from_snapshots(const dynamic_array<snapshot>& snapshots) -> cluster_value_object {
  return {snapshots, get_fresh_snapshot(snapshots)};
}

auto cluster_value_object::restore_from_replicas(
    dynamic_array<replica_ref> const& replicas, policy::quorum_policy_ref const& quorum_policy
) -> cluster_value_object {
  dynamic_array<future<register_value_object>> responses;
  std::ranges::transform(replicas, std::back_inserter(responses), [](replica_ref const& replica) {
    return replica->async_read();
  });

  auto const quorum        = quorum_policy->quorum(responses.size());
  auto       quorum_future = first_n(std::move(responses), quorum);
  auto const snapshots     = std::move(quorum_future).get().value_or_throw();

  return cluster_value_object::restore_from_snapshots(details::cast<dynamic_array<snapshot>>(snapshots));
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

}  // namespace dsac::domain

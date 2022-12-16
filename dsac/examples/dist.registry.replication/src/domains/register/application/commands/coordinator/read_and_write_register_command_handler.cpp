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

class cluster_exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class not_found_latest_value final : public cluster_exception {
public:
  not_found_latest_value()
    : cluster_exception("Couldn't extract the current register value") {
  }
};

class not_found_latest_timestamp final : public cluster_exception {
public:
  not_found_latest_timestamp()
    : cluster_exception("Couldn't extract the current register timestamp") {
  }
};

class cluster_container final {
  dynamic_array<result<register_dto>> snapshots_;
  std::optional<register_dto>         latest_snapshot_;

  [[nodiscard]] auto choose_latest_snapshot() const -> std::optional<register_dto> {
    if (snapshots_.empty()) {
      return std::nullopt;
    }

    auto latest_snapshot = snapshots_.begin();
    for (auto begin = std::begin(snapshots_), end = std::end(snapshots_); begin != end; ++begin) {
      if (*begin > *latest_snapshot || !latest_snapshot->has_value()) {
        latest_snapshot = begin;
      }
    }

    if (!latest_snapshot->has_value()) {
      return std::nullopt;
    }

    return latest_snapshot->value_or_throw();
  }

public:
  /*implicit*/ cluster_container(dynamic_array<result<register_dto>> snapshots)  // NOLINT(google-explicit-constructor)
    : snapshots_(std::move(snapshots))
    , latest_snapshot_(choose_latest_snapshot()) {
  }

  [[nodiscard]] auto has_latest_snapshot() const -> bool try { return latest_snapshot_.has_value(); } catch (...) {
    return false;
  }

  [[nodiscard]] auto is_consistent() const -> bool {
    return std::ranges::all_of(snapshots_, [this](auto&& snapshot) {
      return snapshot.has_value() && (snapshot.value_or_throw() == latest_snapshot_);
    });
  }

  [[nodiscard]] auto get_latest_value() const -> std::int32_t {
    if (not latest_snapshot_.has_value()) [[unlikely]] {
      throw not_found_latest_value{};
    }
    return latest_snapshot_->get_value();
  }

  [[nodiscard]] auto get_latest_timestamp() const -> std::size_t {
    if (not latest_snapshot_.has_value()) [[unlikely]] {
      throw not_found_latest_timestamp{};
    }
    return latest_snapshot_->get_timestamp();
  }
};

auto create_cluster_snapshot(auto&& executor, auto&& quorum_policy) -> cluster_container {
  dynamic_array<future<register_dto>> responses;
  for (std::string const& key : register_replica_client::factory::get_registered_keys()) {
    std::unique_ptr<register_replica_client> replica = register_replica_client::factory::construct(key, executor);
    responses.push_back(replica->read());
  }

  auto const quorum        = quorum_policy->quorum(responses.size());
  auto       quorum_future = first_n(std::move(responses), quorum);

  return std::move(quorum_future).get().value_or_throw();
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
  if (not cluster_snapshot.has_latest_snapshot()) {
    return std::nullopt;
  }

  make_write_phase_if_need(cluster_snapshot, executor_, quorum_policy_);

  return register_dto::hydrate(cluster_snapshot.get_latest_value(), cluster_snapshot.get_latest_timestamp());
} catch ([[maybe_unused]] cluster_exception const& what) {
  return std::nullopt;
}

}  // namespace dsac::application::command::coordinator
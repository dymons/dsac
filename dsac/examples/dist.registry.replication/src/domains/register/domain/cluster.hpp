#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/concurrency/futures/result.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <optional>

namespace dsac::domain {

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

class cluster_dto final {
  dynamic_array<result<register_dto>> snapshots_;
  std::optional<register_dto>         latest_snapshot_;

  cluster_dto(dynamic_array<result<register_dto>> snapshots, std::optional<register_dto> latest_snapshot)
    : snapshots_(std::move(snapshots))
    , latest_snapshot_(latest_snapshot) {
  }

public:
  cluster_dto(const cluster_dto&)                = default;
  cluster_dto(cluster_dto&&) noexcept            = delete;
  cluster_dto& operator=(const cluster_dto&)     = default;
  cluster_dto& operator=(cluster_dto&&) noexcept = delete;
  ~cluster_dto()                                 = default;

  static cluster_dto hydrate(dynamic_array<result<register_dto>> snapshots);

  [[nodiscard]] auto has_latest_snapshot() const -> bool;
  [[nodiscard]] auto is_consistent() const -> bool;
  [[nodiscard]] auto get_latest_value() const -> std::int32_t;
  [[nodiscard]] auto get_latest_timestamp() const -> std::size_t;
};

}  // namespace dsac::domain
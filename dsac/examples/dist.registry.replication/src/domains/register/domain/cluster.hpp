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
  not_found_latest_value();
};

class not_found_latest_timestamp final : public cluster_exception {
public:
  not_found_latest_timestamp();
};

class cluster_dto final {
  // Constructors

  /*!
    \brief
        User constructor in a private scope. Used to create from the hydrate method.
  */
  cluster_dto(dynamic_array<result<register_dto>> snapshots, std::optional<register_dto> latest_snapshot)
    : snapshots_(std::move(snapshots))
    , latest_snapshot_(latest_snapshot) {
  }

public:
  // Constructors

  /*!
    \brief
        Copy constructor.
  */
  cluster_dto(const cluster_dto&) = default;

  /*!
    \brief
        Move constructor.
  */
  cluster_dto(cluster_dto&&) noexcept = delete;

  /*!
    \brief
        A hydrate method to create an Cluster Data Transfer Object from the snapshots data.

    \param snapshots
        Snapshots of the cluster. A snapshot represents the current state of a cluster node at a given time

    \ingroup
        RegistryReplicationDomain

    \code
        dynamic_array<result<register_dto>> snapshots;
        cluster_dto cluster = cluster_dto::hydrate(snapshots);
    \endcode
  */
  static cluster_dto hydrate(dynamic_array<result<register_dto>> snapshots);

  // Destructor

  /*!
    \brief
        Destructor.
  */
  ~cluster_dto() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  cluster_dto& operator=(const cluster_dto&) = default;

  /*!
    \brief
        Move conversion constructor.
  */
  cluster_dto& operator=(cluster_dto&&) noexcept = delete;

  // Observers

  /*!
    \brief
        Returns true if the cluster is consistent. All snapshots of the system have the same register value.
        Returns true if the cluster do not have at least one a valid snapshot.
  */
  [[nodiscard]] auto is_consistent() const -> bool;

  /*!
    \brief
        Returns the current up-to-date register value on the cluster.
  */
  [[nodiscard]] auto get_latest_value() const -> std::int32_t;

  /*!
    \brief
        Returns the current up-to-date register timestamp on the cluster.
  */
  [[nodiscard]] auto get_latest_timestamp() const -> std::size_t;

private:
  /// Snapshots of a cluster at a given time
  dynamic_array<result<register_dto>> snapshots_;

  /// The current up-to-date register snapshots
  std::optional<register_dto> latest_snapshot_;
};

}  // namespace dsac::domain

#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/concurrency/executors/executor.hpp>
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

class cluster_value_object final {
  // Constructors

  /*!
    \brief
        User constructor in a private scope. Used to create from the hydrate method.
  */
  cluster_value_object(dynamic_array<result<register_value_object>> snapshots, std::optional<register_value_object> latest_snapshot)
    : snapshots_(std::move(snapshots))
    , latest_snapshot_(latest_snapshot) {
  }

public:
  /*!
    \brief
        A hydrate method to create Cluster Data Transfer Object from snapshots data.

    \param snapshots
        Snapshots of the cluster. A snapshot represents the current state of a cluster node at a given time

    \ingroup
        RegistryReplicationDomain

    \code
        dynamic_array<result<register_value_object>> snapshots;
        cluster_value_object cluster = cluster_value_object::hydrate(snapshots);
    \endcode
  */
  static auto hydrate(dynamic_array<result<register_value_object>> snapshots) -> cluster_value_object;

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

    \throw dsac::domain::not_found_latest_value
        Cluster has a non-consistent state, it was not possible to extract the current up-to-date register value
  */
  [[nodiscard]] auto get_latest_value() const -> std::int32_t;

  /*!
    \brief
        Returns the current up-to-date register timestamp on the cluster.

    \throw dsac::domain::not_found_latest_timestamp
        Cluster has a non-consistent state, it was not possible to extract the current up-to-date register timestamp
  */
  [[nodiscard]] auto get_latest_timestamp() const -> std::size_t;

  /*!
    \brief
        Creating a snapshot of the cluster configuration.

    \param executor
        Executor for executing asynchronous operations to replicas

    \param quorum_policy
        Policy for reading values from replicas. Used to optimize the execution of read requests

    \ingroup
        RegistryReplicationDomain

    \code
        cluster_value_object cluster = cluster_value_object::make_snapshot(executor, quorum_policy);
    \endcode
  */
  static auto make_snapshot(executor_base_ref executor, policy::quorum_policy_ref quorum_policy) -> cluster_value_object;

private:
  /// Snapshots of a cluster at a given time
  dynamic_array<result<register_value_object>> snapshots_;

  /// The current up-to-date register snapshots
  std::optional<register_value_object> latest_snapshot_;
};

}  // namespace dsac::domain

#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/shapshot.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac::domain {

class cluster_value_object final {
public:
  // Constructors

  /*!
    \brief
        User constructor in a private scope. Used to create from the hydrate method.
  */
  explicit cluster_value_object(dynamic_array<snapshot> snapshots);

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
  [[nodiscard]] auto get_latest_value() const -> register_value;

  /*!
    \brief
        Returns the current up-to-date register timestamp on the cluster.

    \throw dsac::domain::not_found_latest_timestamp
        Cluster has a non-consistent state, it was not possible to extract the current up-to-date register timestamp
  */
  [[nodiscard]] auto get_latest_timestamp() const -> register_timestamp;

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
  static auto make_snapshot(executor_base_ref executor, const policy::quorum_policy_ref& quorum_policy)
      -> cluster_value_object;

private:
  /// Snapshots of a cluster at a given time
  dynamic_array<snapshot> snapshots_;

  /// The current up-to-date register snapshots
  snapshot fresh_snapshot_;
};

}  // namespace dsac::domain

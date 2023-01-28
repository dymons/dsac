#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/policy/quorum.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/replica.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/shapshot.hpp>

#include <dsac/concurrency/executors/executor.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace dsac::domain {

class cluster_value_object final {
  // Constructors

  /*!
    \brief
        Restore the current state of the cluster.
  */
  cluster_value_object(dynamic_array<snapshot> snapshots, snapshot fresh_snapshot_);

public:
  // Constructors

  /*!
    \brief
        Restore the current state of the cluster based on the snapshots from the system nodes.

    \param snapshots
        A set of snapshots from the system nodes

    \ingroup
        RegistryReplicationDomain
  */
  static auto restore_from_snapshots(const dynamic_array<snapshot>& snapshots) -> cluster_value_object;

  /*!
  \brief
      Restore the current state of the cluster based on the snapshots from the system nodes.

    \param replicas
        A set of replicas from which the current register value will be taken

    \param quorum_policy
        Policy for reading values from replicas. Used to optimize the execution of read requests

    \ingroup
        RegistryReplicationDomain
  */
  static auto restore_from_replicas(
      dynamic_array<replica_ref> const& replicas,
      policy::quorum_policy_ref const&  quorum_policy  //
  ) -> cluster_value_object;

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

private:
  /// Snapshots of a cluster at a given time
  dynamic_array<snapshot> snapshots_;

  /// The current up-to-date register snapshots
  snapshot fresh_snapshot_;
};

}  // namespace dsac::domain

#pragma once

#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>

#include <dsac/functional/expected.hpp>
#include <dsac/functional/strong_type.hpp>

namespace dsac::domain {

/*!
  \brief
      Snapshots may be invalid
*/
enum class snapshot_error {
  snapshot_not_found /* The snapshot value was not initialized */,
};

/*!
  \brief
      Snapshot of the register of one of the nodes in the cluster
*/
using snapshot = strong_type<expected<register_value_object, snapshot_error>, struct Snapshot>;

}  // namespace dsac::domain

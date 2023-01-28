#include <examples/dist.registry.replication/src/domains/register/domain/details/cast.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/register.hpp>
#include <examples/dist.registry.replication/src/domains/register/domain/shapshot.hpp>

#include <dsac/concurrency/futures/result.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <algorithm>

namespace dsac::domain::details {

template <>
[[nodiscard]] auto cast(dynamic_array<result<register_value_object>> const& snapshots) -> dynamic_array<snapshot> {
  dynamic_array<snapshot> cast_snapshots;
  std::ranges::transform(
      snapshots,
      std::back_inserter(cast_snapshots),
      [](result<register_value_object> const& _register) {
        if (not _register.has_value()) {
          return snapshot{make_unexpected(snapshot_error::snapshot_not_found)};
        }
        return snapshot{
            _register.value_or_throw(),
        };
      }
  );

  return cast_snapshots;
}

}  // namespace dsac::domain::details
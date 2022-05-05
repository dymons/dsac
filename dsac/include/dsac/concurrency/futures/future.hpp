#pragma once

#include <dsac/concurrency/futures/detail/future.hpp>

namespace dsac {
template <typename T>
using future = detail::Future<T>;
}  // namespace dsac
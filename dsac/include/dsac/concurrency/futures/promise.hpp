#pragma once

#include <dsac/concurrency/futures/detail/promise.hpp>

namespace dsac {
template <typename T>
using promise = detail::Promise<T>;
}  // namespace dsac
#pragma once

#include <dsac/concurrency/futures/try.hpp>
#include <dsac/functional/function2.hpp>

namespace dsac::futures {
template <typename T>
using Callback = fu2::unique_function<void(Try<T>)>;
}  // namespace dsac::futures

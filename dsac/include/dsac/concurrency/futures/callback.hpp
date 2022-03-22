#pragma once

#include <concurrency/futures/try.hpp>
#include <concurrency/utility/function2.hpp>

namespace dsac::futures {
template <typename T>
using Callback = fu2::unique_function<void(Try<T>)>;
}  // namespace dsac::futures

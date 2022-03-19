#pragma once

#include <concurrency/utility/function2.hpp>
#include <concurrency/futures/try.hpp>

namespace algo::futures {
template <typename T>
using Callback = fu2::unique_function<void(Try<T>)>;
}  // namespace algo::futures

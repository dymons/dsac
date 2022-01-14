#pragma once

#include <concurrency/utility/function2.hpp>

namespace algo::concurrency {
using Task = fu2::unique_function<void()>;
}  // namespace algo::concurrency
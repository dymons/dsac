#pragma once

#include <concurrency/utility/function2.hpp>

namespace algo::concurrency {
using Task = fu2::function<void()>;
}  // namespace algo::concurrency
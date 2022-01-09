#pragma once

#include <functional>

namespace algo::concurrency {
using Task = std::function<void()>;
}  // namespace algo::concurrency
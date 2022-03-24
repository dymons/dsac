#pragma once

#include <dsac/functional/function2.hpp>

namespace dsac::concurrency {
using task = fu2::unique_function<void()>;
}  // namespace dsac::concurrency
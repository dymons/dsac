#pragma once

#include <dsac/util/function2.hpp>

namespace dsac::concurrency {
using Task = fu2::unique_function<void()>;
}  // namespace dsac::concurrency
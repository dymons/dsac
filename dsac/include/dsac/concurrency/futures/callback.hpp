#pragma once

#include <dsac/concurrency/futures/result.hpp>
#include <dsac/functional/function2.hpp>

namespace dsac {
template <typename T>
using callback = fu2::unique_function<void(result<T>)>;
}  // namespace dsac

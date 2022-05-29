#pragma once

#include <dsac/concurrency/futures/result.hpp>
#include <dsac/functional/function2.hpp>

namespace dsac {

/*!
    \brief
        Callback with support creating move-only lambda for execution in the executor environment.
*/
template <typename T>
using callback = fu2::unique_function<void(result<T>)>;

}  // namespace dsac

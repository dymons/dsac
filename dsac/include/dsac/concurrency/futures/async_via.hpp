#pragma once

#include <dsac/concurrency/executors/executor.hpp>

namespace dsac {

/*!
    \brief
        Execute async user-routine in specified executor environment.

    \param executor
        Executor within which the execution will be performed routine
    \param routine
        User-routine for execution

    \return
        Return dsac::future for the future value obtained from routine

    \ingroup
        DsacConcurrency

    \code
        auto executor = dsac::make_static_thread_pool(4U);
        dsac::async_via(executor, [](){ do_stuff(); });
        executor->join();
    \endcode
*/
template <typename F>
auto async_via(shared_ptr<iexecutor> executor, F&& routine);

}  // namespace dsac

#define CONCURRENCY_ASYNC_VIA_HPP
#include <dsac/concurrency/futures/detail/async_via-inl.hpp>
#undef CONCURRENCY_ASYNC_VIA_HPP

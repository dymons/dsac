#pragma once

#include <dsac/concurrency/executors/executor.hpp>

namespace dsac {

/*!
    \brief
        Create a static thread pool.

    \param workers
        Number of worker threads in static thread pool

    \ingroup
        DsacConcurrency

    \code
        auto executor = dsac::make_static_thread_pool(4U);
        executor->submit([](){ do_some_work(); });
        executor->join();
    \endcode
*/
[[nodiscard]] executor_base_ptr make_static_thread_pool(std::size_t workers);

}  // namespace dsac

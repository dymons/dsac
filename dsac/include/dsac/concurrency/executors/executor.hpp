#pragma once

#include <dsac/concurrency/executors/task.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class executor_base {
public:
  // Constructors

  /*!
    \brief
        Default constructor.
  */
  executor_base() = default;

  /*!
    \brief
        Move constructor.
  */
  executor_base(executor_base&&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  executor_base& operator=(executor_base&&) = delete;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  virtual ~executor_base() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  executor_base& operator=(const executor_base&) = delete;

  /*!
    \brief
        Copy constructor.
  */
  executor_base(const executor_base&) = delete;

  // Modifiers

  /*!
    \brief
        Add a new task to complete in the thread pool.
  */
  virtual void submit(task&& task) = 0;

  /*!
    \brief
        Finish processing the current tasks and suspend the execution of the thread pool.
  */
  virtual void join() = 0;
};

/*!
    \brief
        The main pointer to the base class of executor object.
  */
using executor_base_ref = dsac::shared_ptr<executor_base>;

}  // namespace dsac

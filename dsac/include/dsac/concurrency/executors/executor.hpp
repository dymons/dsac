#pragma once

#include <dsac/concurrency/executors/task.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class iexecutor {
public:
  // Constructors

  /*!
    \brief
        Default constructor.
  */
  iexecutor() = default;

  /*!
    \brief
        Move constructor.
  */
  iexecutor(iexecutor&&) = delete;

  /*!
    \brief
        Move conversion constructor.
  */
  iexecutor& operator=(iexecutor&&) = delete;

  // Destructor

  /*!
    \brief
        Destructor.
  */
  virtual ~iexecutor() = default;

  // Assignment

  /*!
    \brief
        Copy conversion constructor.
  */
  iexecutor& operator=(const iexecutor&) = delete;

  /*!
    \brief
        Copy constructor.
  */
  iexecutor(const iexecutor&) = delete;

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

}  // namespace dsac

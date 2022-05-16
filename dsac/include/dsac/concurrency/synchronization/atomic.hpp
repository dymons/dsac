#pragma once

#include <cstdint>

namespace dsac {

enum class memory_order : unsigned char {
  memory_order_relaxed,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
};

template <typename T>
class atomic;

template <>
class atomic<std::int64_t> final {
  volatile std::int64_t cell_{};

public:
  using value_type = std::int64_t;

  void store(value_type value, memory_order order = memory_order::memory_order_seq_cst) noexcept;
  value_type load(memory_order order = memory_order::memory_order_seq_cst) const noexcept;
  value_type exchange(value_type new_value, memory_order order = memory_order::memory_order_seq_cst) noexcept;
};

}  // namespace dsac

#define CONCURRENCY_ATOMIC_HPP
#include <dsac/concurrency/synchronization/detail/atomic-inl.hpp>
#undef CONCURRENCY_ATOMIC_HPP

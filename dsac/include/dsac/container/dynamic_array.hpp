#pragma once

#include <dsac/container/detail/dynamic_array.hpp>
#include <memory>

namespace dsac {

template <typename T, typename Allocator = std::allocator<T>>
using dynamic_array = detail::DynamicArray<T, Allocator>;

template <typename T, typename Allocator = std::allocator<T>>
using vector = dynamic_array<T, Allocator>;

}  // namespace dsac
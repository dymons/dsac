#pragma once

#include <memory>

namespace dsac::detail {

template <typename T, typename Allocator>
class DynamicArray {
  class Iterator;
  class ConstIterator;

public:
  using value_type             = T;
  using allocator_type         = Allocator;
  using size_type              = typename std::allocator_traits<allocator_type>::size_type;
  using difference_type        = typename std::allocator_traits<allocator_type>::difference_type;
  using reference              = value_type&;
  using const_reference        = value_type const&;
  using pointer                = typename std::allocator_traits<allocator_type>::pointer;
  using const_pointer          = typename std::allocator_traits<allocator_type>::const_pointer;
  using iterator               = Iterator;
  using const_iterator         = ConstIterator;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
};

}  // namespace dsac::detail
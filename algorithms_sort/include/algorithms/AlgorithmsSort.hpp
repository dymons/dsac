#pragma once

#ifndef ALGORITHMS_ALGORITHMS_SORT_HPP_
#define ALGORITHMS_ALGORITHMS_SORT_HPP_

#include <iostream>
#include <iterator>
#include <algorithm>

namespace algorithms {
  template<class Iterator>
    void insertion_sort(Iterator t_begin, Iterator t_end)
    {
      // From: https://en.cppreference.com/w/cpp/algorithm/rotate
      for(Iterator itr = t_begin; itr != t_end; ++itr) {
        std::rotate(std::upper_bound(t_begin, itr, *itr), itr, std::next(itr));
      }
    }
} // namespace algorithms
#endif // ALGORITHMS_ALGORITHMS_SORT_HPP_
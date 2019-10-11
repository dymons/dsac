#pragma once

#ifndef ALGORITHMS_ALGORITHMS_SORT_HPP_
#define ALGORITHMS_ALGORITHMS_SORT_HPP_

#include <iterator>

namespace algorithms {
  template<class Iterator>
    void insertion_sort(Iterator t_begin, Iterator t_end)
    {
      if(t_begin != t_end) {
        for(Iterator itr = (t_begin + 1); itr != t_end; ++itr) {
          auto key = *itr;
          Iterator prev = (*std::prev(itr) < key) ? itr : std::prev(itr);
          while(*prev > key) {
            *std::next(prev) = *prev;

            if((prev == t_begin) || (*std::prev(prev) < key)) {
              break;
            }

            prev = std::prev(prev);
          }

          *prev = key;
        }
      }
    }
} // namespace algorithms
#endif // ALGORITHMS_ALGORITHMS_SORT_HPP_
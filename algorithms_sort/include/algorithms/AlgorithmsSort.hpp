#pragma once

#ifndef ALGORITHMS_ALGORITHMS_SORT_HPP_
#define ALGORITHMS_ALGORITHMS_SORT_HPP_

#include <deque>
#include <utility>
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

  template<class Iterator>
    void selection_sort(Iterator t_begin, Iterator t_end)
    {
      for(Iterator itr = t_begin; itr != t_end; ++itr) {
        auto min = std::min_element(itr, t_end);
        std::swap(*itr, *min);
      }
    }

  template<class Iterator>
    void merge_sort(Iterator t_begin, Iterator t_end)
    {
      if(t_begin != t_end) {
        if(t_begin == std::prev(t_end)) {
          return;
        }
      }

      auto merge = [](Iterator first, Iterator middle, Iterator finish) {
        const std::deque<std::remove_reference_t<decltype(*first)>> left {first, middle}, right {middle, finish};

        auto left_it = left.begin(), right_it = right.begin();

        for(Iterator itr = first; itr != finish; ++itr) {
          if(left_it == left.end()) {
            *itr = *right_it++;
          } else if(right_it == right.end()) {
            *itr = *left_it++;
          } else if(*left_it <= *right_it) {
            *itr = *left_it++;
          } else {
            *itr = *right_it++;
          }
        }
      };

      if(t_begin != t_end) {
        Iterator middle = t_begin + static_cast<std::size_t>((std::distance(t_begin, t_end) / 2));
        merge_sort(t_begin, middle);
        merge_sort(middle, t_end);
        merge(t_begin, middle, t_end);
      }
    }
} // namespace algorithms
#endif // ALGORITHMS_ALGORITHMS_SORT_HPP_
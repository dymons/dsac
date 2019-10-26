#pragma once

#ifndef ALGORITHMS_ALGORITHMS_SORT_HPP_
#define ALGORITHMS_ALGORITHMS_SORT_HPP_

#include <cmath>
#include <limits>
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace utility {
  template<class Container, class Type>
    Container generator(Type t_from, Type t_to, Type t_step = 1)
    {
      // Sequence storage container
      Container sequence_of_numbers { };
      sequence_of_numbers.reserve(static_cast<std::size_t>(std::abs(t_to - t_from + 1)) / t_step);

      // For floating point data
      if constexpr (std::is_floating_point_v<Type>) {
        // The reverse sequence
        if((t_to - t_from) < 0) {
          for(Type i = t_from; ; i -= t_step) {
            if(i > t_to) {
              sequence_of_numbers.push_back(static_cast<typename Container::value_type>(i));
            } else {
              if(std::fabs(i - t_to) < std::numeric_limits<Type>::epsilon()) {
                sequence_of_numbers.push_back(i);
              }
              break;
            }
          }
        // The direct sequence
        } else {
          for(Type i = t_from; ; i += t_step) {
            if(i < t_to) {
              sequence_of_numbers.push_back(static_cast<typename Container::value_type>(i));
            } else {
              if(std::fabs(i - t_to) < std::numeric_limits<Type>::epsilon()) {
                sequence_of_numbers.push_back(i);
              }
              break;
            }
          }
        }
      // Integer data type
      } else {
        if((t_to - t_from) < 0) {
          for(Type i = t_from; i >= t_to; i -= t_step) {
            sequence_of_numbers.push_back(static_cast<typename Container::value_type>(i));
          }
        } else {
          for(Type i = t_from; i <= t_to; i += t_step) {
            sequence_of_numbers.push_back(static_cast<typename Container::value_type>(i));
          }
        }
      }

      sequence_of_numbers.shrink_to_fit();

      return sequence_of_numbers;
    }
}

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
      // Recursion ends when the sequence becomes 1
      if(std::distance(t_begin, t_end) < 2) {
        return;
      }

      Iterator middle = t_begin + static_cast<std::size_t>((std::distance(t_begin, t_end) / 2));
      merge_sort(t_begin, middle);
      merge_sort(middle, t_end);
      std::merge(t_begin, middle, middle, t_end, t_begin);
    }

  template<class Iterator>
    void bubble_sort(Iterator t_begin, Iterator t_end)
    {
      for(Iterator itr = t_begin; itr != t_end; ++itr) {
        for(Iterator it = t_begin; it != (t_end - std::distance(t_begin, itr)); ++it) {
          if(*it > *std::next(it)) {
            std::swap(*it, *std::next(it));
          }
        }
      }
    }

  template<class Iterator>
    void quick_sort(Iterator t_begin, Iterator t_end)
    {
      // From: https://ru.cppreference.com/w/cpp/algorithm/partition
      if(std::distance(t_begin, t_end) > 1) {
        Iterator middle = t_begin + static_cast<std::size_t>((std::distance(t_begin, t_end) / 2));
        Iterator left_middle_it  = std::partition(t_begin, t_end, [reference_value = *middle](auto& e){ return e < reference_value; });
        Iterator right_middle_it = std::partition(left_middle_it, t_end, [reference_value = *middle](auto& e){ return !(reference_value < e); });

        quick_sort(t_begin, left_middle_it);
        quick_sort(right_middle_it, t_end);
      }
    }
} // namespace algorithms
#endif // ALGORITHMS_ALGORITHMS_SORT_HPP_
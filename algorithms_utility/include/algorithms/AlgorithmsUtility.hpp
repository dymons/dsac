#pragma once

#ifndef ALGORITHMS_ALGORITHMS_UTILITY_HPP_
#define ALGORITHMS_ALGORITHMS_UTILITY_HPP_

#include <type_traits>

namespace algorithms {
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
  }  // namespace utility
} // namespace algorithms
#endif // ALGORITHMS_ALGORITHMS_UTILITY_HPP_
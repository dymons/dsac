#include "algorithms/AlgorithmsSort.hpp"

#include <vector>

#include <gtest/gtest.h>

class AlgorithmsSortTests : public ::testing::Test { };

TEST_F(AlgorithmsSortTests, insertion_sort)
{
  std::vector<int> example_vec { };

  for(int i = 1000; i > 0; --i) {
    example_vec.push_back(i);
  }

  algorithms::insertion_sort(example_vec.begin(), example_vec.end());

  EXPECT_TRUE(std::is_sorted(example_vec.begin(), example_vec.end()));
}
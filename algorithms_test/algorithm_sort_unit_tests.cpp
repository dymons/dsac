#include "algorithms/AlgorithmsSort.hpp"

#include <iostream>
#include <random>
#include <vector>

#include <gtest/gtest.h>

class AlgorithmsSortTests : public ::testing::Test { };

TEST_F(AlgorithmsSortTests, insertion_sort_full)
{
  std::vector<int> full_reverse_sequence { };

  for(int i = 10000; i > 0; --i) {
    full_reverse_sequence.push_back(i);
  }

  algorithms::insertion_sort(full_reverse_sequence.begin(), full_reverse_sequence.end());
  EXPECT_TRUE(std::is_sorted(full_reverse_sequence.begin(), full_reverse_sequence.end()));
}

TEST_F(AlgorithmsSortTests, insertion_sort_forward)
{
  // From: https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
  std::vector<int> mt19937_sequence { };
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, 9999);
  std::generate_n(std::inserter(mt19937_sequence, mt19937_sequence.end()), 10000, [&](){return distribution(generator);});

  algorithms::insertion_sort(mt19937_sequence.begin(), mt19937_sequence.end());
  EXPECT_TRUE(std::is_sorted(mt19937_sequence.begin(), mt19937_sequence.end()));
  EXPECT_TRUE(!std::is_sorted(mt19937_sequence.rbegin(), mt19937_sequence.rend()));
}

TEST_F(AlgorithmsSortTests, insertion_sort_backward)
{
  // From: https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
  std::vector<int> mt19937_sequence { };
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, 9999);
  std::generate_n(std::inserter(mt19937_sequence, mt19937_sequence.end()), 10000, [&](){return distribution(generator);});

  algorithms::insertion_sort(mt19937_sequence.rbegin(), mt19937_sequence.rend());
  EXPECT_TRUE(!std::is_sorted(mt19937_sequence.begin(), mt19937_sequence.end()));
  EXPECT_TRUE(std::is_sorted(mt19937_sequence.rbegin(), mt19937_sequence.rend()));
}
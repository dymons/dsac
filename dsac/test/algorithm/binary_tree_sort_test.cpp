#include <catch2/catch.hpp>

#include <dsac/algorithm/binary_tree_sort.hpp>
#include <dsac/algorithm/is_sorted.hpp>
#include <vector>
#include "dsac/algorithm/is_sorted/is_sorted_seq.hpp"

TEST_CASE("Testcases are sorted using binary tree sort algorithm", "[binary_tree_sort]")
{
  using testcase = std::vector<int>;
  SECTION("Sorting an empty testcase")
  {
    auto empty_testcase = testcase{};
    dsac::binary_tree_sort(std::begin(empty_testcase), std::end(empty_testcase));
    REQUIRE(dsac::is_sorted(empty_testcase));
  }

  SECTION("Sorting a sorted testcase")
  {
    auto sorted_testcase = testcase{1, 2, 3, 4, 5};
    dsac::binary_tree_sort(std::begin(sorted_testcase), std::end(sorted_testcase));
    REQUIRE(dsac::is_sorted(sorted_testcase));
  }
}

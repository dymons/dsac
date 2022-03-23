#include <catch2/catch.hpp>

#include <vector>

#include <dsac/algorithm/BinaryTreeSort.hpp>
#include <dsac/algorithm/is_sorted.hpp>

TEST_CASE("Testcases are sorted using Binary Tree Sort Algorithm", "[is_sorted]")
{
  using Testcase  = std::vector<int>;
  using Testcases = std::vector<Testcase>;

  Testcases testcases{{}, {1, 2, 3, 4, 5}, {10, 9, 8, 7, 6}, {-5, 10, -3, 2, -8, 15, 4, 0, 0, -5, 4}};  // NOLINT
  for (Testcase& testcase : testcases) {
    dsac::binary_tree_sort(testcase);
    REQUIRE(dsac::is_sorted(testcase.begin(), testcase.end()));
  }
}
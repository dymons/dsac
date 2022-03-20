#include <catch2/catch.hpp>

#include <vector>

#include <dsac/algorithm/BinaryTreeSort.hpp>
#include <dsac/algorithm/IsSorted.hpp>

TEST_CASE("Testcases are sorted using Binary Tree Sort Algorithm", "[sort]")
{
  using Testcase  = std::vector<int>;
  using Testcases = std::vector<Testcase>;

  Testcases testcases{{}, {1, 2, 3, 4, 5}, {10, 9, 8, 7, 6}, {-5, 10, -3, 2, -8, 15, 4, 0, 0, -5, 4}};  // NOLINT
  for (Testcase& testcase : testcases) {
    dsac::sort::BinaryTreeSort(testcase);
    REQUIRE(dsac::is_sorted(testcase.begin(), testcase.end(), std::less<int>{}));
  }
}
#include "catch2/catch.hpp"
#include <vector>
#include <algorithm>

#include <dsac/algorithm/InsertionSort.hpp>

TEST_CASE("Сортировка c использованием алгоритма сортировки вставкой",
          "[insertion_sort]") {
  using Testcase = std::vector<int>;
  using Testcases = std::vector<Testcase>;

  Testcases testcases{{1, 2, 3, 4, 5}, {}, {10, 9, 8, 7, 6}};
  for (Testcase& testcase : testcases) {
    dsac::sort::InsertionSort(testcase);
    REQUIRE(std::is_sorted(testcase.begin(), testcase.end()));
  }
}
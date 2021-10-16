#include <catch2/catch.hpp>
#include <vector>
#include <algorithm>

#include <shaker_sort.hpp>

TEST_CASE("Сортировка c использованием алгоритма сортировки перемешиванием",
          "[shaker_sort]") {
  using Testcase = std::vector<int>;
  using Testcases = std::vector<Testcase>;

  // clang-format off
  Testcases testcases
  {
      {},
      {1, 2, 3, 4, 5},
      {10, 9, 8, 7, 6},
      {-5, 10, -3, 2, -8, 15, 4, 0, 0, -5, 4}
  };
  // clang-format on

  SECTION("Наивная реализация алгоритма Shaker Sort") {
    for (Testcase& testcase : testcases) {
      algo::sort::v1::ShakerSort(testcase);
      REQUIRE(std::is_sorted(testcase.begin(), testcase.end()));
    }
  }

  SECTION("Оптимизированная версия алгоритма Shaker Sort") {
    for (Testcase& testcase : testcases) {
      algo::sort::v2::ShakerSort(testcase);
      REQUIRE(std::is_sorted(testcase.begin(), testcase.end()));
    }
  }
}
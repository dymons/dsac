#include <catch2/catch.hpp>

#include <insertion_sort.hpp>
#include <vector>

TEST_CASE("Сортировка c использованием алгоритма сортировки вставкой",
          "[insertion_sort]") {
  using namespace inter::sort;

  SECTION("Заранее отсортированная последовательность") {
    std::vector<int> source = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const std::vector<int> should = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    InsertionSort(source);
    REQUIRE(source == should);
  }
  SECTION("Пустая последовательность") {
    std::vector<int> source = {};
    const std::vector<int> should = {};
    InsertionSort(source);
    REQUIRE(source == should);
  }
  SECTION("Обратная последовательность") {
    std::vector<int> source = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    const std::vector<int> should = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    InsertionSort(source);
    REQUIRE(source == should);
  }
}
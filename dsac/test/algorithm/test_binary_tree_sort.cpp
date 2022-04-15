#include <catch2/catch.hpp>

#include <dsac/algorithm/binary_tree_sort.hpp>
#include <dsac/algorithm/is_sorted.hpp>
#include <dsac/container/dynamic_array.hpp>

TEST_CASE(
    "Testcases are sorted using binary tree sort (D)",
    "[binary_tree_sort][default]") {
  using testcase = dsac::dynamic_array<int>;

  SECTION("Sorting a testcase with custom policy") {
    auto some_testcase = testcase{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    using binary_tree_policy = dsac::binary_search_tree<int, std::greater<>>;
    dsac::binary_tree_sort(
        begin(some_testcase), end(some_testcase), binary_tree_policy{});
    REQUIRE(dsac::is_sorted(some_testcase, std::greater<>{}));
  }
}
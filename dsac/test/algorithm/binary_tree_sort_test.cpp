#include <catch2/catch.hpp>

#include <dsac/algorithm/binary_tree_sort.hpp>
#include <dsac/algorithm/is_sorted.hpp>
#include <vector>

TEST_CASE("Testcases are sorted using binary tree sort algorithm (D)", "[binary_tree_sort-default]")
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
    SECTION("Using predicate by default")
    {
      dsac::binary_tree_sort(std::begin(sorted_testcase), std::end(sorted_testcase));
      REQUIRE(dsac::is_sorted(sorted_testcase));

      dsac::binary_tree_sort(sorted_testcase);
      REQUIRE(dsac::is_sorted(sorted_testcase));
    }

    SECTION("Using custom predicate")
    {
      dsac::binary_tree_sort(sorted_testcase, std::greater<int>{});
      REQUIRE(dsac::is_sorted(sorted_testcase, std::greater<int>{}));
    }
  }
}

TEST_CASE("Testcases are sorted using binary tree sort algorithm (S)", "[binary_tree_sort-stress]")
{
  constexpr std::size_t kNumberOfIteration = 10'000U;

  std::vector<int>                                         testcase(1000);
  std::random_device                                       dev;
  std::mt19937                                             rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1U, testcase.size());

  for (std::size_t i{}; i < kNumberOfIteration; ++i) {
    std::iota(begin(testcase), end(testcase), dist(rng));
    std::shuffle(begin(testcase), end(testcase), rng);

    dsac::binary_tree_sort(std::begin(testcase), std::end(testcase));
    REQUIRE(dsac::is_sorted(begin(testcase), end(testcase)));
  }
}
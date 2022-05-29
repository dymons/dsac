#include <catch2/catch.hpp>

// step 1. include a new sorting algorithm
#include <dsac/algorithm/binary_tree_sort.hpp>
#include <dsac/algorithm/bubble_sort.hpp>
#include <dsac/algorithm/insertion_sort.hpp>
#include <dsac/algorithm/is_sorted.hpp>
#include <dsac/algorithm/selection_sort.hpp>
#include <dsac/container/dynamic_array.hpp>

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DEFINE_FUNCTOR_FOR(sort_name, sort_function) \
                                                     \
  struct sort_name##Functor {                        \
    template <typename... Args>                      \
    void operator()(Args&&... kwargs) {              \
      sort_function(std::forward<Args>(kwargs)...);  \
    }                                                \
  };

// step 2. define a functor for new sorting algorithm
DEFINE_FUNCTOR_FOR(BubbleSort, dsac::bubble_sort)
DEFINE_FUNCTOR_FOR(InsertionSort, dsac::insertion_sort)
DEFINE_FUNCTOR_FOR(BinaryTreeSort, dsac::binary_tree_sort)
DEFINE_FUNCTOR_FOR(SelectionSort, dsac::selection_sort)
}  // namespace

// step 3. include a new functor for default and stress template test case
// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "Testcases are sorted using different sort algorithm",
    "[sort][default]",
    BubbleSortFunctor,
    InsertionSortFunctor,
    BinaryTreeSortFunctor,
    SelectionSortFunctor) {
  TestType sort_type;

  using testcase = dsac::dynamic_array<int>;
  SECTION("Sorting an empty testcase") {
    auto empty_testcase = testcase{};
    sort_type(std::begin(empty_testcase), std::end(empty_testcase));
    REQUIRE(dsac::is_sorted(empty_testcase));
  }

  SECTION("Sorting a sorted testcase") {
    auto sorted_testcase = testcase{1, 2, 3, 4, 5};
    SECTION("Using predicate by default") {
      sort_type(std::begin(sorted_testcase), std::end(sorted_testcase));
      REQUIRE(dsac::is_sorted(sorted_testcase));

      sort_type(sorted_testcase);
      REQUIRE(dsac::is_sorted(sorted_testcase));
    }

    SECTION("Using custom predicate") {
      sort_type(sorted_testcase, std::greater<int>{});
      REQUIRE(dsac::is_sorted(sorted_testcase, std::greater<int>{}));
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "Testcases are sorted using different sort algorithm under stress tests",
    "[.][sort][stress]",
    BubbleSortFunctor,
    InsertionSortFunctor,
    BinaryTreeSortFunctor,
    SelectionSortFunctor) {
  TestType sort_type;

  constexpr std::size_t kNumberOfIteration = 1000U;

  dsac::dynamic_array<int>                                 testcase(1000);
  std::random_device                                       dev;
  std::mt19937                                             rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1U, testcase.size());

  for (std::size_t i{}; i < kNumberOfIteration; ++i) {
    std::iota(begin(testcase), end(testcase), dist(rng));
    std::shuffle(begin(testcase), end(testcase), rng);

    sort_type(std::begin(testcase), std::end(testcase));
    REQUIRE(dsac::is_sorted(begin(testcase), end(testcase)));
  }
}

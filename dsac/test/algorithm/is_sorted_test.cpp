#include <catch2/catch.hpp>

#include <dsac/algorithm/is_sorted.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <limits>
#include <random>

TEST_CASE("Testcases are checked for the sorting (D)", "[is_sorted][default]")
{
  SECTION("Check an empty testcase")
  {
    using testcase      = dsac::dynamic_array<int>;
    auto empty_testcase = testcase{};
    SECTION("Using predicate by default")
    {
      REQUIRE(dsac::is_sorted(empty_testcase));
      REQUIRE(dsac::is_sorted(empty_testcase.begin(), empty_testcase.end()));
    }
    SECTION("Using custom predicate")
    {
      REQUIRE(dsac::is_sorted(testcase{}, std::greater<int>{}));
      REQUIRE(dsac::is_sorted(empty_testcase, std::greater<int>{}));
      REQUIRE(dsac::is_sorted(empty_testcase.begin(), empty_testcase.end(), std::greater<int>{}));
    }
  }

  SECTION("Check sorted testcases")
  {
    REQUIRE(dsac::is_sorted({1, 2, 3, 4, 5}));
    REQUIRE_FALSE(dsac::is_sorted({1, 2, 3, 4, 5}, std::greater<int>{}));
    REQUIRE(dsac::is_sorted({5, 4, 3, 2, 1}, std::greater<int>{}));
  }

  SECTION("Check not sorted testcases")
  {
    REQUIRE_FALSE(dsac::is_sorted({1, 3, 2, 5, 4}));
    REQUIRE_FALSE(dsac::is_sorted({1, 3, 2, 5, 4}, std::greater<int>{}));
  }

  SECTION("Check sorted testcases with duplicates")
  {
    REQUIRE(dsac::is_sorted({1, 1, 1, 1, 1}));
    REQUIRE(dsac::is_sorted({1, 2, 3, 3, 4}));
    REQUIRE(dsac::is_sorted({5, 4, 4, 3, 2, 1}, std::greater<int>{}));
  }

  SECTION("Check working with boundary values")
  {
    using testcase               = std::vector<int>;
    auto const max_value         = std::numeric_limits<int>::max();
    auto const min_value         = std::numeric_limits<int>::min();
    auto const zuro_value        = 0;
    auto const boundary_testcase = testcase{min_value, zuro_value, max_value};
    REQUIRE(dsac::is_sorted(boundary_testcase));
  }

  SECTION("Check constexpr usage")
  {
    constexpr auto kNumElements = 5U;
    using testcase              = std::array<int, kNumElements>;

    constexpr auto kEmptyTestcase       = testcase{};
    constexpr auto kEmptyTestcaseSorted = dsac::is_sorted(kEmptyTestcase);
    STATIC_REQUIRE(kEmptyTestcaseSorted);

    constexpr auto kSortedTestcase = testcase{1, 2, 3, 4, 5};
    constexpr auto kTestcaseSorted = dsac::is_sorted(begin(kSortedTestcase), end(kSortedTestcase));
    STATIC_REQUIRE(kTestcaseSorted);
  }
}

TEST_CASE("Testcases are checked for the sorting (S)", "[is_sorted][stress]")
{
  constexpr std::size_t kNumberOfIteration = 10'000U;

  dsac::dynamic_array<int>                                 testcase(100);
  std::random_device                                       dev;
  std::mt19937                                             rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1U, testcase.size());

  for (std::size_t i{}; i < kNumberOfIteration; ++i) {
    std::generate(begin(testcase), end(testcase), [cnt = dist(rng)]() mutable { return cnt++; });
    REQUIRE(std::is_sorted(begin(testcase), end(testcase)));
    REQUIRE(dsac::is_sorted(begin(testcase), end(testcase)));
  }
}
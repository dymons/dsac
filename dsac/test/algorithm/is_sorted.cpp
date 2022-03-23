#include <catch2/catch.hpp>

#include <dsac/algorithm/is_sorted.hpp>
#include <vector>

TEST_CASE("Testcases are checked for the sorting", "[is_sorted]")
{
  SECTION("Check an empty testcase")
  {
    using testcase      = std::vector<int>;
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
}
#include <catch2/catch.hpp>

#include <dsac/algorithm/IsSorted.hpp>
#include <vector>

TEST_CASE("Testcases are checked for the sorting", "[is_sorted]")
{
  SECTION("Check an empty testcase")
  {
    using testcase      = std::vector<int>;
    auto empty_testcase = testcase{};
    REQUIRE(dsac::is_sorted(empty_testcase.begin(), empty_testcase.end()));
    REQUIRE(dsac::is_sorted(empty_testcase.begin(), empty_testcase.end(), std::greater<int>{}));
    REQUIRE(dsac::is_sorted(empty_testcase));
    REQUIRE(dsac::is_sorted(empty_testcase, std::greater<int>{}));
    REQUIRE(dsac::is_sorted(testcase{}, std::greater<int>{}));
  }
}
#include <catch2/catch.hpp>

#include <dsac/container/dynamic_array.hpp>

TEST_CASE("Dynamic array should be constructable", "[dynamic_array][default]") {
  SECTION("Construct dynamic array by using default constructor") {
    dsac::dynamic_array<int> default_dynamic_array{};
  }
  SECTION("Construct dynamic array by using copy constructor") {
    dsac::dynamic_array<int> const one_dynamic_array{1, 2, 3, 4, 5};
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    dsac::dynamic_array<int> two_dynamic_array = one_dynamic_array;
    REQUIRE(one_dynamic_array == dsac::dynamic_array<int>{1, 2, 3, 4, 5});
    REQUIRE(one_dynamic_array == two_dynamic_array);
  }
  SECTION("Construct dynamic array by using move constructor") {
    dsac::dynamic_array<int> one_dynamic_array{1, 2, 3, 4, 5};
    dsac::dynamic_array<int> two_dynamic_array = std::move(one_dynamic_array);
    REQUIRE(one_dynamic_array.empty());  // NOLINT(bugprone-use-after-move)
    REQUIRE(two_dynamic_array == dsac::dynamic_array<int>{1, 2, 3, 4, 5});
  }
  SECTION("Construct dynamic array by using copy operator") {
    dsac::dynamic_array<int>       one_dynamic_array{1, 2, 3, 4, 5};
    dsac::dynamic_array<int> const two_dynamic_array{6, 7, 8, 9, 10};
    one_dynamic_array = two_dynamic_array;
    REQUIRE(two_dynamic_array == dsac::dynamic_array<int>{6, 7, 8, 9, 10});
    REQUIRE(one_dynamic_array == two_dynamic_array);
  }
}

TEST_CASE("Dynamic array should expand automatically", "[dynamic_array][default]") {
  dsac::dynamic_array<int> dynamic_array;

  SECTION("Empty dynamic array does not take up memory") {
    REQUIRE(dynamic_array.size() == 0);
    REQUIRE(dynamic_array.capacity() == 0);
  }
  SECTION(
      "Dynamic array should expand automatically when an element is "
      "inserted ") {
    dynamic_array.push_back(1);

    REQUIRE(dynamic_array.size() == 1);
    REQUIRE(dynamic_array.capacity() == 1);
  }
  SECTION("When inserting a dynamic array increases the capacity twice") {
    dynamic_array.push_back(1);
    dynamic_array.push_back(2);

    REQUIRE(dynamic_array.size() == 2);
    REQUIRE(dynamic_array.capacity() == 2);

    dynamic_array.push_back(3);
    REQUIRE(dynamic_array.size() == 3);
    REQUIRE(dynamic_array.capacity() == 4);

    dynamic_array.push_back(4);
    dynamic_array.push_back(5);
    REQUIRE(dynamic_array.size() == 5);
    REQUIRE(dynamic_array.capacity() == 8);
  }
  SECTION("When inserting elements, data is not lost") {
    constexpr int kNumberOfElements = 100;
    for (int i{}; i < kNumberOfElements; ++i) {
      dynamic_array.push_back(i);
    }
    REQUIRE(dynamic_array.size() == kNumberOfElements);
  }
  SECTION("When inserting elements, the insertion order is preserved") {
    constexpr int kNumberOfElements = 100;
    for (int i{}; i < kNumberOfElements; ++i) {
      dynamic_array.push_back(i);
    }
    for (int i{}; i < kNumberOfElements; ++i) {
      REQUIRE(dynamic_array[i] == i);
    }
  }
}

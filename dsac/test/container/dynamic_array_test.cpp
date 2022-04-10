#include <catch2/catch.hpp>

#include <dsac/container/dynamic_array.hpp>

TEST_CASE("Dynamic array should expand automatically", "[dynamic_array][default]")
{
  dsac::dynamic_array<int> dynamic_array;

  SECTION("Empty dynamic array does not take up memory")
  {
    REQUIRE(dynamic_array.size() == 0);
    REQUIRE(dynamic_array.capacity() == 0);
  }

  SECTION("Dynamic array should expand automatically when an element is inserted ")
  {
    dynamic_array.push_back(1);

    REQUIRE(dynamic_array.size() == 1);
    REQUIRE(dynamic_array.capacity() == 1);
  }

  SECTION("When inserting a dynamic array increases the capacity twice")
  {
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

  SECTION("When inserting elements, data is not lost")
  {
    constexpr int kNumberOfElements = 100;
    for (int i{}; i < kNumberOfElements; ++i) {
      dynamic_array.push_back(i);
    }
    REQUIRE(dynamic_array.size() == kNumberOfElements);
  }

  SECTION("When inserting elements, the insertion order is preserved")
  {
    constexpr int kNumberOfElements = 100;
    for (int i{}; i < kNumberOfElements; ++i) {
      dynamic_array.push_back(i);
    }
    for (int i{}; i < kNumberOfElements; ++i) {
      REQUIRE(dynamic_array[i] == i);
    }
  }
}

TEST_CASE("Dynamic array should be copyable", "[dynamic_array][default]")
{
  dsac::dynamic_array<int> dynamic_array;
  for (int i{}; i < 10; ++i) {
    dynamic_array.push_back(i);
  }

  auto copy_dynamic_array = dynamic_array;
  REQUIRE(dynamic_array.size() == 10);
  REQUIRE(copy_dynamic_array == dynamic_array);
}
#include "catch2/catch.hpp"

#include <string>
#include <structures/hashtable/hashtable.hpp>

TEST_CASE("Проверка корректности работы стандартных функция с HashTable", "[hash]") {
  using namespace algo::hashtable;

  SECTION("Проверка корректности выполнения добавления элементов в HashTable") {
    HashTable<int, std::string> hashtable;
    hashtable.Insert(2, "abc");
    hashtable.Insert(3, "def");
    hashtable.Insert(4, "ghi");
    hashtable.Insert(5, "jkl");
    hashtable.Insert(6, "mno");
    hashtable.Insert(7, "pqrs");
    hashtable.Insert(8, "tuv");
    hashtable.Insert(9, "wxyz");

    REQUIRE_FALSE(hashtable.Contains(1));
    REQUIRE_THROWS(hashtable.GetValue(1));

    REQUIRE(hashtable.Contains(2));
    REQUIRE(hashtable.GetValue(2) == "abc");

    REQUIRE(hashtable.Contains(3));
    REQUIRE(hashtable.GetValue(3) == "def");

    REQUIRE(hashtable.Contains(4));
    REQUIRE(hashtable.GetValue(4) == "ghi");

    REQUIRE(hashtable.Contains(5));
    REQUIRE(hashtable.GetValue(5) == "jkl");

    REQUIRE(hashtable.Contains(6));
    REQUIRE(hashtable.GetValue(6) == "mno");

    REQUIRE(hashtable.Contains(7));
    REQUIRE(hashtable.GetValue(7) == "pqrs");

    REQUIRE(hashtable.Contains(8));
    REQUIRE(hashtable.GetValue(8) == "tuv");

    REQUIRE(hashtable.Contains(9));
    REQUIRE(hashtable.GetValue(9) == "wxyz");

    REQUIRE_FALSE(hashtable.Contains(10));
    REQUIRE_THROWS(hashtable.GetValue(10));
  }

  SECTION("Проверка корректности изменения элементов в HashTable") {
    HashTable<int, int> hashtable;
    hashtable.Insert(2, 10);

    REQUIRE(hashtable.Contains(2));
    REQUIRE(hashtable.GetValue(2) == 10);

    hashtable.Insert(2, 15);
    REQUIRE(hashtable.Contains(2));
    REQUIRE(hashtable.GetValue(2) == 15);
  }

  SECTION("Проверка корректности удаления элементов из HashTable") {
    HashTable<int, int> hashtable;
    hashtable.Insert(2, 10);
    hashtable.Insert(3, 20);
    hashtable.Insert(4, 30);

    REQUIRE(hashtable.Contains(3));
    hashtable.Remove(3);
    REQUIRE_FALSE(hashtable.Contains(3));
  }
}
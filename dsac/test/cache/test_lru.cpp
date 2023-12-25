#include <catch2/catch.hpp>

#include <dsac/cache/lru.hpp>

TEST_CASE("[LRU] Testcases are checked constructable LRU cache", "[lru][default]") {
  SECTION("Construct lru cache by using user constructor") {
    auto cache = dsac::lru<int, int>{1z};
  }
}

TEST_CASE("[LRU] Testcases are checked inserting elements", "[lru][default]") {
  // Arrange
  auto cache = dsac::lru<int, int>{1z};

  SECTION("Empty lru cache does not take up memory") {
    // Act & Assert
    REQUIRE(cache.size() == 0z);
  }
  SECTION("Lru cache should expand automatically when an element is inserted") {
    // Act
    cache.put(1, 1);

    // Assert
    REQUIRE(cache.size() == 1z);
  }
}

TEST_CASE("[LRU] Testcases are checked eviction of latest elements", "[lru][default]") {
  // Arrange
  auto cache = dsac::lru<int, int>{1z};

  SECTION("When capacity a lru cache increases the latest element evict") {
    // Act
    cache.put(1, 1);
    cache.put(2, 2);

    // Assert
    REQUIRE(cache.size() == 1z);
    REQUIRE(cache.get(1) == std::nullopt);
    REQUIRE(cache.get(2) == 2);
  }
}

TEST_CASE("[LRU] Testcases are checked update elements", "[lru][default]") {
  // Arrange
  auto cache = dsac::lru<int, int>{2z};

  SECTION("Duplicates are not inserted or updated") {
    // Act
    cache.put(1, 1);
    cache.put(1, 2);

    // Assert
    REQUIRE(cache.size() == 1z);
    REQUIRE(cache.get(1) == 1);
  }
}

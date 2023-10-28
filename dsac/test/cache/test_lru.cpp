#include <catch2/catch.hpp>

#include <dsac/cache/lru.hpp>

TEST_CASE("Testcases are checked constructable LRU cache", "[lru][default]") {
  SECTION("Construct lru cache by using user constructor") {
    auto cache = dsac::lru<int, int>{1z};
  }
  SECTION("Construct lru cache by using copy constructor") {
    // Arrange
    auto this_cache = dsac::lru<int, int>{1z};

    // Act
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto that_cache = this_cache;

    // Assert
    REQUIRE(this_cache.size() == that_cache.size());
    REQUIRE(this_cache.get(1) == std::nullopt);
    REQUIRE(that_cache.get(1) == std::nullopt);
  }
  SECTION("Construct lru cache by using move constructor") {
    // Arrange
    auto this_cache = dsac::lru<int, int>{1z};
    REQUIRE(this_cache.put(1, 1));

    // Act
    auto that_cache = std::move(this_cache);

    // Assert
#pragma clang diagnostic push
#pragma ide diagnostic   ignored "clang-analyzer-cplusplus.Move"
    REQUIRE(this_cache.size() == 0z);
#pragma clang diagnostic pop
    REQUIRE(that_cache.size() == 1z);
    REQUIRE(this_cache.get(1) == std::nullopt);
    REQUIRE(that_cache.get(1) == 1);
  }
}

TEST_CASE("Testcases are checked inserting elements", "[lru][default]") {
  // Arrange
  auto cache = dsac::lru<int, int>{1z};

  SECTION("Empty lru cache does not take up memory") {
    // Act & Assert
    REQUIRE(cache.size() == 0z);
  }
  SECTION("Lru cache should expand automatically when an element is inserted") {
    // Act
    REQUIRE(cache.put(1, 1));

    // Assert
    REQUIRE(cache.size() == 1z);
  }
}

TEST_CASE("Testcases are checked eviction of latest elements", "[lru][default]") {
  // Arrange
  auto cache = dsac::lru<int, int>{1z};

  SECTION("When capacity a lru cache increases the latest element evict") {
    // Act
    REQUIRE(cache.put(1, 1));
    REQUIRE(cache.put(2, 2));

    // Assert
    REQUIRE(cache.size() == 1z);
    REQUIRE(cache.get(1) == std::nullopt);
    REQUIRE(cache.get(2) == 2);
  }
}

TEST_CASE("Testcases are checked update elements", "[lru][default]") {
  // Arrange
  auto cache = dsac::lru<int, int>{2z};

  SECTION("Duplicates are not inserted or updated") {
    // Act
    REQUIRE(cache.put(1, 1));
    REQUIRE_FALSE(cache.put(1, 2));

    // Assert
    REQUIRE(cache.size() == 1z);
    REQUIRE(cache.get(1) == 1);
  }
}

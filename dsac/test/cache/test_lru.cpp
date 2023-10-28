#include <catch2/catch.hpp>

#include <dsac/cache/lru.hpp>

TEST_CASE("Testcases are checked constructable LRU cache", "[lru][default]") {
  SECTION("Construct lru cache by using user constructor") {
    auto cache = dsac::lru<int, int>{1z};
  }
  SECTION("Construct lru cache by using copy constructor") {
    auto this_cache = dsac::lru<int, int>{1z};

    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    auto that_cache = this_cache;
    REQUIRE(this_cache.size() == that_cache.size());
    REQUIRE(this_cache.get(1) == std::nullopt);
    REQUIRE(that_cache.get(1) == std::nullopt);
  }
  SECTION("Construct lru cache by using move constructor") {
    auto this_cache = dsac::lru<int, int>{1z};
    this_cache.put(1, 1);
    auto that_cache = std::move(this_cache);

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
  auto cache = dsac::lru<int, int>{1z};

  SECTION("Empty lru cache does not take up memory") {
    REQUIRE(cache.size() == 0z);
  }
  SECTION("Lru cache should expand automatically when an element is inserted") {
    cache.put(1, 1);
    REQUIRE(cache.size() == 1z);
  }
}

TEST_CASE("Testcases are checked eviction of latest elements", "[lru][default]") {
  auto cache = dsac::lru<int, int>{1z};

  SECTION("When capacity a lru cache increases the latest element evict") {
    REQUIRE(cache.size() == 0z);
    cache.put(1, 1);
    cache.put(2, 2);

    REQUIRE(cache.size() == 1z);
    REQUIRE(cache.get(1) == std::nullopt);
    REQUIRE(cache.get(2) == 2);
  }
}

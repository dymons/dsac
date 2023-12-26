#include <catch2/catch.hpp>

#include <dsac/cache/lru-k.hpp>

TEST_CASE("[LRU-K] Testcases are checked constructable LRU-K cache", "[lru-k][default]") {
  SECTION("Construct lru-k cache by using user constructor") {
    auto cache = dsac::lru_k<int, int>{/*capacity=*/3z, /*k=*/2z};
  }
}

TEST_CASE("[LRU-K] Testcases are checked inserting elements", "[lru-k][default]") {
  // Arrange
  auto cache = dsac::lru_k<int, int>{/*capacity=*/3z, /*k=*/2z};

  SECTION("Empty LRU-K cache does not take up memory") {
    // Act & Assert
    REQUIRE(cache.size() == 0z);
  }

  SECTION("LRU-K cache should expand automatically when an element is inserted") {
    // Act
    for (auto value : {1, 2, 3}) {
      cache.put(value, value);
    }

    // Assert
    REQUIRE(cache.size() == 3z);
    for (auto expected_value : {1, 2, 3}) {
      auto* value = cache.get(expected_value);
      REQUIRE(nullptr != value);
      REQUIRE(*value == expected_value);
    }
  }

  SECTION("LRU-K should evict element by FIFO replacement policy") {
    // Act
    for (auto const entity : {1, 2, 3, 4}) {
      cache.put(entity, entity);
    }

    // Assert
    REQUIRE(cache.size() == 3z);
    REQUIRE(cache.get(1) == nullptr);
  }

  SECTION("Should move item to the buffer cache when accessed more than once") {
    // Act
    for (auto const entity : {1, 2, 3, 1}) {
      cache.put(entity, entity);
    }

    // Assert
    REQUIRE(cache.size() == 3z);
    REQUIRE(*cache.get(1) == 1);
  }

  SECTION("Buffer cache should contain only K elements") {
    // Act & Assert
    for (auto const entity : {1, 2, 3}) {
      cache.put(entity, entity);
    }
    REQUIRE(cache.get_history_index_for_testsuite().size() == 3z);
    REQUIRE(cache.get_buffer_index_for_testsuite().size() == 0z);

    // Act & Assert
    for (auto const entity : {1, 2, 3}) {
      cache.put(entity, entity);
    }
    REQUIRE(cache.get_history_index_for_testsuite().size() == 0z);
    REQUIRE(cache.get_buffer_index_for_testsuite().size() == 2z);
    REQUIRE(cache.get(1) == nullptr);
    REQUIRE(*cache.get(2) == 2);
    REQUIRE(*cache.get(3) == 3);
  }
  //
  //  SECTION("LRU-K cache should contains only C elements") {
  //    // Act & Assert
  //    for (auto const entity : {1, 2, 3, /*repeat*/ 1, 2, 3, /*new*/ 4, 5, 6}) {
  //      cache.put(entity, entity);
  //    }
  //    REQUIRE(cache.get_history_cache().size() == 1z);
  //    REQUIRE(cache.get_buffer_cache().size() == 2z);
  //  }
}

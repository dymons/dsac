#include <catch2/catch.hpp>

#include <dsac/container/hash/extendible_hashtable.hpp>

namespace {

using namespace dsac;

auto const kDefaultGlobalDepth = std::size_t{2};
auto const kDefaultBucketSize  = std::size_t{1};

using key   = std::string;
using value = std::string;

}  // namespace

TEST_CASE("Extendible hashtable should be constructable", "[extendible_hashtable][default]") {
  SECTION("Construct extendible hashtable by using default constructor") {
    auto hashtable = extendible_hashtable<key, value>{kDefaultGlobalDepth, kDefaultBucketSize};
  }
}

TEST_CASE("Extendible hashtable should expand automatically", "[extendible_hashtable][default]") {
  SECTION("Given global and local depth when them is equal to zero then initial directory has one pointer to one bucket") {
    auto hashtable = extendible_hashtable<key, value>{/*global_depth=*/0, kDefaultBucketSize};
    REQUIRE(hashtable.size() == 1);
  }
  SECTION("Given empty extendible hashtable when has a collision then bucket split") {
    GIVEN("Given empty extendible hashtable") {
      auto hashtable = extendible_hashtable<key, value>{/*global_depth=*/1, kDefaultBucketSize};
      REQUIRE(hashtable.size() == 2);

      WHEN("Insert a key") {
        hashtable.insert("key1", "value1");

        THEN("Extendible hashtable should not expanded") {
          REQUIRE(hashtable.size() == 2);
          REQUIRE(hashtable.contains("key1"));
        }

        WHEN("Insert a new key with collision") {
          hashtable.insert("key3", "value3");

          THEN("Extendible hashtable should split bucket") {
            REQUIRE(hashtable.size() == 4);
            REQUIRE(hashtable.contains("key3"));
          }
        }
      }
    }
  }
}

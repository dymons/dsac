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
            // So, we expected to see 4, but gotten 16. After an unlucky split
            // all keys might be placed in the one of the two new buckets.
            // Possible leading to a cascade of splits

            // key1 has a binary representation - 111000001001111100011011001001010111110001011011011101001100010
            // key2 has a binary representation - 010010111010001010100110001001001000000010000110010111010011010
            //                                                                                                ^^^
            //                                                                  we have three bits which is equal
            //                                                                  and each splits saves the keys to
            //                                                                  the same bucket
            REQUIRE(hashtable.size() == 16);
            REQUIRE(hashtable.contains("key1"));
            REQUIRE(hashtable.contains("key3"));
          }
        }
      }
    }
  }
}

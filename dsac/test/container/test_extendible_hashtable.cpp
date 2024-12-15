#include <catch2/catch.hpp>

#include <dsac/container/hash/extendible_hashtable.hpp>

#include <unordered_map>

namespace {

using namespace dsac;

auto const kDefaultGlobalDepth = std::size_t{2};
auto const kDefaultBucketSize  = std::size_t{1};

using key   = std::string;
using value = std::string;

class extendible_hashtable_fixture final {
  using key   = std::string;
  using value = std::string;

public:
  auto insert(key key, value value) -> void {
    // Property: We should not lose or add new values before insert new key/value
    std::for_each(begin(hashtable_), end(hashtable_), [this](const auto& data) {
      REQUIRE(extendible_hashtable_.contains(data.first));
    });

    extendible_hashtable_.insert(key, value);
    hashtable_.insert_or_assign(key, value);

    // Property: We should not lose or add new values after insert new key/value
    std::for_each(begin(hashtable_), end(hashtable_), [this](const auto& data) {
      REQUIRE(extendible_hashtable_.contains(data.first));
    });

    // Property: The depth of the directory must be equal to the maximum length of the key collision + 1
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return extendible_hashtable_.size();
  }

private:
  extendible_hashtable<key, value> extendible_hashtable_{/*global_depth=*/1, /*buffer_size=*/1};
  std::unordered_map<key, value>   hashtable_{};
};

}  // namespace

TEST_CASE("Extendible hashtable should be constructable", "[extendible_hashtable][default]") {
  SECTION("Construct extendible hashtable by using default constructor") {
    auto hashtable = extendible_hashtable<key, value>{kDefaultGlobalDepth, kDefaultBucketSize};
  }
}

TEST_CASE("Extendible hashtable should expand automatically", "[extendible_hashtable][default]") {
  SECTION(
      "Given global and local depth "
      "When them is equal to zero "
      "Then initial directory has one pointer to one bucket"
  ) {
    auto hashtable = extendible_hashtable<key, value>{/*global_depth=*/0, kDefaultBucketSize};
    REQUIRE(hashtable.size() == 1);
  }

  SECTION(
      "Given empty extendible hashtable"
      "When insert two keys which has a collision by three least significant bits"
      "Then bucket split cascade"
  ) {
    GIVEN("Given empty extendible hashtable") {
      auto fixture = extendible_hashtable_fixture{};
      REQUIRE(fixture.size() == 2);

      WHEN("Insert a key") {
        fixture.insert("key1", "value1");

        THEN("Extendible hashtable should not expanded") {
          REQUIRE(fixture.size() == 2);
        }

        WHEN("Insert a new key with collision") {
          fixture.insert("key3", "value3");

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
            REQUIRE(fixture.size() == 16);
          }
        }
      }
    }
  }
}

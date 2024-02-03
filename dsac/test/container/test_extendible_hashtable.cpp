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
  SECTION("Given global and local when them is equal to zero then initial directory has one pointer to one bucket") {
    auto hashtable = extendible_hashtable<key, value>{/*global_depth=*/0, /*bucket_size=*/0};
    REQUIRE(hashtable.size() == 1);
  }
}

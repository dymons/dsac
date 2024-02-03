#include <catch2/catch.hpp>

#include <dsac/container/hash/extendible_hashtable.hpp>

namespace {

using namespace dsac;

auto const kDefaultGlobalDepth = std::size_t{2};
auto const kDefaultBucketSize  = std::size_t{2};

using key   = std::string;
using value = std::string;

}  // namespace

TEST_CASE("Extendible hashtable should be constructable", "[extendible_hashtable][default]") {
  auto hashtable = extendible_hashtable<key, value>{kDefaultGlobalDepth, kDefaultBucketSize};
}

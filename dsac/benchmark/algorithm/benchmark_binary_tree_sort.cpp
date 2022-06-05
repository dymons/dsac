#include <benchmark/benchmark.h>

#include <dsac/algorithm/binary_tree_sort.hpp>
#include <dsac/algorithm/is_sorted.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <numeric>

namespace {

using testcase = dsac::dynamic_array<int>;

constexpr std::size_t kSampleSize = 1024U;

}  // namespace

static void bm_binary_tree_sort(benchmark::State& state) {
  testcase testcase(kSampleSize);
  std::iota(testcase.begin(), testcase.end(), 0);
  for ([[maybe_unused]] auto _ : state) {  // NOLINT(readability-identifier-length)
    using binary_tree_policy = dsac::binary_search_tree<int, std::greater<>>;
    dsac::binary_tree_sort(begin(testcase), end(testcase), binary_tree_policy{});
  }
}

BENCHMARK(bm_binary_tree_sort);  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

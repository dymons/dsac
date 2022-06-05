#include <benchmark/benchmark.h>

#include <dsac/algorithm/binary_tree_sort.hpp>
#include <dsac/container/dynamic_array.hpp>

#include <algorithm>
#include <numeric>

namespace {

using testcase = dsac::dynamic_array<int>;

}  // namespace

static void bm_binary_tree_sort(benchmark::State& state) {
  testcase testcase(state.range(0));
  std::iota(testcase.begin(), testcase.end(), 0);
  for ([[maybe_unused]] auto _ : state) {  // NOLINT(readability-identifier-length)
    using binary_tree_policy = dsac::binary_search_tree<int, std::greater<>>;
    dsac::binary_tree_sort(begin(testcase), end(testcase), binary_tree_policy{});
  }
}

static void bm_std_sort(benchmark::State& state) {
  testcase testcase(state.range(0));
  std::iota(testcase.begin(), testcase.end(), 0);
  for ([[maybe_unused]] auto _ : state) {  // NOLINT(readability-identifier-length)
    std::sort(begin(testcase), end(testcase), std::greater<>{});
  }
}

BENCHMARK(bm_binary_tree_sort)  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    ->Args({512})
    ->Args({1024})
    ->Args({2048})
    ->Args({4096})
    ->Args({8192});

BENCHMARK(bm_std_sort)  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    ->Args({512})
    ->Args({1024})
    ->Args({2048})
    ->Args({4096})
    ->Args({8192});

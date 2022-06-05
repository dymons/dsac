#include <benchmark/benchmark.h>

#include <dsac/container/dynamic_array.hpp>

#include <vector>

static void bm_dynamic_array_push_back(benchmark::State& state) {
  dsac::dynamic_array<std::size_t> testcase;
  for ([[maybe_unused]] auto _ : state) {  // NOLINT(readability-identifier-length)
    for (std::size_t i{}; i < state.range(0); ++i) {
      testcase.push_back(i);
    }
  }
}

static void bm_std_vector_push_back(benchmark::State& state) {
  std::vector<std::size_t> testcase;
  for ([[maybe_unused]] auto _ : state) {  // NOLINT(readability-identifier-length)
    for (std::size_t i{}; i < state.range(0); ++i) {
      testcase.push_back(i);
    }
  }
}

BENCHMARK(bm_dynamic_array_push_back)  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    ->Args({512})
    ->Args({1024})
    ->Args({2048})
    ->Args({4096})
    ->Args({8192});

BENCHMARK(bm_std_vector_push_back)  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    ->Args({512})
    ->Args({1024})
    ->Args({2048})
    ->Args({4096})
    ->Args({8192});

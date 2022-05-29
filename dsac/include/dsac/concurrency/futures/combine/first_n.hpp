#pragma once

#include <dsac/concurrency/futures/future.hpp>
#include <dsac/concurrency/futures/promise.hpp>
#include <dsac/container/dynamic_array.hpp>
#include <dsac/memory/shared_ptr.hpp>

#include <optional>

namespace dsac {

template <typename T>
future<dynamic_array<result<T>>> first_n(dynamic_array<future<T>>&& futures, std::size_t const n) {
  using result_type = dynamic_array<result<T>>;

  struct context final {
    explicit context(std::size_t num_futures, std::size_t min)
      : v(num_futures)
      , min(min) {
    }

    dynamic_array<std::optional<result<T>>> v;
    const std::size_t                       min;
    std::atomic<std::size_t>                completed = {0U};
    std::atomic<std::size_t>                stored    = {0U};
    promise<result_type>                    promise_;
  };

  if (futures.size() < n) {
    return make_future_on_error<result_type>("not enough futures");
  }

  auto ctx = make_shared<context>(futures.size(), n);
  for (std::size_t i{}; i < futures.size(); ++i) {
    std::move(futures[i]).subscribe([i, ctx](result<T>&& result_data) {
      auto const c = 1 + ctx->completed.fetch_add(1);
      if (c > ctx->min) {
        return;
      }
      ctx->v[i] = std::move(result_data);

      auto const s = 1 + ctx->stored.fetch_add(1);
      if (s < ctx->min) {
        return;
      }

      result_type result_computation;
      result_computation.reserve(ctx->completed.load());
      for (std::size_t j{}; j < ctx->v.size(); ++j) {
        auto& entry = ctx->v[j];
        if (entry.has_value()) {
          result_computation.emplace_back(std::move(entry).value());
        }
      }
      ctx->promise_.set(result<result_type>(std::move(result_computation)));
    });
  }

  return ctx->promise_.make_future();
}

}  // namespace dsac

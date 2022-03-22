#pragma once

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>

#include <atomic>
#include <optional>

namespace dsac::futures {
template <typename T>
auto FirstN(std::vector<Future<T>>&& futures, std::size_t const n)
{
  using Result = std::vector<Try<T>>;

  struct Context {
    explicit Context(size_t num_futures, size_t min)
      : v(num_futures)
      , min(min)
    {
    }

    std::vector<std::optional<Try<T>>> v;
    std::size_t                        min;
    std::atomic<std::size_t>           completed = {0U};
    std::atomic<std::size_t>           stored    = {0U};
    Promise<Result>                    promise;
  };

  if (futures.size() < n) {
    return Future<Result>::Fail("Not enough futures");
  }

  auto ctx = std::make_shared<Context>(futures.size(), n);
  for (std::size_t i{}; i < futures.size(); ++i) {
    std::move(futures[i]).Subscribe([i, ctx](Try<T>&& t) {
      auto const c = 1 + ctx->completed.fetch_add(1);
      if (c > ctx->min) {
        return;
      }
      ctx->v[i] = std::move(t);

      auto const s = 1 + ctx->stored.fetch_add(1);
      if (s < ctx->min) {
        return;
      }

      Result result;
      result.reserve(ctx->completed.load());
      for (std::size_t j{}; j < ctx->v.size(); ++j) {
        auto& entry = ctx->v[j];
        if (entry.has_value()) {
          result.emplace_back(std::move(entry).value());
        }
      }
      ctx->promise.Set(Try<Result>(std::move(result)));
    });
  }

  return ctx->promise.MakeFuture();
}
}  // namespace dsac::futures
#include <catch2/catch.hpp>

#include <concurrency/futures/future.hpp>
#include <concurrency/futures/promise.hpp>

TEST_CASE("Проверка корректности Future&Promise", "[future_and_promise]") {
  using namespace algo::futures;

  SECTION("Выполнение Future&Promise в однопоточном исполнении") {
    Promise<int> promise;
    Future<int> future = promise.MakeFuture();

    std::move(promise).SetValue(10);
    Try<int> value = std::move(future).GetResult();
    REQUIRE_FALSE(value.HasException());
    REQUIRE(value.HasValue());
    REQUIRE(value.Get() == 10);
  }
  SECTION("Выполнение Future&Promise в разных потоках исполнения") {
    Promise<int> promise;
    Future<int> future = promise.MakeFuture();

    std::thread([&promise] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      std::move(promise).SetValue(10);
    }).detach();

    Try<int> value = std::move(future).GetResult();
    REQUIRE_FALSE(value.HasException());
    REQUIRE(value.HasValue());
    REQUIRE(value.Get() == 10);
  }
}

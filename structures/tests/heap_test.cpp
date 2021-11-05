#include <catch2/catch.hpp>

#include <random>
#include <vector>
#include <structures/heap/heap.hpp>

TEST_CASE("Добавление и удаление элементов в MinHeap", "[min_heap]") {
  using namespace algo::heap;

  SECTION("Корректность структуры MinHeap при пустых данных") {
    MinHeap<int> heap;
    REQUIRE(heap.IsEmpty());
    REQUIRE_THROWS(heap.Pool());
    REQUIRE_THROWS(heap.Peek());
  }

  SECTION("Добавление элементов в MinHeap") {
    MinHeap<int> heap;
    heap.Insert(10);
    heap.Insert(5);
    heap.Insert(2);
    REQUIRE_FALSE(heap.IsEmpty());
    REQUIRE(heap.Pool() == 2);
    REQUIRE(heap.Pool() == 5);
    REQUIRE(heap.Pool() == 10);
    REQUIRE(heap.IsEmpty());
  }

  SECTION("Поиск минимального элемента в MinHeap при случайных данных") {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, 20000);

    MinHeap<int> heap;
    constexpr int count_samples = 2000;
    std::vector<int> buffer;
    buffer.reserve(count_samples);
    for (int i = 0; i < count_samples; ++i) {
      buffer.push_back(distribution(generator));
      heap.Insert(buffer.back());
    }

    REQUIRE_FALSE(heap.IsEmpty());
    REQUIRE(heap.Peek() == *std::min_element(buffer.begin(), buffer.end()));
  }
}

TEST_CASE("Добавление и удаление элементов в MaxHeap", "[max_heap]") {
  using namespace algo::heap;

  SECTION("Корректность структуры MaxHeap при пустых данных") {
    MaxHeap<int> heap;
    REQUIRE(heap.IsEmpty());
    REQUIRE_THROWS(heap.Pool());
    REQUIRE_THROWS(heap.Peek());
  }

  SECTION("Добавление элементов в MaxHeap") {
    MaxHeap<int> heap;
    heap.Insert(10);
    REQUIRE(heap.Peek() == 10);
    heap.Insert(5);
    heap.Insert(2);
    REQUIRE_FALSE(heap.IsEmpty());
    REQUIRE(heap.Pool() == 10);
    REQUIRE(heap.Pool() == 5);
    REQUIRE(heap.Pool() == 2);
    REQUIRE(heap.IsEmpty());
  }

  SECTION("Поиск максимального элемента в MaxHeap при случайных данных") {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, 20000);

    MaxHeap<int> heap;
    constexpr int count_samples = 2000;
    std::vector<int> buffer;
    buffer.reserve(count_samples);
    for (int i = 0; i < count_samples; ++i) {
      buffer.push_back(distribution(generator));
      heap.Insert(buffer.back());
    }

    REQUIRE_FALSE(heap.IsEmpty());
    REQUIRE(heap.Peek() == *std::max_element(buffer.begin(), buffer.end()));
  }
}
#include <dsac/container/graph/travelling_salesman_problem.hpp>

#include <cmath>
#include <string>

namespace dsac::legacy_graph::tsp {
/// Вспомогательная функция для преобразования числа из двоичной системы в
/// десятичную
[[nodiscard]] static int ConvertBinary(int n) {
  int factor = 1;
  int total  = 0;

  while (n != 0) {
    total += (n % 10) * factor;
    n /= 10;
    factor *= 2;
  }

  return total;
}

/// Вспомогательная функция для генерации всех возможных комбинация
/// последовательностей из bits бит раз размера size. К примеру combinations(3,
/// 4) вернет {0111,1011,1101,1110}
[[nodiscard]] static std::vector<int> combinations(int bits, int size) {
  // TODO Переписать на генерацию последовательностей через рекурсивную функцию
  std::string str(size, '0');
  std::fill(std::next(str.begin(), size - bits), str.end(), '1');

  std::vector<int> result;
  result.reserve(size);
  do {
    result.push_back(ConvertBinary(std::stoi(str)));
  } while (std::next_permutation(str.begin(), str.end()));
  return result;
}

namespace detail {
/// Инициализируем таблицу memo оптимальным решением из стартовой позиции во все
/// остальные узлы графа
static void Setup(CompleteGraph& graph, CompleteGraph::AdjacencyMatrix& memo, const Node start, const int N) {
  for (int node_id = 0; node_id < N; ++node_id) {
    if (node_id != start.id) [[likely]] {
      /// Сохраняем оптимальное значение из стартового узла во все остальные
      /// узлы
      memo[node_id][(1 << start.id) | (1 << node_id)] = graph.GetCost(start, {node_id});
    }
  }
}

static void Solve(CompleteGraph& graph, CompleteGraph::AdjacencyMatrix& memo, const Node start, const int N) {
  const auto NotIn = [](Node node, int subset) { return ((1 << node.id) & subset) == 0; };

  for (int r = 3; r <= N; ++r) {
    for (int subset : combinations(r, N)) {
      if (NotIn(start, subset)) {
        continue;
      }

      for (int next = 0; next < N; ++next) {
        if ((next == start.id) || NotIn({next}, subset)) {
          continue;
        }

        const int subset_without_next = subset ^ (1 << next);
        int       min_dist            = std::numeric_limits<int>::max();
        for (int e = 0; e < N; ++e) {
          if ((e == start.id) || (e == next) || (NotIn({e}, subset))) {
            continue;
          }

          const int new_dist = memo[e][subset_without_next] + graph.GetCost({e}, {next});
          min_dist           = std::min(min_dist, new_dist);
        }
        memo[next][subset] = min_dist;
      }
    }
  }
}

static int FindMinCost(CompleteGraph& graph, CompleteGraph::AdjacencyMatrix& memo, const Node start, const int N) {
  const int end_state = (1 << N) - 1;

  int min_tour_cost = std::numeric_limits<int>::max();
  for (int e = 0; e < N; ++e) {
    if (e != start.id) [[likely]] {
      const int tour_cost = memo[e][end_state] + graph.GetCost({e}, start);
      min_tour_cost       = std::min(min_tour_cost, tour_cost);
    }
  }
  return min_tour_cost;
}

static std::vector<Node> FindOptimalTour(
    CompleteGraph& graph, CompleteGraph::AdjacencyMatrix& memo, const Node start, const int N) {
  const auto NotIn = [](Node node, int subset) { return ((1 << node.id) & subset) == 0; };

  int               last_index = start.id;
  int               state      = (1 << N) - 1;
  std::vector<Node> tour(N + 1);

  for (int i = N - 1; i >= 1; --i) {
    int index = -1;
    for (int j = 0; j < N; ++j) {
      if (j == start.id || NotIn({j}, state)) {
        continue;
      }

      if (index == -1) {
        index = j;
      }

      const int prev_dist = memo[index][state] + graph.GetCost({index}, {last_index});
      const int new_dist  = memo[j][state] + graph.GetCost({j}, {last_index});
      if (new_dist < prev_dist) {
        index = j;
      }
    }

    tour[i]    = Node{index};
    state      = state ^ (1 << index);
    last_index = index;
  }

  tour.front() = start;
  tour.back()  = start;
  return tour;
}
}  // namespace detail

TravellingSalesmanResult Solve(CompleteGraph& graph, Node start) {
  const int                      N = graph.GetMatrixSize();
  CompleteGraph::AdjacencyMatrix memo(N, CompleteGraph::AdjacencyRow(1 << N, 0));

  detail::Setup(graph, memo, start, N);
  detail::Solve(graph, memo, start, N);

  TravellingSalesmanResult result;
  result.tour     = detail::FindOptimalTour(graph, memo, start, N);
  result.min_cost = detail::FindMinCost(graph, memo, start, N);
  return result;
}
}  // namespace dsac::legacy_graph::tsp
#pragma once

#include <stdexcept>
#include <vector>

namespace dsac::graph {
class DisjointSetException : public std::logic_error {
 public:
  using std::logic_error::logic_error;
};

class DisjointSetOutOfRange : public DisjointSetException {
 public:
  DisjointSetOutOfRange() : DisjointSetException("Out of range disjoint set") {
  }
};

//////////////////////////////////////////////////////////////////////

class DisjointSet final {
  std::vector<std::size_t> parent_vertex_;

 public:
  //! Конструктор DisjointSet
  /*!
    \param size_set Размер системы непересекающихся множест
    \sa    Time Complexity O(N), Space Complexity O(N)
  */
  explicit DisjointSet(std::size_t size_set);

  //! Объединение двух множеств в одно по двум вершинам
  /*!
    \param vertex1 Вершина из системы множества I
    \param vertex2 Вершина из системы множества J
    \sa    Time Complexity O(log(N)), Space Complexity O(1)
  */
  void Union(std::size_t vertex1, std::size_t vertex2);

  //! Поиск родительской вершины в множестве I для указанной вершины
  /*!
    \param vertex Вершина из системы множества I
    \sa    Time Complexity O(log(N)), Space Complexity O(1)
  */
  std::size_t Find(std::size_t vertex);

  //! Проверка, что две вершины находятся в одной системе множества
  /*!
    \param vertex1 Вершина из системы множества I
    \param vertex2 Вершина из системы множества J
    \sa    Time Complexity O(log(N)), Space Complexity O(1)
  */
  bool IsConnected(std::size_t vertex1, std::size_t vertex2);
};
}  // namespace dsac::graph
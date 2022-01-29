#pragma once

#include <stdexcept>
#include <vector>

namespace algo::graph {
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
  //! The DisjointSet construction
  /*!
    \param size_set The size of disjoint set.
    \sa    Time Complexity O(N), Space Complexity O(N)
  */
  explicit DisjointSet(std::size_t size_set);

  //! The Union function unions two vertices and makes their parent nodes the same.
  /*!
    \param vertex1 first vertex.
    \param vertex2 second vertex.
    \sa    Time Complexity O(N), Space Complexity O(1)
  */
  void Union(std::size_t vertex1, std::size_t vertex2);

  //! The Find function finds the parent node of a given vertex.
  /*!
    \param vertex given vertex.
    \sa    Time Complexity O(1), Space Complexity O(1)
  */
  std::size_t Find(std::size_t vertex);

  //! The IsConnected function checks that two vertices have the same parent.
  /*!
    \param vertex1 first vertex.
    \param vertex2 second vertex.
    \sa    Time Complexity O(1), Space Complexity O(1)
  */
  bool IsConnected(std::size_t vertex1, std::size_t vertex2);
};
}  // namespace algo::graph
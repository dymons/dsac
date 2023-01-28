#ifndef ALGORITHM_BREADTH_FIRST_SEARCH_HPP
#error This file may only be included from breadth_first_search.hpp
#endif

#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace dsac {

template <typename... Args>
std::vector<typename directed_graph<Args...>::node_iterator> bfs(
    directed_graph<Args...> const&                         graph,
    typename directed_graph<Args...>::node_key_type const& source,
    typename directed_graph<Args...>::node_key_type const& destination
) {
  using node_iterator = typename directed_graph<Args...>::node_iterator;

  auto const source_node      = graph.get_node(source);
  auto const destination_node = graph.get_node(destination);

  std::queue<node_iterator> processing;
  processing.push(source_node);

  std::unordered_map<node_iterator, node_iterator> possible_actions;
  possible_actions[source_node] = source_node;

  std::unordered_set<node_iterator> visited;
  visited.emplace(source_node);

  bool found = false;
  while (!processing.empty()) {
    const node_iterator considered = processing.front();
    if (considered == destination_node) {
      found = true;
      break;
    }

    processing.pop();
    for (auto const& edge : considered->edges) {
      if (auto const [_, not_exist] = visited.emplace(edge->to); not_exist) {
        possible_actions[edge->to] = considered;
        processing.push(edge->to);
      }
    }
  }

  std::vector<node_iterator> path;
  if (!found) {
    return path;
  }

  path.push_back(destination_node);
  for (node_iterator latest = path.front(); latest != source_node; latest = path.front()) {
    if (!possible_actions.contains(latest)) {
      break;
    }
    path.insert(path.begin(), possible_actions[latest]);
  }

  return path;
}

}  // namespace dsac
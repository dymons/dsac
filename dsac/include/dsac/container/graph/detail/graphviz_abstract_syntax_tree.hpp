#pragma once

#include <dsac/container/dynamic_array.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

enum class kind {
  node,
  direct_graph,
  undirect_graph,
  statements,
  edge_direct_connection,
  edge_undirect_connection,
  attribute,
  attributes,
  max_kind_names_id,
};

class ast_exception : public std::logic_error {
public:
  using std::logic_error::logic_error;
};

class unexpected_kind : public ast_exception {
public:
  unexpected_kind()
    : ast_exception("The node does not match with kind") {
  }
};

class ast_base {
  kind kind_;

public:
  explicit ast_base(kind kind);
  virtual ~ast_base() = default;

  kind what() noexcept;
};
using ast_base_ref = dsac::shared_ptr<ast_base>;

class ast_direct_graph_node : public ast_base {
  const std::string_view graph_name_;
  ast_base_ref           child_;

public:
  explicit ast_direct_graph_node(std::string_view graph_name, ast_base_ref child);

  std::string_view get_graph_name() const noexcept;
};

class ast_undirect_graph_node : public ast_base {
  const std::string_view graph_name_;
  ast_base_ref           child_;

public:
  explicit ast_undirect_graph_node(std::string_view graph_name, ast_base_ref child);
};

class ast_statements_node : public ast_base {
  dynamic_array<ast_base_ref> statements_;

public:
  using ast_base::ast_base;
  void insert_statement(ast_base_ref statement);
};

class ast_node_node : public ast_base {
  const std::string_view node_name_;
  ast_base_ref           attributes_;

public:
  explicit ast_node_node(kind kind, std::string_view node_name, ast_base_ref attributes);
};

class ast_direct_edge_node : public ast_base {
  ast_base_ref left_child_;
  ast_base_ref right_child_;

public:
  explicit ast_direct_edge_node(ast_base_ref left_child, ast_base_ref right_child);
};

class ast_undirect_edge_node : public ast_base {
  ast_base_ref left_child_;
  ast_base_ref right_child_;

public:
  explicit ast_undirect_edge_node(ast_base_ref left_child, ast_base_ref right_child);
};

class ast_attributes_node : public ast_base {
  dynamic_array<ast_base_ref> attributes_;

public:
  using ast_base::ast_base;
  void insert_attribute(ast_base_ref attribute);
};

class ast_attribute_node : public ast_base {
  const std::string_view key_;
  const std::string_view value_;

public:
  explicit ast_attribute_node(kind kind, std::string_view key, std::string_view value);
};

template <kind k>
struct ast_node_from_kind;

#define MAKE_AST_TRAITS(t, k)    \
  template <>                    \
  struct ast_node_from_kind<k> { \
    using type = t;              \
  };

MAKE_AST_TRAITS(ast_direct_graph_node, kind::direct_graph)
MAKE_AST_TRAITS(ast_undirect_graph_node, kind::undirect_graph)
MAKE_AST_TRAITS(ast_statements_node, kind::statements)
MAKE_AST_TRAITS(ast_node_node, kind::node)
MAKE_AST_TRAITS(ast_direct_edge_node, kind::edge_direct_connection)
MAKE_AST_TRAITS(ast_undirect_edge_node, kind::edge_undirect_connection)
MAKE_AST_TRAITS(ast_attributes_node, kind::attributes)
MAKE_AST_TRAITS(ast_attribute_node, kind::attribute)

namespace detail {

static auto* unsafe_to(ast_base* base) {
  switch (base->what()) {
    case kind::direct_graph: {
      return static_cast<ast_direct_graph_node*>(base);
    }
  }
  throw unexpected_kind{};
}

template <typename ReturnType, class F, class N, std::size_t Kind, typename = std::bool_constant<true>>
struct function_value {
  static constexpr ReturnType (*value)(F&&, N) = [](F&& f, N n) -> ReturnType {
    return f(unsafe_to(n));
  };
};

template <typename F, class N, std::size_t... Idx>
auto visit(F&& f, N n, std::index_sequence<0, Idx...>) {
  using return_type = ast_base*;

  static constexpr return_type (*kVtable[])(F&&, N){(function_value<return_type, F, N, Idx>::value)...};
  return kVtable[(std::size_t)n->what()](std::forward<F>(f), n);
}

}  // namespace detail

template <typename F, class N>
auto visit(F&& f, N n) {
  return detail::visit(std::forward<F>(f), n, std::make_index_sequence<(std::size_t)kind::max_kind_names_id>{});
}

}  // namespace dsac

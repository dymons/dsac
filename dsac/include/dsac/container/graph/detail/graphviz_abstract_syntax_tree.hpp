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
  ast_base_ref           statements_;

public:
  explicit ast_direct_graph_node(std::string_view graph_name, ast_base_ref statements);

  std::string_view get_graph_name() const noexcept;

  ast_base_ref get_statements() const noexcept;
};

class ast_undirect_graph_node : public ast_base {
  const std::string_view graph_name_;
  ast_base_ref           statements_;

public:
  explicit ast_undirect_graph_node(std::string_view graph_name, ast_base_ref statements);
};

class ast_statements_node : public ast_base {
  dynamic_array<ast_base_ref> statements_;

public:
  using ast_base::ast_base;

  void insert_statement(ast_base_ref statement);

  dynamic_array<ast_base_ref>::const_iterator begin() const;
  dynamic_array<ast_base_ref>::const_iterator end() const;
};

class ast_node_node : public ast_base {
  const std::string_view node_name_;
  ast_base_ref           attributes_;

public:
  explicit ast_node_node(kind kind, std::string_view node_name, ast_base_ref attributes);

  std::string_view get_name() const noexcept;

  ast_base_ref get_attributes() const noexcept;
};

class ast_direct_edge_node : public ast_base {
  ast_base_ref left_child_;
  ast_base_ref right_child_;

public:
  explicit ast_direct_edge_node(ast_base_ref left_child, ast_base_ref right_child);

  ast_base_ref get_from_node() const noexcept;
  ast_base_ref get_to_node() const noexcept;
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

namespace detail {

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

template <typename ReturnType, class F, std::size_t Kind>
struct function_value {
  static constexpr ReturnType (*kValue)(F&&, ast_base*) = [](F&& f, ast_base* base) -> ReturnType {
    using ast_node_type = typename ast_node_from_kind<static_cast<kind>(Kind)>::type;
    if constexpr (requires { f(*dynamic_cast<ast_node_type*>(base)); }) {
      return f(*dynamic_cast<ast_node_type*>(base));
    }
    return nullptr;
  };
};

template <typename F, std::size_t... Idx>
auto visit(F&& f, ast_base* base, [[maybe_unused]] std::index_sequence<0, Idx...> sequence) {
  using return_type = ast_base_ref;

  static constexpr return_type (*kVtable[])(F&&, ast_base*){(function_value<return_type, F, Idx - 1U>::kValue)...};
  return kVtable[static_cast<std::size_t>(base->what())](std::forward<F>(f), base);
}

}  // namespace detail

template <typename F, typename Indices = std::make_index_sequence<static_cast<std::size_t>(kind::max_kind_names_id)>>
auto visit(F&& f, ast_base_ref const& base) {
  return detail::visit(std::forward<F>(f), base.get(), Indices{});
}

template <std::derived_from<dsac::ast_base> Derived>
inline Derived* unsafe_to(dsac::ast_base_ref const& base) {
  return dynamic_cast<Derived*>(base.get());
}

}  // namespace dsac

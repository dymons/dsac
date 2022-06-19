#pragma once

#include <dsac/container/dynamic_array.hpp>
#include <dsac/memory/shared_ptr.hpp>

namespace dsac {

class ast_base {
public:
  virtual ~ast_base() = default;
};
using ast_base_ref = dsac::shared_ptr<ast_base>;

class ast_graph_node : public ast_base {
  const std::string_view graph_type_;
  const std::string_view graph_name_;
  ast_base_ref           child_;

public:
  explicit ast_graph_node(std::string_view graph_type, std::string_view graph_name, ast_base_ref child);
};

class ast_statement_list_node : public ast_base {
  dynamic_array<ast_base_ref> statements_;

public:
  void insert_statement(ast_base_ref statement);
};

class ast_node_node : public ast_base {
  const std::string_view node_name_;
  ast_base_ref           attributes_;

public:
  explicit ast_node_node(std::string_view node_name, ast_base_ref attributes);
};

class ast_edge_node : public ast_base {
  const std::string_view operator_;
  ast_base_ref           left_child_;
  ast_base_ref           right_child_;

public:
  explicit ast_edge_node(std::string_view operator_, ast_base_ref left_child, ast_base_ref right_child);
};

class ast_attributes_node : public ast_base {
  dynamic_array<ast_base_ref> attributes_;

public:
  void insert_attribute(ast_base_ref attribute);
};

class ast_attribute_node : public ast_base {
  const std::string_view key_;
  const std::string_view value_;

public:
  explicit ast_attribute_node(std::string_view key, std::string_view value);
};

}  // namespace dsac

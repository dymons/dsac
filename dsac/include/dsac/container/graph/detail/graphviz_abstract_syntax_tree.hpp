#pragma once

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
  ~ast_graph_node() override = default;
};

}  // namespace dsac

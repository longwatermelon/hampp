#pragma once
#include "AST.h"
#include <memory>
#include <vector>
#include <string>

struct Visitor
{
	Visitor();

	std::shared_ptr<AST> builtin_function_print(std::vector<std::shared_ptr<AST>> args);
	std::shared_ptr<AST> builtin_function_destroy(std::vector< std::shared_ptr<AST>> args);

	std::vector<std::shared_ptr<AST>> variable_defs;
	std::vector<std::shared_ptr<AST>> function_defs;

	std::shared_ptr<AST> visit(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_vardef(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_var(std::shared_ptr<AST> node);

	std::shared_ptr<AST> get_var_from_value(std::string value);

	std::shared_ptr<AST> visit_str(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_compound(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_func_call(std::shared_ptr<AST> node);

	std::shared_ptr<AST> get_func_def(const std::string name);

	std::shared_ptr<AST> add_func_def(std::shared_ptr<AST> node);
};
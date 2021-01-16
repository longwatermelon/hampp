#pragma once
#include "AST.h"
#include "utils.h"
#include "../modules/random.h"
#include <memory>
#include <vector>
#include <string>

struct Visitor
{
	Visitor();

	/* built in functions */
	std::shared_ptr<AST> builtin_function_print(std::vector<std::shared_ptr<AST>> args);
	std::shared_ptr<AST> builtin_function_destroy(std::vector< std::shared_ptr<AST>> args);
	std::shared_ptr<AST> builtin_function_strcmp(std::vector<std::shared_ptr<AST>> args);
	std::shared_ptr<AST> builtin_function_add(std::vector<std::shared_ptr<AST>> args, std::shared_ptr<AST> node);
	std::shared_ptr<AST> builtin_function_subtract(std::vector<std::shared_ptr<AST>> args, std::shared_ptr<AST> node);
	std::shared_ptr<AST> builtin_function_index(std::vector<std::shared_ptr<AST>> args, std::shared_ptr<AST> node);

	/* variable definitions */
	std::vector<std::shared_ptr<AST>> variable_defs;
	std::vector<std::shared_ptr<AST>> function_defs;
	std::vector<std::shared_ptr<AST>> struct_defs;

	/* visitor logic */
	std::shared_ptr<AST> visit(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_vardef(std::shared_ptr<AST> node);
	std::shared_ptr<AST> visit_var(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_str(std::shared_ptr<AST> node);
	std::shared_ptr<AST> visit_bool(std::shared_ptr<AST> node);
	std::shared_ptr<AST> visit_int(std::shared_ptr<AST> node);
	std::shared_ptr<AST> visit_list(std::shared_ptr<AST> node);
	std::shared_ptr<AST> visit_instance(std::shared_ptr<AST> node);
	std::shared_ptr<AST> visit_instance_member(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_compound(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_func_call(std::shared_ptr<AST> node);
	std::shared_ptr<AST> get_func_def(const std::string name);
	std::shared_ptr<AST> add_func_def(std::shared_ptr<AST> node);

	std::shared_ptr<AST> visit_struct_def(std::shared_ptr<AST> node);
	std::shared_ptr<AST> add_struct_def(std::shared_ptr<AST> node);
	std::shared_ptr<AST> get_struct_def(std::string name);
	std::shared_ptr<AST> get_instance_member(std::shared_ptr<AST> instance, std::string var_name);

	std::shared_ptr<AST> visit_conditional(std::shared_ptr<AST> node);

	void check_compatible_types(AstType type1, AstType type2, std::shared_ptr<AST> arg);
};
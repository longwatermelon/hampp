#include "..\include\visitor.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

Visitor::Visitor()
{
	const auto const_true = std::make_shared<AST>(AstType::AST_VARIABLE_DEFINITION);
	const auto ast_bool = std::make_shared<AST>(AstType::AST_BOOL);
	const_true->variable_definition_name = "true";
	ast_bool->bool_value = true;
	const_true->variable_definition_value = ast_bool;
	visit(const_true);

	const auto const_false = std::make_shared<AST>(AstType::AST_VARIABLE_DEFINITION);
	const auto ast_bool_false = std::make_shared<AST>(AstType::AST_BOOL);
	const_false->variable_definition_name = "false";
	ast_bool_false->bool_value = false;
	const_false->variable_definition_value = ast_bool_false;
	visit(const_false);
}

std::shared_ptr<AST> Visitor::builtin_function_print(std::vector<std::shared_ptr<AST>> args)
{
	for (auto& arg : args)
	{
		std::shared_ptr<AST> ast = visit(arg);

		switch (ast->type)
		{
		case AstType::AST_STRING: std::cout << ast->string_value << " "; break;
		case AstType::AST_BOOL: std::cout << ast->bool_value << " "; break;
		case AstType::AST_INT: std::cout << ast->int_value << " "; break;
		default: std::cout << ast; break;
		}
	}

	std::cout << std::endl;

	return std::make_shared<AST>(AstType::AST_NOOP);
}

std::shared_ptr<AST> Visitor::builtin_function_destroy(std::vector<std::shared_ptr<AST>> args)
{
	for (auto& arg : args)
	{
		std::shared_ptr<AST> ast = get_var_from_name(arg->variable_name);
		std::vector<std::shared_ptr<AST>>::iterator iter = std::find(variable_defs.begin(), variable_defs.end(), ast);
		if (iter != variable_defs.end())
		{
			__int64 index = std::distance(variable_defs.begin(), iter);
			variable_defs.erase(variable_defs.begin() + index);
		}
		else
		{
			variable_defs.erase(variable_defs.begin());
		}
	}
	
	return std::make_shared<AST>(AstType::AST_NOOP);
}

std::shared_ptr<AST> Visitor::builtin_function_strcmp(std::vector<std::shared_ptr<AST>> args)
{
	auto ast_bool = std::make_shared<AST>(AstType::AST_BOOL);

	for (unsigned i = 0; i < args.size(); ++i)
	{
		if (i == 0) { continue; }

		std::string var1value, var2value;
		AstType type1;

		if (args[i]->type == AstType::AST_STRING)
		{
			var1value = args[i]->string_value;
			type1 = args[i]->type;
		}
		else if (args[i]->type == AstType::AST_INT)
		{
			var1value = args[i]->int_value;
			type1 = args[i]->type;
		}
		else if (args[i]->type == AstType::AST_BOOL)
		{
			var1value = args[i]->bool_value;
			type1 = args[i]->type;
		}
		else if (args[i]->type == AstType::AST_VARIABLE)
		{
			std::shared_ptr<AST> var1 = get_var_from_name(args[i]->variable_name);

			switch (var1->variable_definition_value->type)
			{
			case AstType::AST_STRING: var1value = var1->variable_definition_value->string_value; break;
			case AstType::AST_INT: var1value = var1->variable_definition_value->int_value; break;
			case AstType::AST_BOOL: var1value = var1->variable_definition_value->bool_value; break;
			}

			type1 = var1->variable_definition_value->type;
		}


		if (args[i - 1]->type == AstType::AST_STRING)
		{
			var2value = args[i - 1]->string_value;

			check_compatible_types(args[i]->type, args[i - 1]->type);
		}
		else if (args[i - 1]->type == AstType::AST_INT)
		{
			var2value = args[i - 1]->int_value;
			
			check_compatible_types(args[i]->type, args[i - 1]->type);
		}
		else if (args[i - 1]->type == AstType::AST_BOOL)
		{
			var2value = args[i - 1]->bool_value;

			check_compatible_types(args[i]->type, args[i - 1]->type);
		}
		else if (args[i - 1]->type == AstType::AST_VARIABLE)
		{
			std::shared_ptr<AST> var2 = get_var_from_name(args[i - 1]->variable_name);
			
			switch (var2->variable_definition_value->type)
			{
			case AstType::AST_STRING: var2value = var2->variable_definition_value->string_value; break;
			case AstType::AST_INT: var2value = var2->variable_definition_value->int_value; break;
			case AstType::AST_BOOL: var2value = var2->variable_definition_value->bool_value; break;
			}

			check_compatible_types(var2->variable_definition_value->type, type1);
		}

		if (var1value == var2value)
		{
			ast_bool->bool_value = true;
		}
		else
		{
			ast_bool->bool_value = false;
			return ast_bool;
		}
	}

	return ast_bool;
}

std::shared_ptr<AST> Visitor::visit(std::shared_ptr<AST> node)
{
	switch (node->type)
	{
	case AstType::AST_VARIABLE_DEFINITION: return visit_vardef(node);
	case AstType::AST_VARIABLE: return visit_var(node);
	case AstType::AST_FUNCTION_CALL: return visit_func_call(node);
	case AstType::AST_FUNCTION_DEFINITION: return add_func_def(node);
	case AstType::AST_STRING: return visit_str(node);
	case AstType::AST_INT: return visit_int(node);
	case AstType::AST_BOOL: return visit_bool(node);
	case AstType::AST_COMPOUND: return visit_compound(node);
	case AstType::AST_CONDITIONAL: return visit_conditional(node);
	case AstType::AST_NOOP: return node;
	}
	
	throw std::runtime_error("Uncaught statement\n");
	return nullptr;
}

std::shared_ptr<AST> Visitor::visit_vardef(std::shared_ptr<AST> node)
{
	for (auto& def : variable_defs)
	{
		if (node->variable_definition_name == def->variable_definition_name)
		{
			std::stringstream err;
			err << "Reinitialized a variable on line " << node->error_line_num << ":\n" << node->error_line_contents << "\n"
				<< "\n" << "Variable previously initialized on line " << def->error_line_num << ":\n" << def->error_line_contents << "\n";
			throw std::runtime_error(err.str());
		}
	}

	variable_defs.emplace_back(node);
	return node;
}

std::shared_ptr<AST> Visitor::visit_var(std::shared_ptr<AST> node)
{
	for (auto& def : variable_defs)
	{
		if (def->variable_definition_name == node->variable_name || def->variable_definition_name == node->variable_definition_name)
		{
			return visit(def->variable_definition_value);
		}
	}

	std::stringstream msg;
	msg << "Undefined variable '" << node->variable_name << "' on line " <<
		node->error_line_num << ":\n" << node->error_line_contents << "\n" << node->error_arrow << "\n";
	throw std::runtime_error(msg.str());
	return nullptr;
}

std::shared_ptr<AST> Visitor::get_var_from_value(std::string value)
{
	for (auto& def : variable_defs)
	{
		if (def->string_value == value)
		{
			return def;
		}
	}


	return nullptr;
}

std::shared_ptr<AST> Visitor::get_var_from_name(std::string name)
{
	for (auto& def : variable_defs)
	{
		if (def->variable_definition_name == name || def->variable_name == name)
		{
			return def;
		}
	}

	return nullptr;
}

std::shared_ptr<AST> Visitor::visit_str(std::shared_ptr<AST> node)
{
	return node;
}

std::shared_ptr<AST> Visitor::visit_bool(std::shared_ptr<AST> node)
{
	return node;
}

std::shared_ptr<AST> Visitor::visit_int(std::shared_ptr<AST> node)
{
	return node;
}

std::shared_ptr<AST> Visitor::visit_compound(std::shared_ptr<AST> node)
{
	for (auto& item : node->compound_value)
	{
		visit(item);
	}

	return std::make_shared<AST>(AstType::AST_NOOP);
}

std::shared_ptr<AST> Visitor::visit_func_call(std::shared_ptr<AST> node)
{
	if (node->function_call_name == "prount")
	{
		return builtin_function_print(node->function_call_args);
	}

	if (node->function_call_name == "destroy")
	{
		return builtin_function_destroy(node->function_call_args);
	}

	if (node->function_call_name == "cmp")
	{
		return builtin_function_strcmp(node->function_call_args);
	}

	std::shared_ptr<AST> func_def = get_func_def(node->function_call_name);

	if (func_def == nullptr)
	{
		std::stringstream msg;
		msg << "Undefined method '" << node->function_call_name << "' on line " 
			<< node->error_line_num << ":\n" << node->error_line_contents << "\n" 
			<< node->error_arrow << std::endl;
		throw std::runtime_error(msg.str());

	}

	if (func_def->function_definition_params.size() != node->function_call_args.size())
	{
		std::stringstream err;
		std::string param_display;
		for (auto& param : func_def->function_definition_params)
		{
			param_display += param->variable_name;
			param_display += ", ";
		}

		if (param_display.size() > 2)
		{
			param_display.erase(param_display.size() - 2, param_display.size());
		}

		if (node->function_call_args.size() == 1 && node->function_call_args[0]->type == AstType::AST_NOOP)
		{
			;
		}
		else
		{
			err << "Line " << node->error_line_num << ":\n" << node->error_line_contents << "\n" << "Error: Function '" << func_def->function_definition_name << "' takes " << func_def->function_definition_params.size() << " arguments (" << param_display << ") but " << node->function_call_args.size() << " arguments were supplied.\n";
			throw std::runtime_error(err.str());
		}
		
	}

	std::shared_ptr<AST> ast;

	if (node->function_call_args.size() > 0 && node->function_call_args[0]->type != AstType::AST_NOOP)
	{
		for (int i = 0; i < node->function_call_args.size(); ++i)
		{
			std::shared_ptr<AST> var = func_def->function_definition_params[i];
			std::shared_ptr<AST> value = node->function_call_args[i];

			const auto vardef = std::make_shared<AST>(AstType::AST_VARIABLE_DEFINITION);
			vardef->variable_definition_value = value;

			vardef->variable_definition_name = var->variable_name;

			visit_vardef(vardef);
		}

		const auto ast = visit(func_def->function_definition_body);

		builtin_function_destroy(node->function_call_args);
	}
	else
	{
		ast = visit(func_def->function_definition_body);
	}

	return ast;
}

std::shared_ptr<AST> Visitor::get_func_def(const std::string name)
{
	for (auto& def : function_defs)
	{
		if (def->function_definition_name == name)
		{
			return def;
		}
	}

	return nullptr;
}

std::shared_ptr<AST> Visitor::add_func_def(std::shared_ptr<AST> node)
{
	function_defs.emplace_back(node);

	return node;
}

std::shared_ptr<AST> Visitor::visit_conditional(std::shared_ptr<AST> node)
{
	std::shared_ptr<AST> ast_boolean;
	if (node->conditional_condition->conditional_condition->type == AstType::AST_FUNCTION_CALL)
	{
		ast_boolean = visit_func_call(node->conditional_condition->conditional_condition);
	}
	else if (node->conditional_condition->conditional_condition->type == AstType::AST_VARIABLE)
	{
		ast_boolean = visit_var(node->conditional_condition->conditional_condition);
	}
	
	
	bool boolean = ast_boolean->bool_value;
	if (boolean)
	{
		return visit(node->conditional_body);
	}
	else
	{
		node->conditional_body = std::make_shared<AST>(AstType::AST_NOOP);
		return visit(node->conditional_body);
	}
}

void Visitor::check_compatible_types(AstType type1, AstType type2)
{
	if (type1 != type2)
	{
		std::stringstream err;
		err << "Incompatible types: Cannot compare " << ast_to_str(type1) << " with " << ast_to_str(type2);
		throw std::runtime_error(err.str());
	}
}

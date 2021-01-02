#include "..\include\visitor.h"
#include <iostream>
#include <string>
#include <sstream>

Visitor::Visitor()
{
}

std::shared_ptr<AST> Visitor::builtin_function_print(std::vector<std::shared_ptr<AST>> args)
{
	for (auto& arg : args)
	{
		std::shared_ptr<AST> ast = visit(arg);

		switch (ast->type)
		{
		case AstType::AST_STRING: std::cout << ast->string_value << " "; break;
		default: std::cout << ast; break;
		}
	}

	std::cout << std::endl;

	return std::make_shared<AST>(AstType::AST_NOOP);
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
	case AstType::AST_COMPOUND: return visit_compound(node);
	case AstType::AST_NOOP: return node;
	}
	
	throw std::runtime_error("Uncaught statement\n");
	return nullptr;
}

std::shared_ptr<AST> Visitor::visit_vardef(std::shared_ptr<AST> node)
{
	for (auto& def : variable_defs)
	{
		if (node->function_definition_name == def->function_definition_name)
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
		if (def->variable_definition_name == node->variable_name)
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

std::shared_ptr<AST> Visitor::visit_str(std::shared_ptr<AST> node)
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
		param_display.erase(param_display.size() - 2, param_display.size());
		err << "Line " << node->error_line_num << ":\n" << node->error_line_contents << "\n" << "Error: Function '" << func_def->function_definition_name << "' takes " << func_def->function_definition_params.size() << " arguments (" << param_display << ") but " << node->function_call_args.size() << " arguments were supplied.\n";
		throw std::runtime_error(err.str());
	}

	for (int i = 0; i < node->function_call_args.size(); ++i)
	{
		std::shared_ptr<AST> var = func_def->function_definition_params[i];
		std::shared_ptr<AST> value = node->function_call_args[i];

		const auto vardef = std::make_shared<AST>(AstType::AST_VARIABLE_DEFINITION);
		vardef->variable_definition_value = value;

		vardef->variable_definition_name = var->variable_name;

		visit_vardef(vardef);
	}

	return visit(func_def->function_definition_body);
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

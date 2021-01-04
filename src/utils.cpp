#include "../include/utils.h"

std::shared_ptr<AST> goto_root_of_var(Visitor visitor, std::string name)
{
	auto var = get_var_from_name(visitor, name);
	AstType type = var->variable_definition_value->type;
	while (type == AstType::AST_VARIABLE)
	{
		type = AstType::AST_NOOP;
		switch (type)
		{
		case AstType::AST_VARIABLE: var = goto_root_of_var(visitor, var->variable_definition_value->variable_name); type = AstType::AST_VARIABLE; break;
		default: type = AstType::AST_NOOP; break;
		}
	}


	return var->variable_definition_value;
}

void eat_type(AstType type, AstType expected_type, std::shared_ptr<AST> node)
{
	if (type != expected_type)
	{
		std::stringstream err;
		err << "Line " << node->error_line_num << ":\n" << node->error_line_contents
			<< "\nExpected type " << ast_to_str(expected_type) << " but got " << ast_to_str(type) << "\n";
		throw std::runtime_error(err.str());
	}
}

std::shared_ptr<AST> get_var_from_value(Visitor visitor, std::string value)
{
	for (auto& def : visitor.variable_defs)
	{
		if (def->string_value == value)
		{
			return def;
		}
	}

	return nullptr;
}

std::shared_ptr<AST> get_var_from_name(Visitor visitor, std::string name)
{
	for (auto& def : visitor.variable_defs)
	{
		if (def->variable_definition_name == name || def->variable_name == name)
		{
			return def;
		}
	}

	return nullptr;
}


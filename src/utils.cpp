#include "../include/utils.h"

std::shared_ptr<AST> goto_root_of_var(Visitor visitor, std::string name)
{
	auto var = get_var_from_name(visitor, name);
	AstType type = var->type;
	while (type == AstType::AST_VARIABLE || type == AstType::AST_VARIABLE_DEFINITION)
	{
		if (var->variable_definition_value == nullptr) var = get_var_from_name(visitor, var->variable_name);

		switch (var->type)
		{
		case AstType::AST_VARIABLE_DEFINITION: var = var->variable_definition_value; type = AstType::AST_VARIABLE_DEFINITION; break;
		case AstType::AST_VARIABLE: var = goto_root_of_var(visitor, var->variable_definition_value->variable_name); type = AstType::AST_VARIABLE; break;
		default: type = AstType::AST_NOOP; break;
		}

		type = var->type;
	}

	return var;
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

	for (auto& strooct : visitor.struct_defs)
	{
		for (auto& item : strooct->struct_definition_members)
		{
			if (item->variable_definition_name == name) { return item; }
		}
	}

	return nullptr;
}

void modify_variable(Visitor visitor, std::shared_ptr<AST> variable, std::shared_ptr<AST> value)
{
	switch (value->variable_definition_value->type)
	{
	case AstType::AST_STRING: variable->variable_definition_value->string_value = value->variable_definition_value->string_value; break;
	case AstType::AST_INT: variable->variable_definition_value->int_value = value->variable_definition_value->int_value; break;
	case AstType::AST_VARIABLE: {
		auto temp = goto_root_of_var(visitor, value->variable_definition_value->variable_name);
		switch (temp->type)
		{
		case AstType::AST_INT: variable->variable_definition_value->int_value = temp->int_value; break;
		case AstType::AST_STRING: variable->variable_definition_value->string_value = temp->string_value; break;
		}
	}
	}
}


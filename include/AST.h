#pragma once
#include <string>
#include <memory>
#include <vector>

enum class AstType
{
	AST_VARIABLE_DEFINITION,
	AST_VARIABLE,
	AST_FUNCTION_DEFINITION,
	AST_FUNCTION_CALL,
	AST_STRING,
	AST_COMPOUND,
	AST_NOOP
};

// static std::string AstToStr(AstType type)
// {
// 	switch (type)
// 	{
// 	case AstType::AST_VARIABLE_DEFINITION:
// 		return "variable definition";
// 		break;
// 	case AstType::AST_VARIABLE:
// 		return "variable";
// 		break;
// 	case AstType::AST_FUNCTION_DEFINITION:
// 		return "function definition";
// 		break;
// 	case AstType::AST_FUNCTION_CALL:
// 		return "function call";
// 		break;
// 	case AstType::AST_STRING:
// 		return "string";
// 		break;
// 	case AstType::AST_COMPOUND:
// 		return "compound";
// 		break;
// 	case AstType::AST_NOOP:
// 		return "no operation";
// 		break;
// 	default:
// 		break;
// 	}
// }

struct AST
{
	AST(AstType type);

	AstType type{AstType::AST_NOOP};

	// variable def
	std::string variable_definition_name;
	std::shared_ptr<AST> variable_definition_value;

	// var
	std::string variable_name;

	// function definition
	std::string function_definition_name;
	std::shared_ptr<AST> function_definition_body;
	std::vector<std::shared_ptr<AST>> function_definition_params;

	// function call
	std::string function_call_name;
	std::vector<std::shared_ptr<AST>> function_call_args;

	// ast string
	std::string string_value;

	// ast compound
	std::vector<std::shared_ptr<AST>> compound_value;

	// error properties
	std::string error_line_contents;
	std::string error_arrow;
	unsigned error_line_num{ 0 };
	unsigned error_index{ 0 };
	unsigned error_start_index{ 0 };
};
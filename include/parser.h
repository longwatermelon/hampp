#pragma once
#include "AST.h"
#include "lexer.h"
#include "token.h"
struct Parser
{
	Token currentToken;
	Token prevToken;
	Lexer lexer;

	Parser(std::string ctns);

	void eat(TokenType token);

	std::shared_ptr<AST> parse();

private:
	std::shared_ptr<AST> parse_string();
	std::shared_ptr<AST> parse_int();
	std::shared_ptr<AST> parse_list();

	std::shared_ptr<AST> parse_expr();
	std::shared_ptr<AST> parse_function_call();
	std::shared_ptr<AST> parse_variable();

	std::shared_ptr<AST> parse_struct();
	void parse_struct_members(std::shared_ptr<AST> ast_struct);
	std::shared_ptr<AST> parse_instance();
	std::shared_ptr<AST> parse_instance_member(std::string instance_name);

	std::shared_ptr<AST> parse_variable_definition();
	std::shared_ptr<AST> parse_function_definition();

	std::shared_ptr<AST> parse_id();

	std::shared_ptr<AST> parse_statement();
	std::shared_ptr<AST> parse_statements();

	std::shared_ptr<AST> parse_conditional();

	std::shared_ptr<AST> parse_instance_member_modification();

	void init_error_values(std::shared_ptr<AST> node);
};
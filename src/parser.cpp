#include "../include/parser.h"
#include "../include/AST.h"
#include <iostream>
#include <sstream>
#include <map>

Parser::Parser(std::string ctns)
{
	lexer = Lexer{ ctns };
	currentToken = lexer.get_next_token();
	prevToken = currentToken;
}

void Parser::eat(TokenType token)
{
	if (currentToken.m_type == token)
	{
		prevToken = currentToken;
		currentToken = lexer.get_next_token();
	}
	else
	{
		std::string expected_token;
		std::map<TokenType, std::string> token_repr {
			{TokenType::TOKEN_COMMA, ","},
			{TokenType::TOKEN_EQUALS, "="},
			{TokenType::TOKEN_ID, "identifier"},
			{TokenType::TOKEN_INT, "int"},
			{TokenType::TOKEN_STRING, "string"},
			{TokenType::TOKEN_LBRACE, "{"},
			{TokenType::TOKEN_RBRACE, "}"},
			{TokenType::TOKEN_LBRACKET, "["},
			{TokenType::TOKEN_RBRACKET, "]"},
			{TokenType::TOKEN_PERIOD, "."},
			{TokenType::TOKEN_LPAREN, "("},
			{TokenType::TOKEN_RPAREN, ")"},
			{TokenType::TOKEN_SEMI, ";"}
		};
		std::stringstream msg;
		msg << "Unexpected token '" << lexer.contents.at(lexer.prev_index) << "' at line " << lexer.lineNum << ": \n" << lexer.collect_line() << "\n" << lexer.display_issue() << "\nExpected: " << token_repr[token] << std::endl;
		throw std::runtime_error(msg.str());
	}
}

std::shared_ptr<AST> Parser::parse()
{
	return parse_statements();
}

std::shared_ptr<AST> Parser::parse_string()
{
	const auto astString = std::make_shared<AST>(AstType::AST_STRING);
	astString->string_value = currentToken.m_value;
	eat(TokenType::TOKEN_STRING);
	return astString;
}

std::shared_ptr<AST> Parser::parse_int()
{
	const auto ast = std::make_shared<AST>(AstType::AST_INT);
	std::istringstream(currentToken.m_value) >> ast->int_value;
	eat(TokenType::TOKEN_INT);
	return ast;
}

std::shared_ptr<AST> Parser::parse_list()
{
	eat(TokenType::TOKEN_LBRACKET);
	const auto ast = std::make_shared<AST>(AstType::AST_LIST);
	while (currentToken.m_type != TokenType::TOKEN_RBRACKET)
	{
		switch (currentToken.m_type)
		{
		case TokenType::TOKEN_INT: ast->list_value.emplace_back(parse_int()); break;
		case TokenType::TOKEN_STRING: ast->list_value.emplace_back(parse_string()); break;
		}

		if (currentToken.m_type != TokenType::TOKEN_COMMA) { break; }
		eat(TokenType::TOKEN_COMMA);
	}
	eat(TokenType::TOKEN_RBRACKET);

	return ast;
}

std::shared_ptr<AST> Parser::parse_expr()
{
	switch (currentToken.m_type)
	{
		case TokenType::TOKEN_STRING: return parse_string();
		case TokenType::TOKEN_ID: return parse_id();
		case TokenType::TOKEN_INT: return parse_int();
		case TokenType::TOKEN_LBRACKET: return parse_list();
		case TokenType::TOKEN_PERIOD: return parse_instance_member(prevToken.m_value);
	}

	return std::make_shared<AST>(AstType::AST_NOOP);
}

std::shared_ptr<AST> Parser::parse_id()
{
	if (currentToken.m_value == "def")
	{
		return parse_variable_definition();
	}
	else if (currentToken.m_value == "func")
	{
		return parse_function_definition();
	}
	else if (currentToken.m_value == "if")
	{
		return parse_conditional();
	}
	else if (currentToken.m_value == "strooct")
	{
		return parse_struct();
	}
	else if (currentToken.m_value == "new")
	{
		return parse_instance();
	}
	else
	{
		return parse_variable();
	}

	return nullptr;
}

std::shared_ptr<AST> Parser::parse_variable_definition()
{
	const auto retVal = std::make_shared<AST>(AstType::AST_VARIABLE_DEFINITION);
	init_error_values(retVal);
	eat(TokenType::TOKEN_ID);
	const auto varName = currentToken.m_value;
	eat(TokenType::TOKEN_ID);

	if (currentToken.m_type == TokenType::TOKEN_PERIOD) // struct member modification
	{
		const auto ast = parse_instance_member_modification();
		init_error_values(ast);

		return ast;
	}

	eat(TokenType::TOKEN_EQUALS);
	
	retVal->variable_definition_name = varName;
	retVal->variable_definition_value = parse_expr();

	
	return retVal;
}

std::shared_ptr<AST> Parser::parse_function_definition()
{
	const auto ast = std::make_shared<AST>(AstType::AST_FUNCTION_DEFINITION);

	init_error_values(ast);
	eat(TokenType::TOKEN_ID); // func kw
	eat(TokenType::TOKEN_ID); // func name

	ast->function_definition_name = prevToken.m_value;

	eat(TokenType::TOKEN_LPAREN);
	if (currentToken.m_type != TokenType::TOKEN_RPAREN)
	{
		std::shared_ptr<AST> arg = parse_variable();
		ast->function_definition_params.emplace_back(arg);

		while (currentToken.m_type == TokenType::TOKEN_COMMA)
		{
			eat(TokenType::TOKEN_COMMA);

			auto arg = parse_variable();
			ast->function_definition_params.emplace_back(arg);
		}
	}
	

	eat(TokenType::TOKEN_RPAREN);

	eat(TokenType::TOKEN_LBRACE);

	ast->function_definition_body = parse_statements();
	
	eat(TokenType::TOKEN_RBRACE);

	

	return ast;
}

std::shared_ptr<AST> Parser::parse_function_call()
{
	const auto functionCall = std::make_shared<AST>(AstType::AST_FUNCTION_CALL);

	functionCall->function_call_name = prevToken.m_value;
	init_error_values(functionCall);
	eat(TokenType::TOKEN_LPAREN);
	std::shared_ptr<AST> expr = parse_expr();
	functionCall->function_call_args.emplace_back(expr);

	while (currentToken.m_type == TokenType::TOKEN_COMMA)
	{
		eat(TokenType::TOKEN_COMMA);

		expr = parse_expr();
		functionCall->function_call_args.emplace_back(expr);
	}

	eat(TokenType::TOKEN_RPAREN);

	return functionCall;
}

std::shared_ptr<AST> Parser::parse_variable()
{
	const std::string tokenValue = currentToken.m_value;
	const auto ast = std::make_shared<AST>(AstType::AST_VARIABLE);
	init_error_values(ast);
	eat(TokenType::TOKEN_ID);

	if (currentToken.m_type == TokenType::TOKEN_LPAREN)
	{
		return parse_function_call();
	}

	if (currentToken.m_type == TokenType::TOKEN_PERIOD)
	{
		return parse_instance_member(prevToken.m_value);
	}

	ast->variable_name = tokenValue;

	return ast;
}

std::shared_ptr<AST> Parser::parse_struct()
{
	const auto ast_struct = std::make_shared<AST>(AstType::AST_STRUCT);
	init_error_values(ast_struct);
	eat(TokenType::TOKEN_ID); // struct kw

	ast_struct->struct_definition_name = currentToken.m_value;
	eat(TokenType::TOKEN_ID); // struct name

	eat(TokenType::TOKEN_LBRACE);

	parse_struct_members(ast_struct);

	eat(TokenType::TOKEN_RBRACE);

	return ast_struct;
}

void Parser::parse_struct_members(std::shared_ptr<AST> ast_struct)
{
	std::shared_ptr<AST> statement = parse_statement();
	if (statement->type != AstType::AST_VARIABLE_DEFINITION)
	{
		std::stringstream err;
		err << "\nOnly variable definitions are allowed inside of a strooct.\n";
		throw std::runtime_error(err.str());
	}
	ast_struct->struct_definition_members.emplace_back(statement);
	
	while (currentToken.m_type != TokenType::TOKEN_RBRACE)
	{
		eat(TokenType::TOKEN_SEMI);

		statement = parse_statement();

		if (statement != nullptr) 
		{
			if (statement->type != AstType::AST_VARIABLE_DEFINITION)
			{
				std::stringstream err;
				err << "\nOnly variable definitions are allowed inside of a strooct.\n";
				throw std::runtime_error(err.str());
			}

			ast_struct->struct_definition_members.emplace_back(statement); 
		}
	}
}

std::shared_ptr<AST> Parser::parse_instance()
{
	const auto instance = std::make_shared<AST>(AstType::AST_STRUCT_INSTANCE);
	eat(TokenType::TOKEN_ID);
	instance->instance_struct_reference_name = currentToken.m_value;
	eat(TokenType::TOKEN_ID);
	return instance;
}

std::shared_ptr<AST> Parser::parse_instance_member(std::string instance_name)
{
	eat(TokenType::TOKEN_PERIOD);
	std::string member_name = currentToken.m_value;
	const auto ast_instance_member = std::make_shared<AST>(AstType::AST_INSTANCE_MEMBER);
	ast_instance_member->instance_member_name = member_name;
	ast_instance_member->instance_member_instance_name = instance_name;
	eat(TokenType::TOKEN_ID);
	return ast_instance_member;
}

std::shared_ptr<AST> Parser::parse_statement()
{
	switch (currentToken.m_type)
	{
		case TokenType::TOKEN_ID: return parse_id();
		case TokenType::TOKEN_STRING: return parse_string();
		case TokenType::TOKEN_INT: return parse_int();
		case TokenType::TOKEN_LBRACKET: return parse_list();
		default: break;
	}

	return nullptr;
}

std::shared_ptr<AST> Parser::parse_statements()
{
	const auto ast = std::make_shared<AST>(AstType::AST_COMPOUND);
	
	std::shared_ptr<AST> statement = parse_statement();

	ast->compound_value.emplace_back(statement);

	//while (currentToken.m_type == TokenType::TOKEN_SEMI)
	while (lexer.index < lexer.contents.size())
	{
		eat(TokenType::TOKEN_SEMI);

		statement = parse_statement();
		if (statement != nullptr)
		{
			init_error_values(statement);
			ast->compound_value.emplace_back(statement);
		}
		else
		{
			break;
		}
	}

	return ast;
}

std::shared_ptr<AST> Parser::parse_conditional()
{
	const auto ast = std::make_shared<AST>(AstType::AST_CONDITIONAL);
	eat(TokenType::TOKEN_ID);
	eat(TokenType::TOKEN_LPAREN);

	const auto boolval = std::make_shared<AST>(AstType::AST_BOOL);
	boolval->conditional_condition = parse_expr();
	ast->conditional_condition = boolval;

	eat(TokenType::TOKEN_RPAREN);

	eat(TokenType::TOKEN_LBRACE);

	ast->conditional_body = parse_statements();

	eat(TokenType::TOKEN_RBRACE);
	
	return ast;
}

std::shared_ptr<AST> Parser::parse_instance_member_modification()
{
	const auto ast = std::make_shared<AST>(AstType::AST_INSTANCE_MEMBER_MODIFICATION);

	// current token is period
	ast->modified_instance_name = prevToken.m_value;
	eat(TokenType::TOKEN_PERIOD);
	ast->modified_member_name = currentToken.m_value;
	eat(TokenType::TOKEN_ID);

	eat(TokenType::TOKEN_EQUALS);

	ast->modified_member_value = parse_expr();

	return ast;
}

void Parser::init_error_values(std::shared_ptr<AST> node)
{
	node->error_arrow = lexer.display_issue();
	node->error_line_contents = lexer.collect_line();
	node->error_index = lexer.index;
	node->error_start_index = lexer.startIndex;
	node->error_line_num = lexer.lineNum;
}

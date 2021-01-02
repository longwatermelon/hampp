#include "../include/parser.h"
#include "../include/AST.h"
#include <iostream>
#include <sstream>

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
		std::stringstream msg;
		msg << "Unexpected token '" << lexer.contents.at(lexer.prev_index) << "' at line " << lexer.lineNum << ": \n" << lexer.collect_line() << "\n" << lexer.display_issue() << std::endl;
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

std::shared_ptr<AST> Parser::parse_expr()
{
	switch (currentToken.m_type)
	{
		case TokenType::TOKEN_STRING: return parse_string();
		case TokenType::TOKEN_ID: return parse_id();
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

	std::shared_ptr<AST> arg = parse_variable();
	ast->function_definition_params.emplace_back(arg);

	while (currentToken.m_type == TokenType::TOKEN_COMMA)
	{
		eat(TokenType::TOKEN_COMMA);

		auto arg = parse_variable();
		ast->function_definition_params.emplace_back(arg);
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

	ast->variable_name = tokenValue;	

	return ast;
}

std::shared_ptr<AST> Parser::parse_statement()
{
	switch (currentToken.m_type)
	{
		case TokenType::TOKEN_ID: return parse_id();
		case TokenType::TOKEN_STRING: return parse_string();
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

void Parser::init_error_values(std::shared_ptr<AST> node)
{
	node->error_arrow = lexer.display_issue();
	node->error_line_contents = lexer.collect_line();
	node->error_index = lexer.index;
	node->error_start_index = lexer.startIndex;
	node->error_line_num = lexer.lineNum;
}

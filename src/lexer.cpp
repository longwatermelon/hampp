#include "../include/lexer.h"
#include <stdexcept>
#include <ctype.h>
#include <sstream>
#include <iostream>

Lexer::Lexer() = default;

Lexer::Lexer(std::string ctn)
	: contents(std::move(ctn))
{
	if (!contents.empty())
	{
		currentCharacter = contents.at(index);
	}
	else
	{
		std::cout << "Could not find file.\n";
		exit(0);
	}
}

void Lexer::advance()
{
	if (index < contents.size() - 1)
	{
		++index;
		currentCharacter = contents.at(index);
	}
}

void Lexer::skip_whitespace()
{
	while(isspace(currentCharacter))
	{
		advance();
	}
}

std::string Lexer::collect_string()
{
	const auto currentIndex = index + 1;
	advance();
	while (currentCharacter != '"' && index < contents.size() && currentCharacter != 10)
	{
		advance();
	}
	const auto retVal = contents.substr(currentIndex, index - currentIndex);
	advance();
	return retVal;
}

std::string Lexer::collect_int()
{
	std::string integer;
	while (isdigit(currentCharacter))
	{
		integer += currentCharacter;
		advance();
	}

	return integer;
}

std::string Lexer::collect_single_char_string()
{
	const auto retVal = std::string(1, currentCharacter);
	advance();
	return retVal;
}

std::string Lexer::collect_id()
{
	const auto currentIndex = index;
	while ((isalnum(currentCharacter) || currentCharacter == '_') && index < contents.size())
	{
		advance();
	}
	const auto retVal = contents.substr(currentIndex, index - currentIndex);
	return retVal;
}

Token Lexer::get_next_token()
{
	while (index < contents.size() - 1)
	{
		prev_index = index;
		if (isspace(currentCharacter) && currentCharacter != 10)
		{
			skip_whitespace();
		}

		if (isdigit(currentCharacter))
		{
			return Token(TokenType::TOKEN_INT, collect_int());
		}

		switch (currentCharacter)
		{
		case ';': return Token(TokenType::TOKEN_SEMI, collect_single_char_string());
		case '=': return Token(TokenType::TOKEN_EQUALS, collect_single_char_string());
		case '(': return Token(TokenType::TOKEN_LPAREN, collect_single_char_string());
		case ')': return Token(TokenType::TOKEN_RPAREN, collect_single_char_string());
		case ',': return Token(TokenType::TOKEN_COMMA, collect_single_char_string());
		case '{': return Token(TokenType::TOKEN_LBRACE, collect_single_char_string());
		case '}': return Token(TokenType::TOKEN_RBRACE, collect_single_char_string());
		case '[': return Token(TokenType::TOKEN_LBRACKET, collect_single_char_string());
		case ']': return Token(TokenType::TOKEN_RBRACKET, collect_single_char_string());
		case '.': return Token(TokenType::TOKEN_PERIOD, collect_single_char_string());
		case '"': return Token(TokenType::TOKEN_STRING, collect_string());
		default:
			if (isalnum(currentCharacter))
			{
				return Token(TokenType::TOKEN_ID, collect_id());
			}			
			break;
		}
		advance();
		startIndex = index;
		++lineNum;
	}
	return {TokenType::TOKEN_EOF, ""};
}

std::string Lexer::collect_line()
{
	unsigned idx = index;
	while ((contents.at(idx)) != 10 && idx < contents.size() - 1)
	{
		++idx;
	}

	const std::string str = contents.substr(startIndex, idx - startIndex);

	return str;
}

std::string Lexer::display_issue()
{
	std::string str;
	for (unsigned i = startIndex; i < prev_index; ++i)
	{
		str += ' ';
	}
	str += '^';

	return str;
}
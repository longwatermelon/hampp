#pragma once
#include <string>

enum class TokenType
{
    TOKEN_ID, // 0
    TOKEN_SEMI, // 1
    TOKEN_LPAREN, // 2
    TOKEN_RPAREN, // 3
    TOKEN_EQUALS, // 4
    TOKEN_STRING, // 5
    TOKEN_COMMA, // 6
    TOKEN_LBRACE, // 7
    TOKEN_RBRACE, // 8
    TOKEN_EOF // 9
};

struct Token
{
    Token() = default;
    Token(TokenType type, std::string value);
    std::string m_value;
    TokenType m_type{ TokenType::TOKEN_EOF };
};


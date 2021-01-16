#pragma once
#include <string>

enum class TokenType
{
    TOKEN_ID,
    TOKEN_SEMI,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EQUALS,
    TOKEN_STRING,
    TOKEN_INT,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_PERIOD,
    TOKEN_EOF
};

struct Token
{
    Token() = default;
    Token(TokenType type, std::string value);
    std::string m_value;
    TokenType m_type{ TokenType::TOKEN_EOF };
};


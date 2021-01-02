#include "../include/token.h"

Token::Token(TokenType type, std::string value)
    : m_value{std::move(value) }, m_type(type)
{}


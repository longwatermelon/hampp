#pragma once
#include "token.h"
#include <string>

struct Lexer
{
    char currentCharacter{ ' ' };
    unsigned int index{ 0 };
    std::string contents;
    unsigned lineNum{ 1 };
    unsigned startIndex{ 0 };
    unsigned prev_index{ 0 };

    Lexer();
    Lexer(std::string contents);

    void advance();

    void skip_whitespace();

    std::string collect_string();

    std::string collect_int();

    std::string collect_single_char_string();

    std::string collect_id();

    Token get_next_token();

    std::string collect_line();

    std::string display_issue();
};


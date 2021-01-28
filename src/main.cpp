#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/visitor.h"
#include <fstream>
#include <sstream>
#include <iostream>


std::string getTokenName(TokenType type)
{
    switch (type)
    {
    case TokenType::TOKEN_ID:
        return "ID";
    case TokenType::TOKEN_SEMI:
        return "SemiColen";
    case TokenType::TOKEN_LPAREN:
        return "LeftParen";
    case TokenType::TOKEN_RPAREN:
        return "RightParen";
    case TokenType::TOKEN_EQUALS:
        return "Equal";
    case TokenType::TOKEN_STRING:
        return "String";
    case TokenType::TOKEN_COMMA:
        return "Comma";
    case TokenType::TOKEN_LBRACE:
        return "LeftBrace";
    case TokenType::TOKEN_RBRACE:
        return "RightBrace";
    case TokenType::TOKEN_EOF:
        return "EndOfFile";
    default:
        break;
    }
    return "";
}


std::string read_file(std::string fp)
{
    std::ifstream infile;
    infile.open(fp);
    std::stringstream contents;
    std::string line;
    while (std::getline(infile, line)) contents << line << "\n";
    
    infile.close();
    return contents.str();
}


void run_file(Parser* parser, Visitor* visitor, std::string contents)
{
    std::shared_ptr<AST> root = parser->parse();
    visitor->visit(root);
}


int main(int argc, char* argv[])
{
    std::string contents = read_file(argv[1]);
    Parser parser(contents);
    Visitor visitor{};
   
    try
    {
        run_file(&parser, &visitor, contents);
    }
    catch (const std::runtime_error& ex)
    {
        std::cout << ex.what();
    }

    return 0;
}
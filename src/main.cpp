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

int main(int argc, char* argv[])
{
    // printf("%s\n", get_file_contents(argv[1]));
    std::ifstream infile;
    infile.open(argv[1]);
    std::stringstream contents;
    std::string line;
    while (std::getline(infile, line))
    {
        //std::cout << line << "\n";
        contents << line << std::endl;
    }
    infile.close();

    //Parser parser("def name = \"john\";\nprount(\"hello world\");\nprount(name);");
   
    try
    {
        Parser parser(contents.str());
        std::shared_ptr<AST> root = parser.parse();
        Visitor visitor = Visitor();
        visitor.visit(root);
    }
    catch (const std::runtime_error& ex)
    {
        std::cout << ex.what();
    }
    /*Lexer lexer(contents.str());
    while (lexer.index < lexer.contents.size())
    {
        const auto token = lexer.get_next_token();
        std::cout << getTokenName(token.m_type) << ": " << token.m_value
            << std::endl << "========================" << std::endl;
    }*/
    //parser_T* parser = init_parser(lexer);
    //// printf("creating root node\n");
    //AST_T* root = parser_parse(parser);
    //// printf("creating visitor\n");
    //visitor_T* visitor = init_visitor();
    //// printf("visiting\n");
    //visitor_visit(visitor, root);

    // printf("%d\n", root->type);
    // printf("%d\n", root->compound_size);

    return 0;
}
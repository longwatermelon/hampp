#pragma once
#include "AST.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "visitor.h"
#include <sstream>
#include <stdexcept>

std::shared_ptr<AST> goto_root_of_var(struct Visitor visitor, std::string name);
void eat_type(AstType type, AstType expected_type, std::shared_ptr<AST> node);

std::shared_ptr<AST> get_var_from_value(struct Visitor visitor, std::string value);
std::shared_ptr<AST> get_var_from_name(struct Visitor visitor, std::string name);
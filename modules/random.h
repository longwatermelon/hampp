#pragma once
#include "../include/utils.h"

std::shared_ptr<AST> builtin_function_random_randint(std::vector<std::shared_ptr<AST>> args, std::shared_ptr<AST> node, struct Visitor visitor);

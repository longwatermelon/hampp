#include "../modules/random.h"
#include <ctime>


std::shared_ptr<AST> builtin_function_random_randint(std::vector<std::shared_ptr<AST>> args, std::shared_ptr<AST> node, struct Visitor visitor)
{
	if (args.size() != 2)
	{
		std::stringstream err;
		err << "Line " << node->error_line_num << ":\n" << node->error_line_contents << "\n"
			<< "random_randint takes 2 arguments (min, max) but " << args.size() << " arguments were supplied.\n";
		throw std::runtime_error(err.str());
	}

	int min, max;
	AstType type = AstType::AST_VARIABLE;
	std::shared_ptr<AST> var1, var2;
	if (args[0]->type == AstType::AST_VARIABLE)
	{
		var1 = goto_root_of_var(visitor, args[0]->variable_name);
		min = var1->variable_definition_value->int_value;
		eat_type(var1->variable_definition_value->type, AstType::AST_INT, node);
	}
	else
	{
		min = args[0]->int_value;
		eat_type(args[0]->type, AstType::AST_INT, node);
	}

	if (args[1]->type == AstType::AST_VARIABLE)
	{
		var2 = goto_root_of_var(visitor, args[1]->variable_name);
		max = var2->variable_definition_value->int_value;
		eat_type(var2->variable_definition_value->type, AstType::AST_INT, node);
	}
	else
	{
		max = args[1]->int_value;
		eat_type(args[1]->type, AstType::AST_INT, node);
	}

	std::shared_ptr<AST> result = std::make_shared<AST>(AstType::AST_INT);

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	result->int_value = std::rand() % max + min;

	return result;
}

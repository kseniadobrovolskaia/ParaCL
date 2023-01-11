#include "Lexer.hpp"


//std::vector<Lex_t*> lex_string(std::vector<std::string> &vars);
void print_nodes(std::vector<Lex_t*> &nodes, std::vector<std::string> &vars);


int main(int argc, char const *argv[])
{
	try
	{
		std::vector<std::string> vars;
		std::vector<Lex_t*> nodes = lex_string(vars);

		while (!nodes.empty())
		{
			print_nodes(nodes, vars);
			nodes = lex_string(vars);
		}

	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


void print_nodes(std::vector<Lex_t*> &nodes, std::vector<std::string> &vars)
{
	for (auto elem = nodes.begin(); elem < nodes.end(); ++elem)
	{
		std::cout << (*elem)->name();
		if ((*elem)->get_kind() == 1)
		{
			std::cout << vars[(*elem)->get_data()];
		}
		std::cout << " ";
	}
	std::cout << std::endl;
}


#include "Parser.hpp"
#include "Sintax_graph.hpp"


void print_lexems(std::vector<Lex_t*> &lexems, std::vector<std::string> &vars);



int main(int argc, char const *argv[])
{
	try
	{
		std::vector<std::string> vars;
		std::vector<Lex_t*> lexems = lex_string(vars); //(7 - 85+(8-9*6)/1 +8)*2-4 <= 0;.

		//print_lexems(lexems, vars);

		check_brases(lexems);

		Lex_t *sintax_tree = parse_arithmetic(lexems);
		build_sintax_graph(sintax_tree);

		int result = calculate(sintax_tree);
		std::cout << "Значение арифметического выражения: " << result << std::endl;
		system ("dot sintax_tree.txt -Tpng -o sintax_tree.png\n"
				"shotwell sintax_tree.png");
	
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


void print_lexems(std::vector<Lex_t*> &lexems, std::vector<std::string> &vars)
{
	for (auto elem = lexems.begin(); elem < lexems.end(); ++elem)
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


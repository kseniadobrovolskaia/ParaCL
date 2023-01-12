#include "Parser.hpp"


//std::vector<Lex_t*> lex_string(std::vector<std::string> &vars);
void print_lexems(std::vector<Lex_t*> &lexems, std::vector<std::string> &vars);



int main(int argc, char const *argv[])
{
	try
	{
		std::vector<std::string> vars;
		std::vector<Lex_t*> lexems = lex_string(vars);

		print_lexems(lexems, vars);

		check_brases(lexems);

		Lex_t *sintax_tree = Parse_arithmetic(lexems);

		int result = Calculate_arithmetic(sintax_tree);
		std::cout << "Значение арифметического выражения: " << result << std::endl;

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


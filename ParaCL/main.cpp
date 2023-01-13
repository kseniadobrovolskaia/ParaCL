
#include "Sintax_graph.hpp"
#include "Parser_stmts.hpp"
#include "Parser_arith.hpp"

void print_lexems(std::vector<Lex_t*> &lexems, std::vector<std::string> &vars);
void print_VARS();
void print_prog_elems(std::vector<Statement*> prog);



int main(int argc, char const *argv[])
{
	try
	{
		std::vector<Lex_t*> lexems = lex_string(vars);

		std::vector<Statement*> prog = parse_prog(lexems);
		build_sintax_graph(prog);
		//print_prog_elems(prog);
		print_VARS();

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


void print_VARS()
{
	std::cout << "Значения глобальных переменных: " << std::endl;

	for (auto const &pair : VARS)
	{
		std::cout << pair.first << " = " << pair.second << std::endl;
	}
}


void print_prog_elems(std::vector<Statement*> prog)
{
	std::cout << "Элементы программы :" << std::endl;
	for (auto elem : prog)
	{
		std::cout << (elem->get_lhs())->name() << std::endl;
	}
}
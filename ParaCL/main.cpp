
#include "Sintax_graph.hpp"
#include "Run_program.hpp"

void print_VARS();
void print_lexems(std::vector<Lex_t*> &lexems);
void print_prog_elems(std::vector<Statement*> prog);


int main(int argc, char const *argv[])
{
	try
	{
		std::vector<Lex_t*> lexems = lex_string(vars);
		program_size = lexems.size();

		std::vector<Statement*> prog = parse_program(lexems);
		build_sintax_graph(prog);

		run_program(prog);

		//system ("dot sintax_tree.txt -Tpng -o sintax_tree.png\n"
		//		"shotwell sintax_tree.png");
	
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


//--------------------------------------------DEBUG--------------------------------------------------------


void print_lexems(std::vector<Lex_t*> &lexems)
{
	for (auto elem = lexems.begin(); elem < lexems.end(); ++elem)
	{
		std::cout << (*elem)->name();
		std::cout << " ";
	}
	std::cout << std::endl;
}



void print_prog_elems(std::vector<Statement*> prog)
{
	std::cout << "Утверждения программы :" << std::endl;
	for (auto elem : prog)
	{
		std::cout << elem->name() << std::endl;
	}
}


void print_VARS()
{
	std::cout << "Значения глобальных переменных: " << std::endl;

	for (auto const &pair : VARS)
	{
		std::cout << pair.first << " = " << pair.second << std::endl;
	}
}


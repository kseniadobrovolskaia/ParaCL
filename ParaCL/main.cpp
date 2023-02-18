#include "Sintax_graph.hpp"
#include "Run_program.hpp"


void print_lex_array(std::vector<Lex_t*> &lex_array);
void print_prog_elems(std::vector<Statement*> prog);


int main(int argc, char const *argv[])
{
	try
	{
		if (argc > 1)
		{
			std::ifstream data;

			data.open(argv[1]);
			if (!(data.is_open()))
			{
				std::cerr << "File \"" << argv[1] << "\" did not open" << std::endl;
				exit(EXIT_FAILURE);
			}

			lex_array = lex_string(data);
		}
		else
		{
			lex_array = lex_string(std::cin);
		}

		Scope *prog = parse_program(lex_array);
		build_sintax_graph(prog->get_lhs());

		run_program(prog->get_lhs(), std::cin, std::cout);

		//system ("dot sintax_tree.txt -Tpng -o sintax_tree.png\n"
				//cd "shotwell sintax_tree.png");
	
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


//--------------------------------------------DEBUG--------------------------------------------------------


void print_lex_array(std::vector<Lex_t*> &lex_array)
{
	for (auto elem = lex_array.begin(); elem < lex_array.end(); ++elem)
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



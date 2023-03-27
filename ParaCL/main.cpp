#include "Sintax_graph.hpp"
#include "Scope_table.hpp"


//std::unordered_map<std::string, int>VARS;		//initialized variables and their values
Scope_table *CURR_SCOPE;

void print_lex_array(std::vector<Lex_t*> &lex_array);
void print_prog_elems(std::vector<Statement*> prog);


int main(int argc, char const *argv[])
{
	try
	{
		CURR_SCOPE = new Scope_table();

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

		Lex_t *prog = parse_scope(lex_array);
		//build_sintax_graph(static_cast<Scope*>(prog)->get_lhs());

		//print_prog_elems(static_cast<Scope*>(prog)->get_lhs());
		run_program(prog, std::cin, std::cout);

		//system ("dot sintax_tree.txt -Tpng -o sintax_tree.png\n"
				// "shotwell sintax_tree.png");
	
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



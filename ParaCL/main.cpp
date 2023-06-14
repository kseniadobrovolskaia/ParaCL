#include "Parser_stmts.hpp"

void print_lex_array(std::vector<std::shared_ptr<Lex_t>> &lex_array);
void print_prog_elems(std::vector<std::shared_ptr<Statement>> prog);
void build_sintax_graph(std::vector<std::shared_ptr<Statement>> prog);


int main(int argc, char const *argv[])
{
	try
	{
		CURR_SCOPE = std::make_shared<Scope_table>();

		if (argc > 1)
		{
			std::ifstream data;

			data.open(argv[1]);
			if (!(data.is_open()))
			{
				std::cerr << "File \"" << argv[1] << "\" did not open" << std::endl;
				exit(EXIT_FAILURE);
			}

			lex_array = std::make_shared<Lex_array_t>(data);
		}
		else
		{
			lex_array = std::make_shared<Lex_array_t>(std::cin);
		}

		//lex_array->print();

		std::shared_ptr<Lex_t> prog = parse_scope();

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


void print_prog_elems(std::vector<std::shared_ptr<Statement>> prog)
{
	std::cout << "Утверждения программы :" << std::endl;
	for (auto &&elem : prog)
	{
		std::cout << elem->name() << "\n";
	}
}

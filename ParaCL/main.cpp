#include "Parser_stmts.hpp"


void build_sintax_graph(std::shared_ptr<Lex_array_t> lex_array, std::vector<std::shared_ptr<Statement>> prog);



int main(int argc, char const *argv[])
{
	try
	{
		bool CODEGEN = 0;
		AST_creator creator;

		if (argc > 1)
		{
			if (!strcmp(argv[1],"CODEGEN"))
			{
				CODEGEN = 1;
				creator.lexical_analysis(std::cin);
			}
			else
			{
				std::ifstream data;

				data.open(argv[1]);
				if (!(data.is_open()))
				{
					std::cerr << "File \"" << argv[1] << "\" did not open\n";
					exit(EXIT_FAILURE);
				}

				creator.lexical_analysis(data);
			}
		}
		else
		{
			creator.lexical_analysis(std::cin);
		}

		creator.parsing();

		std::string file_name = "../../llvmIR.txt";
		std::ofstream llvmIR;

		llvmIR.open(file_name);
		if (!(llvmIR.is_open()))
		{
		  std::cerr << "File \"" << file_name << "\" did not open\n";
		  exit(EXIT_FAILURE);
		}
		
		creator.codegen(llvmIR);

		if (!CODEGEN)
		{
			creator.run_program(std::cin, std::cout);
		}

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


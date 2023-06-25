#include "Parser_stmts.hpp"




int main(int argc, char const *argv[])
{
	try
	{
		bool CODEGEN = 0;///Print dump to file without execution
		AST_creator creator;

		if (argc > 1)
		{
			bool from_file = 0;

			if (!strcmp(argv[1],"CODEGEN"))
			{
				CODEGEN = 1;
				if (argc > 2)
					from_file = 1;
			}

			std::ifstream data;

			if (!CODEGEN || from_file)
			{
				data.open(argv[from_file + 1]);
				if (!(data.is_open()))
				{
					std::cerr << "File \"" << argv[from_file + 1] << "\" did not open\n";
					exit(EXIT_FAILURE);
				}
				creator.lexical_analysis(data);
			}
			else
			{
				creator.lexical_analysis(std::cin);
			}
		}
		else
		{
			creator.lexical_analysis(std::cin);
		}


		creator.parsing();

		std::string file_name = "llvmIR.txt";

		creator.codegen(file_name);

		if (!CODEGEN)
		{
			creator.run_program(std::cin, std::cout);
		}
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


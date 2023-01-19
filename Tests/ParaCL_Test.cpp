#include "Run_program.hpp"


int main()
{
	for (int num_test = 1; num_test < 3; num_test++)
	{
		try
		{
			std::ifstream data;

			std::string name_data = std::to_string(num_test) + "_test_data.txt";
			std::string name_results = std::to_string(num_test) + "_test_results.txt";

			data.open(name_data);
			if (!(data.is_open()))
		  {
		    std::cerr << "File \"" << name_data << "\" did not open" << std::endl;
		    exit(EXIT_FAILURE);
		  }

			std::ofstream results;

			results.open(name_results);
			if (!(results.is_open()))
			{
			  std::cerr << "File \"" << name_results << "\" did not open" << std::endl;
			  exit(EXIT_FAILURE);
			}
			
			
				data >> std::noskipws;
		
				std::vector<Lex_t*> lexems = lex_string(vars, data);
				
				program_size = lexems.size();
				
				std::vector<Statement*> prog = parse_program(lexems);
				
				run_program(prog, data, results);
			
		}
		catch(std::exception & ex)
		{
			std::cerr << ex.what() << std::endl;
			exit(EXIT_FAILURE);
		}

		token_counter(RESET);
	}

	return 0;
}


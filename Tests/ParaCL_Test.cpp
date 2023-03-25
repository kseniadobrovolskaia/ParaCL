#include "Run_program.hpp"


void clean_all_global_arrays();


int main()
{
	try
	{
		for (int num_test = 1; num_test < 6; num_test++)
		{
			std::cout << "Start test number " << num_test << std::endl;
			std::string name_data = "tests/data/" + std::to_string(num_test) + "_test_data.txt";
			std::string name_results = "tests/results/" + std::to_string(num_test) + "_test_results.txt";
			std::string name_input = "tests/input/" + std::to_string(num_test) + "_test_input.txt";

			std::ifstream data;

			data.open(name_data);
			if (!(data.is_open()))
			{
			  std::cerr << "File \"" << name_data << "\" did not open" << std::endl;
			  exit(EXIT_FAILURE);
			}

			std::ifstream input;

			input.open(name_input);
			if (!(input.is_open()))
			{
			  std::cerr << "File \"" << name_input << "\" did not open" << std::endl;
			  exit(EXIT_FAILURE);
			}

			std::ofstream results;

			results.open(name_results);
			if (!(results.is_open()))
			{
			  std::cerr << "File \"" << name_results << "\" did not open" << std::endl;
			  exit(EXIT_FAILURE);
			}

			std::vector<Lex_t*> lexems = lex_string(data);
			
			Lex_t *prog = parse_scope(lexems);
			
			run_program(prog, input, results);
			clean_all_global_arrays();
		}
	}
	catch(std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	
	for (int num_test = 1; num_test < 13; num_test++)
	{	
		std::cout << "Start input test number " << num_test << std::endl;

		std::string name_input = "input_tests/data/" + std::to_string(num_test) + "_input_test.txt";
		std::string name_results = "input_tests/results/" + std::to_string(num_test) + "_input_results.txt";

		std::ifstream input_data;

		input_data.open(name_input);
		if (!(input_data.is_open()))
	  {
	    std::cerr << "File \"" << name_input << "\" did not open" << std::endl;
	    exit(EXIT_FAILURE);
	  }

	  std::ofstream results;
		results.open(name_results);
		if (!(results.is_open()))
		{
		  std::cerr << "File \"" << name_results << "\" did not open" << std::endl;
		  exit(EXIT_FAILURE);
		}

	  try
	  {
	  	clean_all_global_arrays();
	  	
	  	input_data >> std::noskipws;
			
	  	std::vector<Lex_t*> lexems = lex_string(input_data);
	 
			Lex_t *prog = parse_scope(lexems);
			
			run_program(prog, input_data, results);
	  }
	  catch(std::exception & ex)
	  {
	  	std::string err_mess = ex.what();

	  	int start = err_mess.find("Error in row number");

	  	std::string error = err_mess.substr(start);

	  	results << error;
	  	results.close();
	  	input_data.clear();
	  }
	}

	return 0;
}


void clean_all_global_arrays()
{
	token_counter(RESET);
	lex_array.clear();
	EoF = 0;
	MAIN = 1;
	VARS.clear();
	vars.clear();
}

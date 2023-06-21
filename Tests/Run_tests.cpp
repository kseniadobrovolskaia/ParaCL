#include "Parser_stmts.hpp"


void set_main()
{
	parse_scope(nullptr, 1); //This means MAIN = 1
}


void Run_tests()
{
	try
	{
		AST_creator creator;

		for (int num_test = 1; num_test < 16; num_test++)
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

			creator.lexical_analysis(data);

			creator.parsing();
			
			creator.run_program(input, results);
			set_main();
		}
	}
	catch(std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	
	for (int num_test = 1; num_test < 21; num_test++)
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
			AST_creator creator;

			input_data >> std::noskipws;
			
			creator.lexical_analysis(input_data);

			creator.parsing();
			
			creator.run_program(input_data, results);
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

		set_main();
	}
}


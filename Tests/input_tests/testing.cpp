#include <iostream>
#include <fstream>
#include <string>



int main()
{
	for (int num_test = 13; num_test < 21; num_test++)
	{
		std::string name_data = "data/" + std::to_string(num_test) + "_input_test.txt";
		std::string name_input = "answers/" + std::to_string(num_test) + "_input_answer.txt";


		std::ofstream data, input;

	
		data.open(name_data);
		if (!(data.is_open()))
		{
		  std::cerr << "File \"" << name_data << "\" did not open" << std::endl;
		  exit(EXIT_FAILURE);
		}

		input.open(name_input);
		if (!(input.is_open()))
		{
		  std::cerr << "File \"" << name_input << "\" did not open" << std::endl;
		  exit(EXIT_FAILURE);
		}


	}

	return 0;

}

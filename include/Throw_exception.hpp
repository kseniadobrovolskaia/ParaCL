#ifndef THROW_EXCEPTION_H
#define THROW_EXCEPTION_H


#include "Lexer.hpp"


//------------------------------------------Throw_exception-------------------------------------------------------


void throw_exception(std::string mess, int error_elem)
{
	int is_last_elem = 0;
	std::string command = mess;

	int program_size = lex_array.size();

	if (error_elem >= program_size)
	{
		error_elem = program_size - 1;
		is_last_elem = 1;
	}

	int curr_elem = error_elem,
	first_elem = error_elem, 
	last_elem = error_elem;

	int str = lex_array[error_elem]->get_str();

	command += std::to_string(str) + " | ";

	while ((first_elem >= 0) && (lex_array[first_elem]->get_str() == str))
	{
		first_elem--;
	}

	first_elem++;

	std::string command4(static_cast<int>(std::to_string(str).size()), ' ');
	command4 += " | Error in row number " + std::to_string(str) + " and column number " + std::to_string(curr_elem - first_elem + (is_last_elem ? 2 : 1)) + "\n";

	while ((last_elem < program_size) && (lex_array[last_elem]->get_str() == str))
	{
		last_elem++;
	}

	for (; first_elem <= curr_elem; first_elem++)
	{
		command += lex_array[first_elem]->short_name() + " ";
	}

	std::string command2(static_cast<int>(command.size() - mess.size() - 3 - std::to_string(str).size()), ' ');
	std::string command3(static_cast<int>(std::to_string(str).size()), ' ');
	command3 += " |" + command2;

	command3 +=  "^\n";

	for (; first_elem < last_elem; first_elem++)
	{
		command += lex_array[first_elem]->short_name() + " ";
	}

	command += "\n" + command3 + command4;
	
	throw std::logic_error(command);
}


#endif

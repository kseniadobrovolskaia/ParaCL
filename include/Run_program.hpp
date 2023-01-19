#ifndef RUN_PROGRAM_H
#define RUN_PROGRAM_H



#include "Parser_stmts.hpp"


//--------------------------------------------RUN_PROGRAM--------------------------------------------------


void run_program(std::vector<Statement*> prog, std::istream & istr, std::ostream & ostr)
{
	int size_prog = prog.size();
	
	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		prog[prog_elem]->run_stmt(istr, ostr);
	}
}


#endif

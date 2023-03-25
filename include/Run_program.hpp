#ifndef RUN_PROGRAM_H
#define RUN_PROGRAM_H



#include "Parser_stmts.hpp"


//--------------------------------------------RUN_PROGRAM--------------------------------------------------


void run_program(Lex_t *prog, std::istream & istr, std::ostream & ostr)
{
	prog->calculate(istr, ostr);
}


#endif

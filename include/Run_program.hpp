#ifndef RUN_PROGRAM_H
#define RUN_PROGRAM_H



#include "Parser_stmts.hpp"


void run_program(std::vector<Statement*> prog)
{
	int size_prog = prog.size();

	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		prog[prog_elem]->run_stmt();
	}
}


void Assign::run_stmt()
{
	std::string name_var = vars[(this->get_lhs())->get_data()];

	if (is_scan(rhs_))
	{
		std::cin >> std::ws;
		std::cin >> VARS[name_var];
	}
	else
	{
		VARS[name_var] = calculate(rhs_);
	}
}


void If::run_stmt()
{
	int condition = calculate(this->get_lhs());

	if (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs());
	}
}


void While::run_stmt()
{
	int condition = calculate(this->get_lhs());

	while (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs());
		condition = calculate(this->get_lhs());
	}
}


void Print::run_stmt()
{
	int val = calculate(this->get_lhs());

	std::cout << val << std::endl;
}


void Inc_Dec::run_stmt()
{
	std::string name_var = vars[(this->get_lhs())->get_data()];

	if (this->get_kind() == Keywords_t::INCREM)
	{
		VARS[name_var]++;
	}
	else
	{
		VARS[name_var]--;
	}
}



#endif

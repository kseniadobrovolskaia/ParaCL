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
	lhs_->calculate();
}


void If::run_stmt()
{
	int condition = lhs_->calculate();

	if (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs());
	}
}


void While::run_stmt()
{
	int condition = lhs_->calculate();

	while (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs());
		condition = lhs_->calculate();
	}
}


void Print::run_stmt()
{
	int val = lhs_->calculate();

	std::cout << val << std::endl;
}


void Inc_Dec::run_stmt()
{
	lhs_->calculate();

	Lex_t *var = lhs_, *tmpvar;
	
	int flag = 1;
	while (flag)
	{
		if ((tmpvar = dynamic_cast<Variable*>(var)))
		{
			flag = 0;
		}
		else
		{
			var = var->get_var();
		}
	}
	
	std::string var_name = vars[var->get_data()];

	if (this->get_kind() == Statements_t::INC)
	{
		VARS[var_name] += 1;
	}
	else
	{
		VARS[var_name] -= 1;
	}
}



#endif

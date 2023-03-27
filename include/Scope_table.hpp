#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H

#include "Run_program.hpp"




//--------------------------------------------SCOPE_TABLE_CLASS-------------------------------------------------------


class Scope_table
{
	Scope_table *higher_scope_;
	std::unordered_map<std::string, int> vars_;	

public:
	Scope_table(Scope_table *higher_scope) : higher_scope_(higher_scope) {};
	Scope_table() : higher_scope_(nullptr) {};

	int get_value(std::string name);
	int& write_value(std::string name);
};


int Scope_table::get_value(std::string name)
{
	if (!vars_.contains(name))
	{
		throw_exception("Uninitialized variable\n", 10); //think how get num wrong str
	}

	return vars_[name];
}


int& Scope_table::write_value(std::string name)
{
	// if (!vars_.contains(name))
	// {
	// 	vars_[name] = 0;
	// }

	return vars_[name];
}

#endif

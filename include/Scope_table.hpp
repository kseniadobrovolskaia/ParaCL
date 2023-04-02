#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H


#include "Throw_exception.hpp"


//--------------------------------------------SCOPE_TABLE_CLASS-------------------------------------------------------


class Scope_table
{
	Scope_table *higher_scope_;
	std::unordered_map<std::string, int> vars_;	

public:
	Scope_table(Scope_table *higher_scope) : higher_scope_(higher_scope) {};
	Scope_table() : higher_scope_(nullptr) {};

	void init_var(const std::string &name);
	int &get_var(const std::string &name, int num_lexem);
	void print();
};


void Scope_table::init_var(const std::string &name)
{
	vars_[name];
}


int &Scope_table::get_var(const std::string &name, int num_lexem)
{
	if (!vars_.contains(name))
	{
		if (higher_scope_)
		{
			return higher_scope_->get_var(name, num_lexem);
		}
		else
		{
			throw_exception("Uninitialized variable in current scope\n", num_lexem);
		}
	}

	return vars_[name];
}



#endif

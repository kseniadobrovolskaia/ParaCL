#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H


#include "Throw_exception.hpp"


//--------------------------------------------SCOPE_TABLE_CLASS-------------------------------------------------------

class Statement;

std::unordered_map<std::string, Statement*> FUNCTIONS;

class Scope_table
{
	Scope_table *higher_scope_;
	std::unordered_map<std::string, int> vars_;
	std::unordered_map<std::string, Statement*> funcs_;

public:
	Scope_table(Scope_table *higher_scope) : higher_scope_(higher_scope) {};
	Scope_table() : higher_scope_(nullptr) {};

	void init_var(const std::string &name);
	void init_func(const std::string &name, Statement *body);

	int is_var_exist(const std::string &name);
	int is_func_exist(const std::string &name);

	int &get_var(const std::string &name, int num_lexem);
	Statement *get_func_decl(const std::string &name, int num_lexem);

	void clean_var_table();
	void clean_func_table();

	Scope_table *get_high_scope() const { return higher_scope_; };
	void print_scope() const;
};


void Scope_table::print_scope() const
{
	std::cout << "Variables: \n";
	for (auto it = vars_.begin(); it != vars_.end(); ++it)
        std::cout << it->first << " = " << it->second << "\n";
    
    std::cout << "Functions: \n";
	for (auto it = funcs_.begin(); it != funcs_.end(); ++it)
        std::cout << it->first << " = " << it->second << "\n";
}

void Scope_table::init_var(const std::string &name)
{
	if (!is_var_exist(name))
	{
		vars_[name];
	}
}


void Scope_table::init_func(const std::string &name, Statement *body)
{
	funcs_[name] = body;
}


int Scope_table::is_var_exist(const std::string &name)
{
	if (vars_.contains(name))
	{
		return 1;
	}

	if (higher_scope_)
	{
		return higher_scope_->is_var_exist(name);
	}

	return 0;
}


int Scope_table::is_func_exist(const std::string &name)
{
	if (funcs_.contains(name))
	{
		return 1;
	}

	if (higher_scope_)
	{
		return higher_scope_->is_func_exist(name);
	}

	return FUNCTIONS.contains(name);
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


Statement *Scope_table::get_func_decl(const std::string &name, int num_lexem)
{
	if (!funcs_.contains(name))
	{
		if (higher_scope_)
		{
			return higher_scope_->get_func_decl(name, num_lexem);
		}
		else
		{
			if (FUNCTIONS.contains(name))
			{
				return FUNCTIONS[name];
			}
			
			throw_exception("This function was not declared\n", num_lexem);
		}
	}

	return funcs_[name];
}


void Scope_table::clean_var_table()
{
	vars_.clear();
}


void Scope_table::clean_func_table()
{
	funcs_.clear();
}


#endif

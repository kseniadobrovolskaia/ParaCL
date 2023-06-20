#include "Scope_table.hpp"



void Scope_table::print_scope() const
{
	std::cout << "Variables: \n";
	for (auto &&elem : vars_)
        std::cout << elem.first << " = " << elem.second << "\n";
    
    std::cout << "Functions: \n";
	for (auto &&elem : funcs_)
        std::cout << elem.first << " = " << elem.second << "\n";
}


void Scope_table::init_var(const std::string &name)
{
	if (!is_var_exist(name))
	{
		vars_[name];
	}
}


void Scope_table::init_func(const std::string &name, std::shared_ptr<Statement> body)
{
	funcs_[name] = body;
}


int Scope_table::is_var_exist(const std::string &name) const
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


int Scope_table::is_func_exist(const std::string &name) const
{
	if (funcs_.contains(name))
	{
		return 1;
	}

	if (higher_scope_)
	{
		return higher_scope_->is_func_exist(name);
	}

	return AST_creator::FUNCTIONS.contains(name);
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


std::shared_ptr<Statement> Scope_table::get_func_decl(const std::string &name, int num_lexem)
{
	if (!funcs_.contains(name))
	{
		if (higher_scope_)
		{
			return higher_scope_->get_func_decl(name, num_lexem);
		}
		else
		{
			if (AST_creator::FUNCTIONS.contains(name))
			{
				return AST_creator::FUNCTIONS[name];
			}
			
			throw_exception("This function was not declared\n", num_lexem);
		}
	}

	return funcs_[name];
}


void Scope_table::clean_var_table() noexcept
{
	vars_.clear();
}


void Scope_table::clean_func_table() noexcept
{
	funcs_.clear();
}

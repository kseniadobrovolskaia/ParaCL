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


static llvm::AllocaInst *CreateAlloca(const std::string &VarName)
{
	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();
	llvm::Function *Func = InsertBB->getParent();
	
	llvm::IRBuilder<> TmpB(&Func->getEntryBlock(), Func->getEntryBlock().begin());

	return TmpB.CreateAlloca(llvm::Type::getInt32Ty(*AST_creator::TheContext), 0, VarName.c_str());
}


llvm::AllocaInst *Scope_table::alloca_var(const std::string &name, int num_lexem)
{
  	if (!is_var_alloca(name))
	{
		VarNames[name] = CreateAlloca(name);
		return VarNames[name];
	}

	return get_var_addr(name, num_lexem);
}        
	

void Scope_table::init_func(const std::string &name, std::shared_ptr<Statement> body)
{
	funcs_[name] = body;
}


void Scope_table::add_func(const std::string &name, llvm::Function *func)
{
	FuncNames[name] = func;
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


int Scope_table::is_var_alloca(const std::string &name) const
{
	if (VarNames.contains(name))
	{
		return 1;
	}

	if (higher_scope_)
	{
		return higher_scope_->is_var_alloca(name);
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


llvm::AllocaInst *Scope_table::get_var_addr(const std::string &name, int num_lexem)
{
	if (!VarNames.contains(name))
	{
		if (higher_scope_)
		{
			return higher_scope_->get_var_addr(name, num_lexem);
		}
		else
		{
			throw_exception("Uninitialized variable in current scope\n", num_lexem);
		}
	}

	return VarNames[name];
}            


llvm::Function *Scope_table::get_func_addr(const std::string &name, int num_lexem)
{
	if (!FuncNames.contains(name))
	{
		if (higher_scope_)
		{
			return higher_scope_->get_func_addr(name, num_lexem);
		}
		else
		{
			throw_exception("Uninitialized function in current scope\n", num_lexem);
		}
	}

	return FuncNames[name];
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
			
			throw_exception("Uninitialized function in current scope\n", num_lexem);
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


void Scope_table::clean_alloca_table() noexcept
{
	VarNames.clear();
}

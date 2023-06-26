#include <Stmt_methods.hpp>


//--------------------------------------------RUN_STATEMENTS------------------------------------------------


int If::run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int res;
	int condition = lhs_->calculate(istr, ostr, creator);

	if (condition)
	{
		res = rhs_->calculate(istr, ostr, creator);
	}
	else
	{
		if (else_)
		{
			res = else_->calculate(istr, ostr, creator);
		}
	}

	return res;
}


int While::run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int condition = lhs_->calculate(istr, ostr, creator);

	while (condition)
	{
		rhs_->calculate(istr, ostr, creator);

		if (creator.is_return())
			break;

		condition = lhs_->calculate(istr, ostr, creator);
	}

	return 0;
}


int Print::run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int val = lhs_->calculate(istr, ostr, creator);

	ostr << val << std::endl;

	return val;
}


int Return::run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	*(creator.set_return()) = 1;

	int val = lhs_->calculate(istr, ostr, creator);

	return val;
}


int Arithmetic::run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int res = lhs_->calculate(istr, ostr, creator);

	return res;
}


int Declaration::run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	creator.init_func(func_->short_name(), this_.lock());

	return 0;
}


//-----------------------------------------------CODEGEN------------------------------------------------




llvm::Value *Declaration::codegen(Codegen_creator &creator) 
{
	return creator.codegen_decl(*this);
}


llvm::Function *Declaration::codegen_func(Codegen_creator &creator) 
{
	return creator.codegen_func_decl(*this);
}


llvm::Value *If::codegen(Codegen_creator &creator) 
{
	return creator.codegen_if(*this);
}


llvm::Value *While::codegen(Codegen_creator &creator) 
{
  	return creator.codegen_while(*this);
}


llvm::Value *Print::codegen(Codegen_creator &creator) 
{
	return creator.codegen_print(*this);
}


llvm::Value *Return::codegen(Codegen_creator &creator) 
{
	return creator.codegen_return(*this);
}


llvm::Value *Arithmetic::codegen(Codegen_creator &creator) 
{
	return creator.codegen_arith(*this);
}


llvm::Function *Arithmetic::codegen_func(Codegen_creator &creator) 
{
	return creator.codegen_func_arith(*this);
}


//-------------------------------------------------NAMES----------------------------------------------------


std::string If::name() const
{
	return "if";
}

std::string While::name() const
{
	return "while";
}

std::string Print::name() const
{
	return "print";
}

std::string Return::name() const
{
	return "return";
}

std::string Arithmetic::name() const
{
	return lhs_->short_name();
}

std::string Declaration::name() const
{
	return func_->short_name();
}


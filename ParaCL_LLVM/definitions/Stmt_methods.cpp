#include <Stmt_methods.hpp>


//--------------------------------------------RUN_STATEMENTS------------------------------------------------


int If::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int res;
	int condition = lhs_->calculate(istr, ostr);

	if (condition)
	{
		res = rhs_->calculate(istr, ostr);
	}
	else
	{
		if (else_)
		{
			res = else_->calculate(istr, ostr);
		}
	}

	return res;
}


int While::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int condition = lhs_->calculate(istr, ostr);

	while (condition)
	{
		rhs_->calculate(istr, ostr);

		if (RETURN_COMMAND)
			break;

		condition = lhs_->calculate(istr, ostr);
	}

	return 0;
}


int Print::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int val = lhs_->calculate(istr, ostr);

	ostr << val << std::endl;

	return val;
}


int Return::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	if (!IN_FUNCTION)
	{
		throw_exception("You can return only from functions\n", lhs_->get_num() - 1);
	}

	RETURN_COMMAND = 1;

	int val = lhs_->calculate(istr, ostr);

	return val;
}


int Arithmetic::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int res = lhs_->calculate(istr, ostr);

	return res;
}


int Declaration::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	CURR_SCOPE->init_func(func_->short_name(), this_.lock());

	return 0;
}


//-----------------------------------------------CODE_GEN------------------------------------------------


llvm::Function *Declaration::codegen()
{
	std::string decl_name = funcs[func_->get_data()];
	std::vector<llvm::Type *> Ints(vars_.size(), llvm::Type::getInt32Ty(*TheContext));
  	
  	llvm::FunctionType *FuncType =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext), Ints, false);


  	llvm::Function *Func = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, TheModule.get());

  	if (!(Func->empty()))
  	{
  		throw_exception("Redefinition of function\n", func_->get_num() - 1);
  	}

  	int size = vars_.size();

  	NamedValues.clear();

  	unsigned Idx = 0;

  	//std::cout << "Function " << decl_name << " with " << size << " arguments\n";

  	for (auto &Arg : Func->args())
  	{
  		std::string name = vars[vars_[Idx++]->get_data()];
  		Arg.setName(name);

  		NamedValues[name] = &Arg;
  	}
    

	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*TheContext, "entry", Func);
  	Builder->SetInsertPoint(Block);

  	if (llvm::Value *RetVal = scope_->codegen())
  	{
    	Builder->CreateRet(RetVal);
		
		verifyFunction(*Func);

    	return Func;
  	}

	Func->eraseFromParent();

	throw_exception("Error in declaration codegen\n", scope_->get_num() - 1);

  	return nullptr;
}


llvm::Function *If::codegen()
{
	return nullptr;
}


llvm::Function *While::codegen()
{
	return nullptr;
}


llvm::Function *Print::codegen()
{
	return nullptr;
}


llvm::Function *Return::codegen()
{
	return nullptr;
}


llvm::Function *Arithmetic::codegen()
{
	return nullptr;
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
	std::cout << "Arithmetic ";
	return lhs_->short_name();
}

std::string Declaration::name() const
{
	std::cout << "Declaration ";
	return func_->short_name();
}


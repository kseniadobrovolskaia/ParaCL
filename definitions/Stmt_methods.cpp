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

		if (AST_creator::RETURN_COMMAND)
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
	if (!AST_creator::IN_FUNCTION)
	{
		throw_exception("You can return only from functions\n", lhs_->get_num() - 1);
	}

	AST_creator::RETURN_COMMAND = 1;

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
	AST_creator::CURR_SCOPE->init_func(func_->short_name(), this_.lock());

	return 0;
}


//-----------------------------------------------CODEGEN------------------------------------------------


llvm::Function *Declaration::codegen()
{
	//two! functions this function must generate
	std::string decl_name = Lex_t::funcs_table()[func_->get_data()];
	std::vector<llvm::Type *> Ints(vars_.size(), llvm::Type::getInt32Ty(*AST_creator::TheContext));
  	
  	llvm::FunctionType *FuncType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(*AST_creator::TheContext), Ints, false);

  	llvm::Function *Func = 
  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, AST_creator::TheModule.get());

  	if (!(Func->empty()))
  	{
  		throw_exception("Redefinition of function\n", func_->get_num() - 1);
  	}

  	AST_creator::NamedValues.clear();

  	unsigned Idx = 0;

  	for (auto &Arg : Func->args())
  	{
  		std::string name = Lex_t::vars_table()[vars_[Idx++]->get_data()];
  		Arg.setName(name);

  		AST_creator::NamedValues[name] = &Arg;
  	} 

	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*AST_creator::TheContext, "entry", Func);
  	AST_creator::Builder->SetInsertPoint(Block);

  	if (llvm::Value *RetVal = scope_->codegen())
  	{
    	AST_creator::Builder->CreateRet(RetVal);
		verifyFunction(*Func);

    	return Func;
  	}

	Func->eraseFromParent();

	throw_exception("Error in declaration codegen\n", scope_->get_num() - 1);

  	return nullptr;
}


llvm::Value *If::codegen_if()
{
	llvm::Value *Cond = lhs_->codegen();

 	if (!Cond)
  	{
  		throw_exception("Error in codegen condition in \"if\"\n", lhs_->get_num());
  	}

  	const llvm::APInt zero(32, 0, true);
  	
  	Cond = AST_creator::Builder->CreateICmpNE(Cond, 
    llvm::ConstantInt::get(*AST_creator::TheContext, zero), "ifcond");

	llvm::Function *Func = (AST_creator::Builder->GetInsertBlock())->getParent();
  	
	llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "then", Func);

	//move в if (else_) I fix it
	llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "else");
	llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "ifcont");

	AST_creator::Builder->CreateCondBr(Cond, ThenBB, ElseBB);

	AST_creator::Builder->SetInsertPoint(ThenBB);

	llvm::Value *Then = rhs_->codegen();

 	if (!Then)
  	{
  		throw_exception("Error in codegen then in \"if\"\n", rhs_->get_num());
  	}

	AST_creator::Builder->CreateBr(MergeBB);
	
	ThenBB = AST_creator::Builder->GetInsertBlock();

	//Now if there is not else - segfault
	Func->getBasicBlockList().push_back(ElseBB);
	AST_creator::Builder->SetInsertPoint(ElseBB);

	llvm::Value *Else = else_->codegen();
	
	if (!Else)
  	{
  		throw_exception("Error in codegen else in \"if\"\n", else_->get_num());
  	}

	AST_creator::Builder->CreateBr(MergeBB);
	ElseBB = AST_creator::Builder->GetInsertBlock();

	Func->getBasicBlockList().push_back(MergeBB);
	AST_creator::Builder->SetInsertPoint(MergeBB);
	llvm::PHINode *PN = 
	AST_creator::Builder->CreatePHI(llvm::Type::getInt32Ty(*AST_creator::TheContext), 2, "iftmp");

	PN->addIncoming(Then, ThenBB);
	PN->addIncoming(Else, ElseBB);

	return PN;
}

//I do it
llvm::Function *While::codegen()
{
	return nullptr;
}

//I do it
llvm::Function *Print::codegen()
{
	return nullptr;
}

//I do it
llvm::Function *Return::codegen()
{
	return nullptr;
}


llvm::Function *Arithmetic::codegen()
{
	///Create function without name
	std::string decl_name = "";
	std::vector<llvm::Type *> Ints(0, llvm::Type::getInt32Ty(*AST_creator::TheContext));
  	
  	llvm::FunctionType *FuncType =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*AST_creator::TheContext), Ints, false);

  	llvm::Function *Func = 
  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, AST_creator::TheModule.get());

  	if (!(Func->empty()))
  	{
  		throw_exception("Redefinition of function\n", lhs_->get_num() - 1);
  	}
    
	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*AST_creator::TheContext, "entry", Func);
  	AST_creator::Builder->SetInsertPoint(Block);

  	if (llvm::Value *RetVal = lhs_->codegen())
  	{
    	AST_creator::Builder->CreateRet(RetVal);
		verifyFunction(*Func);

    	return Func;
  	}

	Func->eraseFromParent();

	throw_exception("Error in declaration codegen\n", lhs_->get_num() - 1);

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
	return lhs_->short_name();
}

std::string Declaration::name() const
{
	return func_->short_name();
}


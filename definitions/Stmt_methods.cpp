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


llvm::Value *Declaration::codegen() const
{
	const llvm::APInt zero(32, 0, true);
	return llvm::ConstantInt::get(*AST_creator::TheContext, zero);
}


static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *Func, const std::string &VarName)
{
  llvm::IRBuilder<> TmpB(&Func->getEntryBlock(), Func->getEntryBlock().begin());

  return TmpB.CreateAlloca(llvm::Type::getInt32Ty(*AST_creator::TheContext), 0, VarName.c_str());
}


llvm::Function *Declaration::codegen_func() const
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

  	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*AST_creator::TheContext, "entry", Func);
  	AST_creator::Builder->SetInsertPoint(Block);
  	
  	AST_creator::NamedValues.clear();//it strange

  	unsigned Idx = 0;
  	for (auto &Arg : Func->args())
  	{
  		std::string name = Lex_t::vars_table()[vars_[Idx++]->get_data()];

  		llvm::AllocaInst *Alloca = CreateEntryBlockAlloca(Func, name);
  		AST_creator::Builder->CreateStore(&Arg, Alloca);

  		AST_creator::NamedValues[name] = Alloca;
  	} 


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


llvm::Value *If::codegen() const
{
	llvm::Value *Cond = lhs_->codegen();

 	if (!Cond)
  	{
  		throw_exception("Error in codegen condition in \"if\"\n", lhs_->get_num());
  	}

  	const llvm::APInt zero(32, 0, true);
	llvm::Value *Zero = llvm::ConstantInt::get(*AST_creator::TheContext, zero);

  	Cond = AST_creator::Builder->CreateICmpNE(Cond, Zero, "ifcond");

  	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();
	llvm::Function *Func = InsertBB->getParent();
	llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "then", Func);

	llvm::BasicBlock *ElseBB;
	llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "ifcont");

	if (else_)
	{
		ElseBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "else");
		AST_creator::Builder->CreateCondBr(Cond, ThenBB, ElseBB);
	}
	else
	{
		AST_creator::Builder->CreateCondBr(Cond, ThenBB, MergeBB);
	}

	AST_creator::Builder->SetInsertPoint(ThenBB);

	llvm::Value *Then = rhs_->codegen();

 	if (!Then)
  	{
  		throw_exception("Error in codegen then in \"if\"\n", rhs_->get_num());
  	}

	AST_creator::Builder->CreateBr(MergeBB);
	ThenBB = AST_creator::Builder->GetInsertBlock();

	llvm::Value *Else;

	if (else_)
	{
		Func->getBasicBlockList().push_back(ElseBB);
		AST_creator::Builder->SetInsertPoint(ElseBB);

		Else = else_->codegen();
		
		if (!Else)
	  	{
	  		throw_exception("Error in codegen else in \"if\"\n", else_->get_num());
	  	}

		AST_creator::Builder->CreateBr(MergeBB);
		ElseBB = AST_creator::Builder->GetInsertBlock();
	}
	else
	{
		const llvm::APInt zero(32, 0, true);
		Else = llvm::ConstantInt::get(*AST_creator::TheContext, zero);
		ElseBB = InsertBB;
	}

	Func->getBasicBlockList().push_back(MergeBB);
	AST_creator::Builder->SetInsertPoint(MergeBB);
	llvm::PHINode *PN = 
	AST_creator::Builder->CreatePHI(llvm::Type::getInt32Ty(*AST_creator::TheContext), 2, "iftmp");

	PN->addIncoming(Then, ThenBB);
	PN->addIncoming(Else, ElseBB);

	return PN;
}


llvm::Value *While::codegen() const
{
  	const llvm::APInt zero(32, 0, true);
	llvm::Value *Zero = llvm::ConstantInt::get(*AST_creator::TheContext, zero);

  	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();
	llvm::Function *Func = InsertBB->getParent();

	llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "loopCond", Func);
	AST_creator::Builder->CreateBr(CondBB);
	AST_creator::Builder->SetInsertPoint(CondBB);

	llvm::Value *Cond = lhs_->codegen();
	Cond = AST_creator::Builder->CreateICmpNE(Cond, Zero, "whilecond");

 	if (!Cond)
  	{
  		throw_exception("Error in codegen condition in \"while\"\n", lhs_->get_num());
  	}
	
	llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "loop");
	llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "loopEnd");
	
	Func->getBasicBlockList().push_back(LoopBB);
	AST_creator::Builder->CreateCondBr(Cond, LoopBB, EndBB);
	CondBB = AST_creator::Builder->GetInsertBlock();

	AST_creator::Builder->SetInsertPoint(LoopBB);

	llvm::Value *Loop = rhs_->codegen();

	AST_creator::Builder->CreateBr(CondBB);
	LoopBB = AST_creator::Builder->GetInsertBlock();//may be InsertBB..?

	Func->getBasicBlockList().push_back(EndBB);
	AST_creator::Builder->SetInsertPoint(EndBB);

	return Zero;
}

//I do it
llvm::Value *Print::codegen() const
{
	return nullptr;
}

//I do it
llvm::Value *Return::codegen() const
{
	return nullptr;
}


llvm::Value *Arithmetic::codegen() const
{
	return lhs_->codegen();
}


llvm::Function *Arithmetic::codegen_func() const
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


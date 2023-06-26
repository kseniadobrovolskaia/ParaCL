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
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	creator.init_func(func_->short_name(), this_.lock());

	return 0;
}


//-----------------------------------------------CODEGEN------------------------------------------------


llvm::Value *Declaration::codegen(AST_creator &creator) const
{
	const llvm::APInt zero(32, 0, true);
	return llvm::ConstantInt::get(*AST_creator::TheContext, zero);
}


llvm::Function *Declaration::codegen_func(AST_creator &creator) const
{
	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();

	std::string decl_name = func_->short_name();

	if (global_name_.size() > 0)
	{
		decl_name = global_name_;
	}

	std::vector<llvm::Type *> Ints(vars_.size(), llvm::Type::getInt32Ty(*AST_creator::TheContext));
  	
  	llvm::FunctionType *FuncType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(*AST_creator::TheContext), Ints, false);

  	llvm::Function *Func = 
  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, AST_creator::TheModule.get());

  	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*AST_creator::TheContext, "entry", Func);
  	AST_creator::Builder->SetInsertPoint(Block);
  	
  	std::shared_ptr<Scope_table> func_scope = std::make_shared<Scope_table>();
  	std::shared_ptr<Scope_table> old_curr_scope = creator.get_CURR_SCOPE();
	creator.set_CURR_SCOPE(func_scope);

  	unsigned Idx = 0;
  	for (auto &Arg : Func->args())
  	{
  		std::string name = vars_[Idx++]->short_name();
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  		llvm::AllocaInst *Alloca = creator.alloca_var(name, func_->get_num() + 3);
  		AST_creator::Builder->CreateStore(&Arg, Alloca);
  	} 

  	llvm::Value *RetVal = scope_->codegen(creator);

  	if (!RetVal)
  	{
		throw_exception("Error in codegen return in declaration\n", scope_->get_num());
  	}

  	AST_creator::Builder->CreateRet(RetVal);
	verifyFunction(*Func);

  	creator.set_CURR_SCOPE(old_curr_scope);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  	creator.add_func(func_->short_name(), Func);

  	AST_creator::Builder->SetInsertPoint(InsertBB);

  	return Func;
}


llvm::Value *If::codegen(AST_creator &creator) const
{
	llvm::Value *Cond = lhs_->codegen(creator);

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

	llvm::Value *Then = rhs_->codegen(creator);

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

		Else = else_->codegen(creator);
		
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


llvm::Value *While::codegen(AST_creator &creator) const
{
  	const llvm::APInt zero(32, 0, true);
	llvm::Value *Zero = llvm::ConstantInt::get(*AST_creator::TheContext, zero);

  	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();
	llvm::Function *Func = InsertBB->getParent();

	llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*AST_creator::TheContext, "loopCond", Func);
	AST_creator::Builder->CreateBr(CondBB);
	AST_creator::Builder->SetInsertPoint(CondBB);

	llvm::Value *Cond = lhs_->codegen(creator);
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

	rhs_->codegen(creator);

	AST_creator::Builder->CreateBr(CondBB);
	LoopBB = AST_creator::Builder->GetInsertBlock();

	Func->getBasicBlockList().push_back(EndBB);
	AST_creator::Builder->SetInsertPoint(EndBB);

	return Zero;
}


llvm::Value *Print::codegen(AST_creator &creator) const
{
	llvm::Value *Val = lhs_->codegen(creator);

	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();

 	llvm::Function *func_printf = AST_creator::TheModule->getFunction("printf");

    if (!func_printf)
    {
    	std::vector<llvm::Type *> Ints(1, llvm::Type::getInt32Ty(*AST_creator::TheContext));
	  	
	  	llvm::FunctionType *FuncType =
	    llvm::FunctionType::get(llvm::Type::getInt32Ty(*AST_creator::TheContext), Ints, false);

	  	func_printf = 
	  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "printf", AST_creator::TheModule.get());

        func_printf->setCallingConv(llvm::CallingConv::C);
    }

    llvm::Value *str = AST_creator::Builder->CreateGlobalStringPtr("%d");
    std::vector <llvm::Value*> call_params;
    call_params.push_back(str);
	
	call_params.push_back(Val);
 
   	llvm::CallInst::Create(func_printf, call_params, "calltmp", InsertBB);
    
    return Val;
}


llvm::Value *Return::codegen(AST_creator &creator) const
{
	llvm::Value *RetVal = lhs_->codegen(creator);

  	if (!RetVal)
  	{
		throw_exception("Error in codegen Return\n", lhs_->get_num());
  	}

  	AST_creator::Builder->CreateRet(RetVal);

  	return RetVal;
}


llvm::Value *Arithmetic::codegen(AST_creator &creator) const
{
	return lhs_->codegen(creator);
}


llvm::Function *Arithmetic::codegen_func(AST_creator &creator) const
{
	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();

	///Create function without name
	std::string decl_name = "";
	std::vector<llvm::Type *> Ints(0, llvm::Type::getInt32Ty(*AST_creator::TheContext));
  	
  	llvm::FunctionType *FuncType =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*AST_creator::TheContext), Ints, false);

  	llvm::Function *Func = 
  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, AST_creator::TheModule.get());

	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*AST_creator::TheContext, "entry", Func);
  	AST_creator::Builder->SetInsertPoint(Block);

  	std::shared_ptr<Scope_table> func_scope = std::make_shared<Scope_table>(creator.get_CURR_SCOPE());
  	std::shared_ptr<Scope_table> old_curr_scope = creator.get_CURR_SCOPE();
	creator.set_CURR_SCOPE(func_scope);

	llvm::Value *RetVal = lhs_->codegen(creator);

  	if (!RetVal)
  	{
		throw_exception("Error in declaration codegen\n", lhs_->get_num());
  	}

  	AST_creator::Builder->CreateRet(RetVal);
	verifyFunction(*Func);

  	creator.set_CURR_SCOPE(old_curr_scope);

  	AST_creator::Builder->SetInsertPoint(InsertBB);

  	return Func;
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


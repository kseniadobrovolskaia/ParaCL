#include "Parser.hpp"




llvm::AllocaInst *Codegen_creator::alloca_var(const std::string &name, int num_lexem)
{ 
	return CURR_SCOPE_->alloca_var(name, num_lexem, TheContext_, Builder_);
};


//------------------------------------------------------CODEGEN_LEXEMS---------------------------------------------------------



llvm::Value *Codegen_creator::codegen_value(Value &val)
{
	if (val.get_type() != Value_type::INPUT)
	{	
		const llvm::APInt value(32, val.get_value(), true);

		return llvm::ConstantInt::get(*TheContext_, value);
	}

	llvm::BasicBlock *InsertBB = Builder_->GetInsertBlock();
 	llvm::Function *func_scanf = TheModule_->getFunction("scanf");

    if (!func_scanf)
    {
    	std::vector<llvm::Type *> Ints(0, llvm::Type::getInt32Ty(*TheContext_));
	  	
	  	llvm::FunctionType *FuncType =
	    llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext_), Ints, false);

	  	func_scanf = 
	  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "scanf", TheModule_.get());

        func_scanf->setCallingConv(llvm::CallingConv::C);
    }

    llvm::Value *str = Builder_->CreateGlobalStringPtr("%d");
    std::vector <llvm::Value*> call_params;
    call_params.push_back(str);
 
    llvm::CallInst *call = llvm::CallInst::Create(func_scanf, call_params, "calltmp", InsertBB);
    
   	return call;
}


llvm::Value *Codegen_creator::codegen_variable(Variable &variable)
{
	std::string var_name = variable.short_name();

	llvm::AllocaInst *Var = CURR_SCOPE_->get_var_addr(var_name, variable.get_num());

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", variable.get_num());
  	}

  	return Builder_->CreateLoad(Var->getAllocatedType(), Var, var_name.c_str());
}


llvm::Value *Codegen_creator::codegen_negation(Negation &negation)
{
	llvm::Value *right = negation.get_rhs().codegen(*this);
	const llvm::APInt zero(32, 0, true);
  	
  	right = Builder_->CreateICmpEQ(right, 
    llvm::ConstantInt::get(*TheContext_, zero), "neg");

    right = Builder_->CreateUIToFP(right, llvm::Type::getDoubleTy(*TheContext_), "doubletmp");
	return Builder_->CreateFPToSI(right, llvm::Type::getInt32Ty(*TheContext_), "inttmp");
}


llvm::Value *Codegen_creator::codegen_assign(Assign_node &assign)
{
	llvm::Value *Val = assign.get_rhs().codegen(*this);

  	if (!Val)
  	{
  		throw_exception("Non-existing operand\n", assign.get_rhs().get_num());
  	}
	
	Lex_t *var = &(assign.get_lhs());
	std::string var_name;

	if (dynamic_cast<Variable*>(var))
	{
		var_name = var->short_name();
	}
	else
	{
		std::shared_ptr<Lex_t> vari = assign.get_variable();
		var_name = vari->short_name();
	}

	llvm::AllocaInst *Var = CURR_SCOPE_->alloca_var(var_name, assign.get_num(), TheContext_, Builder_);

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", assign.get_num());
  	}

  	Builder_->CreateStore(Val, Var);

  	return Val;
}


llvm::Value *Codegen_creator::codegen_scope(Scope &scope)
{
	std::shared_ptr<Scope_table> curr_scope = std::make_shared<Scope_table>(CURR_SCOPE_);
	std::shared_ptr<Scope_table> old_curr_scope = CURR_SCOPE_;
	
	CURR_SCOPE_ = curr_scope;

  	const std::vector<std::shared_ptr<Statement>> &stmts = scope.get_lhs();
	const llvm::APInt zero(32, 0, true);
		
	if (!stmts.size())
	{
		return llvm::ConstantInt::get(*TheContext_, zero);
	}
	
	llvm::Value *last_expr = llvm::ConstantInt::get(*TheContext_, zero);

	for (auto &elem : stmts)
	{
		if (std::dynamic_pointer_cast<Declaration>(elem))
		{
			elem->codegen_func(*this);
		}
		else
		{
			last_expr = elem->codegen(*this);
		}
	}

	CURR_SCOPE_ = old_curr_scope;

	return last_expr;
}


llvm::Value *Codegen_creator::codegen_call(Function &function)
{
	std::string func_name = function.short_name();
	llvm::Function *Call = TheModule_->getFunction(func_name);

	if (!Call)
	{
		Call = CURR_SCOPE_->get_func_addr(func_name, function.get_num());
  	}

  	const std::vector<std::shared_ptr<Lex_t>> &args = function.get_args();
	
  	int args_size = args.size();

	if (static_cast<int>(Call->arg_size()) != args_size)
	{
		throw_exception("Incorrect count arguments\n", function.get_num());
	}
   
	std::vector<llvm::Value*> ArgsV;

  	for (auto &&arg : args)
  	{
  		ArgsV.push_back(arg->codegen(*this));
    }
    
  	return Builder_->CreateCall(Call, ArgsV, "calltmp");
}


llvm::Value *Codegen_creator::codegen_unop(UnOp &unop)
{
	const llvm::APInt one(32, 1, true);	
	llvm::Value *One = llvm::ConstantInt::get(*TheContext_, one);
	
	std::shared_ptr<Lex_t> var = unop.get_variable();
	std::string var_name;

	llvm::Value *Var = var->codegen(*this);

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", unop.get_num());
  	}

  	llvm::Value *Val;

	switch (unop.get_data())
	{
		case Statements_t::INC:
			Val = Builder_->CreateAdd(Var, One, "Inctmp");
			break;
		case Statements_t::DEC:
			Val = Builder_->CreateSub(Var, One, "Dectmp");
			break;
	}

  	if (!Val)
  	{
  		throw_exception("Non-existing operand\n", unop.get_num());
  	}

  	Builder_->CreateStore(Val, Var);

  	return Val;
}


llvm::Value *Codegen_creator::codegen_binop(BinOp &binop)
{
	llvm::Value *left = binop.get_lhs().codegen(*this);
  	llvm::Value *right = binop.get_rhs().codegen(*this);

  	if (!left || !right) 
  	{
  		throw_exception("Non-existing operand\n", binop.get_num());
  	};

	switch (binop.get_data())
	{
		case BinOp_t::ADD:
			return Builder_->CreateAdd(left, right, "addtmp");
		case BinOp_t::SUB:
			return Builder_->CreateSub(left, right, "subtmp");
		case BinOp_t::MULT:
			return Builder_->CreateMul(left, right, "multtmp");
		case BinOp_t::DIV:
			if (right == 0)
			{
				throw_exception("Division by zero\n", binop.get_num());
			}
			return Builder_->CreateSDiv(left, right, "divtmp");
	}

	throw_exception("Error: it is not clear what is in function \"BinOp::Codegen\"\n", binop.get_num());
	return nullptr;
}


llvm::Value *Codegen_creator::codegen_compop(CompOp &compop)
{
	llvm::Value *left = compop.get_lhs().codegen(*this);
  	llvm::Value *right = compop.get_rhs().codegen(*this);

  	if (!left || !right) 
  	{
  		throw_exception("Non-existing operand\n", compop.get_num());
  	};
  
	switch (compop.get_data())
	{
		case CompOp_t::LESS:
			left = Builder_->CreateICmpSLT(left, right, "lesstmp");
			break;
		case CompOp_t::GREATER:
			left = Builder_->CreateICmpSGT(left, right, "greatertmp");
			break;
		case CompOp_t::LESorEQ:
			left = Builder_->CreateICmpSLE(left, right, "LESorEQtmp");
			break;
		case CompOp_t::GRorEQ:
			left = Builder_->CreateICmpSGE(left, right, "GRorEQtmp");
			break;
		case CompOp_t::EQUAL:
			left = Builder_->CreateICmpEQ(left, right, "EQtmp");
			break;
		case CompOp_t::NOT_EQUAL:
			left = Builder_->CreateICmpNE(left, right, "NEtmp");
			break;
	}
	
	left = Builder_->CreateUIToFP(left, llvm::Type::getDoubleTy(*TheContext_), "doubletmp");
	return Builder_->CreateFPToSI(left, llvm::Type::getInt32Ty(*TheContext_), "inttmp");
}


//---------------------------------------------------CODEGEN_STATEMENS--------------------------------------------------------



llvm::Value *Codegen_creator::codegen_decl(Declaration &declaration)
{
	const llvm::APInt zero(32, 0, true);
	return llvm::ConstantInt::get(*TheContext_, zero);
}


llvm::Function *Codegen_creator::codegen_func_decl(Declaration &declaration)
{
	llvm::BasicBlock *InsertBB = Builder_->GetInsertBlock();

	std::string decl_name = declaration.get_func_name();
	std::string global_name = declaration.get_global_name();

	std::shared_ptr<Lex_t> scope = declaration.get_scope();
	const std::vector<std::shared_ptr<Lex_t>> vars = declaration.get_args();

	if (global_name.size() > 0)
	{
		decl_name = global_name;
	}

	std::vector<llvm::Type *> Ints(vars.size(), llvm::Type::getInt32Ty(*TheContext_));
  	
  	llvm::FunctionType *FuncType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext_), Ints, false);

  	llvm::Function *Func = 
  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, TheModule_.get());

  	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*TheContext_, "entry", Func);
  	Builder_->SetInsertPoint(Block);
  	
  	std::shared_ptr<Scope_table> func_scope = std::make_shared<Scope_table>(CURR_SCOPE_);
  	std::shared_ptr<Scope_table> old_curr_scope = CURR_SCOPE_;
	CURR_SCOPE_ = func_scope;

  	unsigned Idx = 0;
  	for (auto &Arg : Func->args())
  	{
  		std::string name = vars[Idx++]->short_name();
  		llvm::AllocaInst *Alloca = alloca_var(name, scope->get_num() + 3);
  		Builder_->CreateStore(&Arg, Alloca);
  	} 

  	llvm::Value *RetVal = scope->codegen(*this);

  	if (!RetVal)
  	{
		throw_exception("Error in codegen return in declaration\n", scope->get_num());
  	}

  	Builder_->CreateRet(RetVal);
	verifyFunction(*Func);

  	CURR_SCOPE_ = old_curr_scope;
  	
  	add_func(declaration.get_func_name(), Func);

  	Builder_->SetInsertPoint(InsertBB);

  	return Func;
}


llvm::Value *Codegen_creator::codegen_if(If &if_class)
{
	llvm::Value *Cond = if_class.get_lhs().codegen(*this);

 	if (!Cond)
  	{
  		throw_exception("Error in codegen condition in \"if\"\n", if_class.get_lhs().get_num());
  	}

  	const llvm::APInt zero(32, 0, true);
	llvm::Value *Zero = llvm::ConstantInt::get(*TheContext_, zero);

  	Cond = Builder_->CreateICmpNE(Cond, Zero, "ifcond");

  	llvm::BasicBlock *InsertBB = Builder_->GetInsertBlock();
	llvm::Function *Func = InsertBB->getParent();
	llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*TheContext_, "then", Func);

	llvm::BasicBlock *ElseBB;
	llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*TheContext_, "ifcont");

	std::shared_ptr<Lex_t> Else = if_class.get_Else();

	if (Else)
	{
		ElseBB = llvm::BasicBlock::Create(*TheContext_, "else");
		Builder_->CreateCondBr(Cond, ThenBB, ElseBB);
	}
	else
	{
		Builder_->CreateCondBr(Cond, ThenBB, MergeBB);
	}

	Builder_->SetInsertPoint(ThenBB);

	llvm::Value *Then = if_class.get_rhs().codegen(*this);

 	if (!Then)
  	{
  		throw_exception("Error in codegen then in \"if\"\n", if_class.get_rhs().get_num());
  	}

	Builder_->CreateBr(MergeBB);
	ThenBB = Builder_->GetInsertBlock();

	llvm::Value *Else_val;

	if (Else)
	{
		Func->getBasicBlockList().push_back(ElseBB);
		Builder_->SetInsertPoint(ElseBB);

		Else_val = Else->codegen(*this);
		
		if (!Else_val)
	  	{
	  		throw_exception("Error in codegen else in \"if\"\n", Else->get_num());
	  	}

		Builder_->CreateBr(MergeBB);
		ElseBB = Builder_->GetInsertBlock();
	}
	else
	{
		const llvm::APInt zero(32, 0, true);
		Else_val = llvm::ConstantInt::get(*TheContext_, zero);
		ElseBB = InsertBB;
	}

	Func->getBasicBlockList().push_back(MergeBB);
	Builder_->SetInsertPoint(MergeBB);
	llvm::PHINode *PN = 
	Builder_->CreatePHI(llvm::Type::getInt32Ty(*TheContext_), 2, "iftmp");

	PN->addIncoming(Then, ThenBB);
	PN->addIncoming(Else_val, ElseBB);

	return PN;
}


llvm::Value *Codegen_creator::codegen_while(While &while_class)
{
  	const llvm::APInt zero(32, 0, true);
	llvm::Value *Zero = llvm::ConstantInt::get(*TheContext_, zero);

  	llvm::BasicBlock *InsertBB = Builder_->GetInsertBlock();
	llvm::Function *Func = InsertBB->getParent();

	llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*TheContext_, "loopCond", Func);
	Builder_->CreateBr(CondBB);
	Builder_->SetInsertPoint(CondBB);

	llvm::Value *Cond = while_class.get_lhs().codegen(*this);
	Cond = Builder_->CreateICmpNE(Cond, Zero, "whilecond");

 	if (!Cond)
  	{
  		throw_exception("Error in codegen condition in \"while\"\n", while_class.get_lhs().get_num());
  	}
	
	llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*TheContext_, "loop");
	llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*TheContext_, "loopEnd");
	
	Func->getBasicBlockList().push_back(LoopBB);
	Builder_->CreateCondBr(Cond, LoopBB, EndBB);
	CondBB = Builder_->GetInsertBlock();

	Builder_->SetInsertPoint(LoopBB);

	while_class.get_rhs().codegen(*this);

	Builder_->CreateBr(CondBB);
	LoopBB = Builder_->GetInsertBlock();

	Func->getBasicBlockList().push_back(EndBB);
	Builder_->SetInsertPoint(EndBB);

	return Zero;
}


llvm::Value *Codegen_creator::codegen_print(Print &print)
{
	llvm::Value *Val = print.get_lhs().codegen(*this);

	llvm::BasicBlock *InsertBB = Builder_->GetInsertBlock();

 	llvm::Function *func_printf = TheModule_->getFunction("printf");

    if (!func_printf)
    {
    	std::vector<llvm::Type *> Ints(1, llvm::Type::getInt32Ty(*TheContext_));
	  	
	  	llvm::FunctionType *FuncType =
	    llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext_), Ints, false);

	  	func_printf = 
	  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "printf", TheModule_.get());

        func_printf->setCallingConv(llvm::CallingConv::C);
    }

    llvm::Value *str = Builder_->CreateGlobalStringPtr("%d");
    std::vector <llvm::Value*> call_params;
    call_params.push_back(str);
	
	call_params.push_back(Val);
 
   	llvm::CallInst::Create(func_printf, call_params, "calltmp", InsertBB);
    
    return Val;
}


llvm::Value *Codegen_creator::codegen_return(Return &ret)
{
	llvm::Value *RetVal = ret.get_lhs().codegen(*this);

  	if (!RetVal)
  	{
		throw_exception("Error in codegen Return\n", ret.get_lhs().get_num());
  	}

  	Builder_->CreateRet(RetVal);

  	return RetVal;
}


llvm::Value *Codegen_creator::codegen_arith(Arithmetic &arith)
{
	return arith.get_lhs().codegen(*this);
}


llvm::Function *Codegen_creator::codegen_func_arith(Arithmetic &arith)
{
	llvm::BasicBlock *InsertBB = Builder_->GetInsertBlock();

	///Create function without name
	std::string decl_name = "";
	std::vector<llvm::Type *> Ints(0, llvm::Type::getInt32Ty(*TheContext_));
  	
  	llvm::FunctionType *FuncType =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext_), Ints, false);

  	llvm::Function *Func = 
  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, decl_name, TheModule_.get());

	llvm::BasicBlock *Block = llvm::BasicBlock::Create(*TheContext_, "entry", Func);
  	Builder_->SetInsertPoint(Block);

  	std::shared_ptr<Scope_table> func_scope = std::make_shared<Scope_table>(CURR_SCOPE_);
  	std::shared_ptr<Scope_table> old_curr_scope = CURR_SCOPE_;
	CURR_SCOPE_ = func_scope;

	llvm::Value *RetVal = arith.get_lhs().codegen(*this);

  	if (!RetVal)
  	{
		throw_exception("Error in declaration codegen\n", arith.get_lhs().get_num());
  	}

  	Builder_->CreateRet(RetVal);
	verifyFunction(*Func);

  	CURR_SCOPE_ = old_curr_scope;

  	Builder_->SetInsertPoint(InsertBB);

  	return Func;
}


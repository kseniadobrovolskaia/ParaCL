#include "Stmt_methods.hpp"


//---------------------------------------------CALCULATE----------------------------------------------------


int Value::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	if (type_ != Value_type::INPUT)
	{	
		return this->get_data();
	}

	int number;

	istr >> std::ws;
	istr >> number;

	if (istr.fail())
	{
		throw std::logic_error("Incorrect input\n");
	}

	return number;
}


int Variable::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	return creator.get_CURR_SCOPE()->get_var(this->short_name(), this->get_num());
}


int Negation::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int right = rhs_->calculate(istr, ostr, creator);

	return !right;
}


int Assign_node::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	std::shared_ptr<Lex_t> var = lhs_;
	std::string var_name;

	int right_part = rhs_->calculate(istr, ostr, creator);

	if (std::dynamic_pointer_cast<Variable>(var))
	{
		var_name = var->short_name();
		creator.init_var(var_name);
	}
	else
	{
		var = get_variable();
		var_name = var->short_name();
	}

	lhs_->calculate(istr, ostr, creator);

//!!!!!!!!!!!!!!!!!!!!!!!
	creator.get_var(var_name, var->get_num()) = right_part;

	return creator.get_var(var_name, var->get_num());
}


int BinOp::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int left = lhs_->calculate(istr, ostr, creator);
	int right = rhs_->calculate(istr, ostr, creator);

	switch (this->get_data())
	{
		case BinOp_t::ADD:
			return left + right;
		case BinOp_t::SUB:
			return left - right;
		case BinOp_t::MULT:
			return left * right;
		case BinOp_t::DIV:
			if (right == 0)
			{
				throw_exception("Division by zero\n", this->get_num());
			}
			return left / right;
	}

	throw_exception("Error: it is not clear what is in function \"BinOp::calculate\"\n", this->get_num());
	return 1;
}


int UnOp::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	lhs_->calculate(istr, ostr, creator);
	std::shared_ptr<Lex_t> var = lhs_;

	std::string var_name;
	
	if (!std::dynamic_pointer_cast<Variable>(var))
	{
		var = get_variable();
	}
	
	var_name = var->short_name();

	switch (this->get_data())
	{
		case Statements_t::INC:
			creator.get_var(var_name, this->get_num()) += 1;
			return creator.get_var(var_name, this->get_num());
		case Statements_t::DEC:
			creator.get_var(var_name, this->get_num()) -= 1;
			return creator.get_var(var_name, this->get_num());
	}

	throw_exception("Error: it is not clear what is in function \"UnOp::calculate\"\n", this->get_num());
	return 1;
}


int CompOp::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	int left = lhs_->calculate(istr, ostr, creator);
	int right = rhs_->calculate(istr, ostr, creator);

	switch (this->get_data())
	{
		case CompOp_t::LESS:
			return left < right;
		case CompOp_t::GREATER:
			return left > right;
		case CompOp_t::LESorEQ:
			return left <= right;
		case CompOp_t::GRorEQ:
			return left >= right;
		case CompOp_t::EQUAL:
			return left == right;
		case CompOp_t::NOT_EQUAL:
			return left != right;
	}

	throw_exception("Error: it is not clear what is in function \"CompOp::calculate\"\n", this->get_num());
	return 1;
}


int Scope::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	std::shared_ptr<Scope_table> scp_table = std::make_shared<Scope_table>(creator.get_CURR_SCOPE());
	std::shared_ptr<Scope_table> old_table = creator.get_CURR_SCOPE();

	creator.set_CURR_SCOPE(scp_table);
	int res = 0;

	for (auto &&stmt : stmts_)
	{
		res = stmt->run_stmt(istr, ostr, creator);

		if (creator.is_return())
			break;
	}

	creator.set_CURR_SCOPE(old_table);

	return res;
}


int Function::calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const
{
	std::string func_name = this->short_name();

	std::shared_ptr<Statement> Definition = creator.get_CURR_SCOPE()->get_func_decl(func_name, this->get_num());
	std::shared_ptr<Declaration> Definit = std::static_pointer_cast<Declaration>(Definition);
	
	std::shared_ptr<Scope_table> func_table = std::make_shared<Main_Scope_table>(creator.get_CURR_SCOPE()->get_global_funcs());

	const std::vector<std::shared_ptr<Lex_t>> &args = Definit->get_args();
	
	if (args_.size() != args.size())
	{
		throw_exception("Incorrect number of function arguments\n", this->get_num());
	}

	for (int num_arg = 0, count_args = args.size(); num_arg < count_args; num_arg++)
	{
		std::string name = args[num_arg]->short_name();
		func_table->init_var(name);

		int val = args_[num_arg]->calculate(istr, ostr, creator);
		func_table->get_var(name, args_[num_arg]->get_str()) = val;
	}

	std::shared_ptr<Lex_t> scope = Definit->get_scope();

	std::shared_ptr<Scope_table> old_curr_scope = creator.get_CURR_SCOPE();
	creator.set_CURR_SCOPE(func_table);

	const std::vector<std::shared_ptr<Statement>> &stmts = std::static_pointer_cast<Scope>(scope)->get_lhs();

	int res = 0;
	for (auto &&stmt : stmts)
	{
		res = stmt->run_stmt(istr, ostr, creator);

		if (creator.is_return())
		{
			*(creator.set_return()) = 0;
			break;
		}
	}

	creator.set_CURR_SCOPE(old_curr_scope);

	return res;
}


//----------------------------------------------CODEGEN--------------------------------------------------


llvm::Value *Value::codegen(AST_creator &creator)
{
	if (type_ != Value_type::INPUT)
	{	
		const llvm::APInt value(32, this->get_data(), true);

		return llvm::ConstantInt::get(*AST_creator::TheContext, value);
	}

	llvm::BasicBlock *InsertBB = AST_creator::Builder->GetInsertBlock();
 	llvm::Function *func_scanf = AST_creator::TheModule->getFunction("scanf");

    if (!func_scanf)
    {
    	std::vector<llvm::Type *> Ints(0, llvm::Type::getInt32Ty(*AST_creator::TheContext));
	  	
	  	llvm::FunctionType *FuncType =
	    llvm::FunctionType::get(llvm::Type::getInt32Ty(*AST_creator::TheContext), Ints, false);

	  	func_scanf = 
	  	llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "scanf", AST_creator::TheModule.get());

        func_scanf->setCallingConv(llvm::CallingConv::C);
    }

    llvm::Value *str = AST_creator::Builder->CreateGlobalStringPtr("%d");
    std::vector <llvm::Value*> call_params;
    call_params.push_back(str);
 
    llvm::CallInst *call = llvm::CallInst::Create(func_scanf, call_params, "calltmp", InsertBB);
    
   	return call;
}


llvm::Value *Variable::codegen(AST_creator &creator)
{
	std::string var_name = this->short_name();

	llvm::AllocaInst *Var = creator.get_CURR_SCOPE()->get_var_addr(var_name, this->get_num());

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", this->get_num());
  	}

  	return AST_creator::Builder->CreateLoad(Var->getAllocatedType(), Var, var_name.c_str());
}


llvm::Value *Negation::codegen(AST_creator &creator)
{
	llvm::Value *right = rhs_->codegen(creator);
	const llvm::APInt zero(32, 0, true);
  	
  	right = AST_creator::Builder->CreateICmpEQ(right, 
    llvm::ConstantInt::get(*AST_creator::TheContext, zero), "neg");

    right = AST_creator::Builder->CreateUIToFP(right, llvm::Type::getDoubleTy(*AST_creator::TheContext), "doubletmp");
	return AST_creator::Builder->CreateFPToSI(right, llvm::Type::getInt32Ty(*AST_creator::TheContext), "inttmp");
}


llvm::Value *Assign_node::codegen(AST_creator &creator)
{
	llvm::Value *Val = rhs_->codegen(creator);

  	if (!Val)
  	{
  		throw_exception("Non-existing operand\n", rhs_->get_num());
  	}
	
	std::shared_ptr<Lex_t> var = lhs_;
	std::string var_name;

	if (std::dynamic_pointer_cast<Variable>(var))
	{
		var_name = var->short_name();
	}
	else
	{
		var = get_variable();
		var_name = var->short_name();
	}

	llvm::AllocaInst *Var = creator.get_CURR_SCOPE()->alloca_var(var_name, this->get_num());

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", this->get_num());
  	}

  	AST_creator::Builder->CreateStore(Val, Var);

  	return Val;
}


llvm::Value *Scope::codegen(AST_creator &creator)
{
	std::shared_ptr<Scope_table> curr_scope = std::make_shared<Scope_table>(creator.get_CURR_SCOPE());
	std::shared_ptr<Scope_table> old_curr_scope = creator.get_CURR_SCOPE();
	
	creator.set_CURR_SCOPE(curr_scope);

  	
	const llvm::APInt zero(32, 0, true);
		
	if (!stmts_.size())
	{
		return llvm::ConstantInt::get(*AST_creator::TheContext, zero);
	}
	
	llvm::Value *last_expr = llvm::ConstantInt::get(*AST_creator::TheContext, zero);

	for (auto &elem : stmts_)
	{
		if (std::dynamic_pointer_cast<Declaration>(elem))
		{
			elem->codegen_func(creator);
		}
		else
		{
			last_expr = elem->codegen(creator);
		}
	}

	creator.set_CURR_SCOPE(old_curr_scope);

	return last_expr;
}


llvm::Value *Function::codegen(AST_creator &creator)
{
	std::string func_name = this->short_name();
	llvm::Function *Call = AST_creator::TheModule->getFunction(func_name);

	if (!Call)
	{
		Call = creator.get_CURR_SCOPE()->get_func_addr(func_name, this->get_num());
  	}

  	int args_size = args_.size();

	if (static_cast<int>(Call->arg_size()) != args_size)
	{
		throw_exception("Incorrect count arguments\n", this->get_num());
	}
   
	std::vector<llvm::Value*> ArgsV;

  	for (auto &&arg : args_)
  	{
  		ArgsV.push_back(arg->codegen(creator));
    }
    
  	return AST_creator::Builder->CreateCall(Call, ArgsV, "calltmp");
}


llvm::Value *UnOp::codegen(AST_creator &creator)
{
	const llvm::APInt one(32, 1, true);	
	llvm::Value *One = llvm::ConstantInt::get(*AST_creator::TheContext, one);
	
	std::shared_ptr<Lex_t> var = get_variable();
	std::string var_name;

	llvm::Value *Var = var->codegen(creator);

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", this->get_num());
  	}

  	llvm::Value *Val;

	switch (this->get_data())
	{
		case Statements_t::INC:
			Val = AST_creator::Builder->CreateAdd(Var, One, "Inctmp");
			break;
		case Statements_t::DEC:
			Val = AST_creator::Builder->CreateSub(Var, One, "Dectmp");
			break;
	}

  	if (!Val)
  	{
  		throw_exception("Non-existing operand\n", lhs_->get_num());
  	}

  	AST_creator::Builder->CreateStore(Val, Var);

  	return Val;
}


llvm::Value *BinOp::codegen(AST_creator &creator)
{
	llvm::Value *left = lhs_->codegen(creator);
  	llvm::Value *right = rhs_->codegen(creator);

  	if (!left || !right) 
  	{
  		throw_exception("Non-existing operand\n", this->get_num());
  	};

	switch (this->get_data())
	{
		case BinOp_t::ADD:
			return AST_creator::Builder->CreateAdd(left, right, "addtmp");
		case BinOp_t::SUB:
			return AST_creator::Builder->CreateSub(left, right, "subtmp");
		case BinOp_t::MULT:
			return AST_creator::Builder->CreateMul(left, right, "multtmp");
		case BinOp_t::DIV:
			if (right == 0)
			{
				throw_exception("Division by zero\n", this->get_num());
			}
			return AST_creator::Builder->CreateSDiv(left, right, "divtmp");
	}

	throw_exception("Error: it is not clear what is in function \"BinOp::Codegen\"\n", this->get_num());
	return nullptr;
}


llvm::Value *CompOp::codegen(AST_creator &creator)
{
	llvm::Value *left = lhs_->codegen(creator);
  	llvm::Value *right = rhs_->codegen(creator);

  	if (!left || !right) 
  	{
  		throw_exception("Non-existing operand\n", this->get_num());
  	};
  
	switch (this->get_data())
	{
		case CompOp_t::LESS:
			left = AST_creator::Builder->CreateICmpSLT(left, right, "lesstmp");
			break;
		case CompOp_t::GREATER:
			left = AST_creator::Builder->CreateICmpSGT(left, right, "greatertmp");
			break;
		case CompOp_t::LESorEQ:
			left = AST_creator::Builder->CreateICmpSLE(left, right, "LESorEQtmp");
			break;
		case CompOp_t::GRorEQ:
			left = AST_creator::Builder->CreateICmpSGE(left, right, "GRorEQtmp");
			break;
		case CompOp_t::EQUAL:
			left = AST_creator::Builder->CreateICmpEQ(left, right, "EQtmp");
			break;
		case CompOp_t::NOT_EQUAL:
			left = AST_creator::Builder->CreateICmpNE(left, right, "NEtmp");
			break;
	}
	
	left = AST_creator::Builder->CreateUIToFP(left, llvm::Type::getDoubleTy(*AST_creator::TheContext), "doubletmp");
	return AST_creator::Builder->CreateFPToSI(left, llvm::Type::getInt32Ty(*AST_creator::TheContext), "inttmp");
}


//------------------------------------------------NAMES---------------------------------------------------


std::string Lex_t::name() const
{
	switch (kind_)
	{
	case Lex_kind_t::BRACE:
		return static_cast<std::string>("BRACE:") + ((data_ == Brace_t::LBRACE) ? "LBRACE" : "RBRACE");
	case Lex_kind_t::UNOP:
		return static_cast<std::string>("UNOP:") + ((data_ == Statements_t::INC) ? "INC" : "DEC");
	case Lex_kind_t::SCOPE:
		return static_cast<std::string>("SCOPE:") + ((data_ == Scope_t::LSCOPE) ? "LSCOPE" : "RSCOPE");
	case Lex_kind_t::VALUE:
		return static_cast<std::string>("VALUE:") + std::to_string(data_);
	case Lex_kind_t::STMT:
	{
		std::string type;
		switch (data_)
		{
		case Statements_t::ASSIGN:
			type = "=";
			break;
		case Statements_t::IF:
			type = "if";
			break;
		case Statements_t::WHILE:
			type = "while";
			break;
		case Statements_t::FUNC:
			type = "func";
			break;
		case Statements_t::RETURN:
			type = "return";
			break;
		case Statements_t::PRINT:
			type = "print";
			break;
		}
		return static_cast<std::string>("STMT:") + type;
	}
	case Lex_kind_t::SYMBOL:
	{
		std::string type;
		switch (data_)
		{
		case Symbols_t::ELSE:
			type = "else";
			break;
		case Symbols_t::SCAN:
			type = "?";
			break;
		case Symbols_t::SEMICOL:
			type = ";";
			break;
		case Symbols_t::COLON:
			type = ":";
			break;
		case Symbols_t::COMMA:
			type = ",";
			break;
		case Symbols_t::NEGATION:
			type = "!";
			break;
		}
		return static_cast<std::string>("SYMBOL:") + type;
	}
	case Lex_kind_t::BAD_SYMBOL:
	{
		std::string ret;
		ret.push_back(data_);
		return ret;
		return static_cast<std::string>("STMT:") + ret;
	}
	case Lex_kind_t::BINOP:
	{
		std::string type;
		switch (data_)
		{
		case BinOp_t::ADD:
			type = "+";
			break;
		case BinOp_t::SUB:
			type = "-";
			break;
		case BinOp_t::MULT:
			type = "*";
			break;
		case BinOp_t::DIV:
			type = "/";
			break;
		}
		return static_cast<std::string>("BINOP:") + type;
	}
	case Lex_kind_t::COMPOP:
	{
		std::string type;
		switch (data_)
		{
		case CompOp_t::LESS:
			type = "<";
			break;
		case CompOp_t::GREATER:
			type = ">";
			break;
		case CompOp_t::LESorEQ:
			type = "<=";
			break;
		case CompOp_t::GRorEQ:
			type = ">=";
			break;
		case CompOp_t::EQUAL:
			type = "==";
			break;
		case CompOp_t::NOT_EQUAL:
			type = "!=";
			break;

		}
		return static_cast<std::string>("COMPOP:") + type;
	}
	case Lex_kind_t::VAR:
		return static_cast<std::string>("VAR:") + name_;
	case Lex_kind_t::FUNCTION:
		return static_cast<std::string>("FUNCTION:") + name_;
	}
	return nullptr;
}


//---------------------------------------SHORT_NAMES---------------------------------------------------


std::string Lex_t::short_name() const
{
	switch (kind_)
	{
	case Lex_kind_t::VAR:
		return name_;
	case Lex_kind_t::FUNCTION:
		return name_;
	case Lex_kind_t::BRACE:
		return ((data_ == Brace_t::LBRACE) ? "(" : ")");
	case Lex_kind_t::UNOP:
		return ((data_ == Statements_t::INC) ? "++" : "--");
	case Lex_kind_t::SCOPE:
		return ((data_ == Scope_t::LSCOPE) ? "{" : "}");
	case Lex_kind_t::VALUE:
		return std::to_string(data_);
	case Lex_kind_t::STMT:
		switch (data_)
		{
		case Statements_t::ASSIGN:
			return "=";
		case Statements_t::PRINT:
			return "print";
		case Statements_t::FUNC:
			return "func";
		case Statements_t::RETURN:
			return "return";
		case Statements_t::IF:
			return "if";
		case Statements_t::WHILE:
			return "while";
		}
	case Lex_kind_t::SYMBOL:
		switch (data_)
		{
		case Symbols_t::ELSE:
			return "else";
		case Symbols_t::SEMICOL:
			return ";";
		case Symbols_t::COLON:
			return ":";
		case Symbols_t::COMMA:
			return ",";
		case Symbols_t::SCAN:
			return "?";
		case Symbols_t::NEGATION:
			return "!";
		}
	case Lex_kind_t::BAD_SYMBOL:
		{
			std::string ret;
			ret.push_back(data_);
			return ret;
		}
	case Lex_kind_t::BINOP:
		switch (data_)
		{
		case BinOp_t::ADD:
			return "+";
		case BinOp_t::SUB:
			return "-";
		case BinOp_t::MULT:
			return "*";
		case BinOp_t::DIV:
			return "/";
		}
		
	case Lex_kind_t::COMPOP:
		switch (data_)
		{
		case CompOp_t::LESS:
			return "less";
		case CompOp_t::GREATER:
			return "greater";
		case CompOp_t::LESorEQ:
			return "lesOReq";
		case CompOp_t::GRorEQ:
			return "grOReq";
		case CompOp_t::EQUAL:
			return "==";
		case CompOp_t::NOT_EQUAL:
			return "!=";
		}
	}
	return nullptr;
}

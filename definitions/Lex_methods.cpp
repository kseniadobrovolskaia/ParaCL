#include "Stmt_methods.hpp"


//---------------------------------------------CALCULATE----------------------------------------------------


int Value::calculate(std::istream &istr, std::ostream &ostr) const
{
	if (type_ != Value_type::INPUT)
	{	
		return this->get_data();
	}

	int number;

	istr >> std::ws;
	istr >> number;

	return number;
}


int Variable::calculate(std::istream &istr, std::ostream &ostr) const
{
	std::string var_name = Lex_t::vars_table()[this->get_data()];

	return AST_creator::CURR_SCOPE->get_var(var_name, this->get_num());
}


int Negation::calculate(std::istream &istr, std::ostream &ostr) const
{
	int right = rhs_->calculate(istr, ostr);

	return !right;
}


int Assign_node::calculate(std::istream &istr, std::ostream &ostr) const
{
	Lex_t *var = lhs_.get();
	std::string var_name;

	int right_part = rhs_->calculate(istr, ostr);

	if (dynamic_cast<Variable*>(var))
	{
		var_name = Lex_t::vars_table()[var->get_data()];
		AST_creator::CURR_SCOPE->init_var(var_name);
	}
	else
	{
		var = &get_variable();
		var_name = Lex_t::vars_table()[var->get_data()];
	}

	lhs_->calculate(istr, ostr);

	AST_creator::CURR_SCOPE->get_var(var_name, var->get_num()) = right_part;

	return AST_creator::CURR_SCOPE->get_var(var_name, var->get_num());
}


int BinOp::calculate(std::istream &istr, std::ostream &ostr) const
{
	int left = lhs_->calculate(istr, ostr);
	int right = rhs_->calculate(istr, ostr);

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


int UnOp::calculate(std::istream &istr, std::ostream &ostr) const
{
	lhs_->calculate(istr, ostr);
	Lex_t *var = lhs_.get();

	std::string var_name;
	
	if (!dynamic_cast<Variable*>(var))
	{
		var = &get_variable();
	}
	
	var_name = Lex_t::vars_table()[var->get_data()];

	switch (this->get_data())
	{
		case Statements_t::INC:
			AST_creator::CURR_SCOPE->get_var(var_name, this->get_num()) += 1;
			return AST_creator::CURR_SCOPE->get_var(var_name, this->get_num());
		case Statements_t::DEC:
			AST_creator::CURR_SCOPE->get_var(var_name, this->get_num()) -= 1;
			return AST_creator::CURR_SCOPE->get_var(var_name, this->get_num());
	}

	throw_exception("Error: it is not clear what is in function \"UnOp::calculate\"\n", this->get_num());
	return 1;
}


int CompOp::calculate(std::istream &istr, std::ostream &ostr) const
{
	int left = lhs_->calculate(istr, ostr);
	int right = rhs_->calculate(istr, ostr);

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


int Scope::calculate(std::istream &istr, std::ostream &ostr) const
{
	std::shared_ptr<Scope_table> scp_table = std::make_shared<Scope_table>(AST_creator::CURR_SCOPE);
	std::shared_ptr<Scope_table> old_table = AST_creator::CURR_SCOPE;

	AST_creator::CURR_SCOPE = scp_table;
	int res = 0;

	// auto run = [&](std::shared_ptr<Statement> stmt)
	// 			 { stmt->run_stmt(istr, ostr); return AST_creator::RETURN_COMMAND; };

	// std::find_if(stmts_.begin(), stmts_.end(), run); //this does not work:(

	for (auto &&stmt : stmts_)
	{
		res = stmt->run_stmt(istr, ostr);

		if (AST_creator::RETURN_COMMAND)
			break;
	}

	AST_creator::CURR_SCOPE = old_table;

	return res;
}


int Function::calculate(std::istream &istr, std::ostream &ostr) const
{
	std::string func_name = this->short_name();

	std::shared_ptr<Statement> Definition = AST_creator::CURR_SCOPE->get_func_decl(func_name, this->get_num());
	Declaration *Definit = static_cast<Declaration*>(Definition.get());
	
	AST_creator::IN_FUNCTION++;
	int res = 0;
	std::shared_ptr<Scope_table> parent_table = (Definit->get_scope_table())->get_high_scope();

	std::shared_ptr<Scope_table> func_table = std::make_shared<Scope_table>(parent_table);

	const std::vector<std::shared_ptr<Lex_t>> &args = Definit->get_args();
	
	if (args_.size() != args.size())
	{
		throw_exception("Incorrect number of function arguments\n", this->get_num());
	}

	for (int num_arg = 0, count_args = args.size(); num_arg < count_args; num_arg++)
	{
		std::string name = args[num_arg]->short_name();
		func_table->init_var(name);

		int val = args_[num_arg]->calculate(istr, ostr);
		func_table->get_var(name, args_[num_arg]->get_str()) = val;
	}

	std::shared_ptr<Lex_t> scope = Definit->get_scope();

	std::shared_ptr<Scope_table> old_curr_scope = AST_creator::CURR_SCOPE;
	AST_creator::CURR_SCOPE = func_table;

	const std::vector<std::shared_ptr<Statement>> &stmts = static_cast<Scope*>(scope.get())->get_lhs();

	for (auto &&stmt : stmts)
	{
		res = stmt->run_stmt(istr, ostr);

		if (AST_creator::RETURN_COMMAND)
		{
			AST_creator::RETURN_COMMAND = 0;
			break;
		}
	}

	AST_creator::CURR_SCOPE = old_curr_scope;

	AST_creator::IN_FUNCTION--;
	return res;
}


//----------------------------------------------CODEGEN--------------------------------------------------


llvm::Value *Value::codegen()
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


llvm::Value *Variable::codegen()
{
	std::string var_name = Lex_t::vars_table()[this->get_data()];

	llvm::AllocaInst *Var = AST_creator::CURR_SCOPE->get_var_addr(var_name, this->get_num());

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", this->get_num());
  	}

  	return AST_creator::Builder->CreateLoad(Var->getAllocatedType(), Var, var_name.c_str());
}


llvm::Value *Negation::codegen()
{
	llvm::Value *right = rhs_->codegen();
	const llvm::APInt zero(32, 0, true);
  	
  	right = AST_creator::Builder->CreateICmpEQ(right, 
    llvm::ConstantInt::get(*AST_creator::TheContext, zero), "neg");

    right = AST_creator::Builder->CreateUIToFP(right, llvm::Type::getDoubleTy(*AST_creator::TheContext), "doubletmp");
	return AST_creator::Builder->CreateFPToSI(right, llvm::Type::getInt32Ty(*AST_creator::TheContext), "inttmp");
}


llvm::Value *Assign_node::codegen()
{
	llvm::Value *Val = rhs_->codegen();

  	if (!Val)
  	{
  		throw_exception("Non-existing operand\n", rhs_->get_num());
  	}
	
	Lex_t *var = lhs_.get();
	std::string var_name;

	if (dynamic_cast<Variable*>(var))
	{
		var_name = Lex_t::vars_table()[var->get_data()];
	}
	else
	{
		var = &get_variable();
		var_name = Lex_t::vars_table()[var->get_data()];
	}

	llvm::AllocaInst *Var = AST_creator::CURR_SCOPE->alloca_var(var_name, this->get_num());

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", this->get_num());
  	}

  	AST_creator::Builder->CreateStore(Val, Var);

  	return Val;
}


llvm::Value *Scope::codegen()
{
	std::shared_ptr<Scope_table> curr_scope = std::make_shared<Scope_table>(AST_creator::CURR_SCOPE);
	std::shared_ptr<Scope_table> old_curr_scope = AST_creator::CURR_SCOPE;
	
	AST_creator::CURR_SCOPE = curr_scope;

  	
	const llvm::APInt zero(32, 0, true);
		
	if (!stmts_.size())
	{
		return llvm::ConstantInt::get(*AST_creator::TheContext, zero);
	}
	
	llvm::Value *last_expr = llvm::ConstantInt::get(*AST_creator::TheContext, zero);

	for (auto &elem : stmts_)
	{
		if (dynamic_cast<Declaration*>(elem.get()))
		{
			elem->codegen_func();
		}
		else
		{
			last_expr = elem->codegen();
		}
	}

	AST_creator::CURR_SCOPE = old_curr_scope;

	return last_expr;
}


llvm::Value *Function::codegen()
{
	std::string func_name = this->short_name();
	llvm::Function *Call = AST_creator::TheModule->getFunction(func_name);

	if (!Call)
	{
		Call = AST_creator::CURR_SCOPE->get_func_addr(func_name, this->get_num());
  	}

  	int args_size = args_.size();

	if (static_cast<int>(Call->arg_size()) != args_size)
	{
		throw_exception("Incorrect count arguments\n", this->get_num());
	}
   
	std::vector<llvm::Value*> ArgsV;

  	for (auto &&arg : args_)
  	{
  		ArgsV.push_back(arg->codegen());
    }
    
  	return AST_creator::Builder->CreateCall(Call, ArgsV, "calltmp");
}


llvm::Value *UnOp::codegen()
{
	const llvm::APInt one(32, 1, true);	
	llvm::Value *One = llvm::ConstantInt::get(*AST_creator::TheContext, one);
	
	Lex_t *var = &get_variable();
	std::string var_name;

	llvm::Value *Var = var->codegen();

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


llvm::Value *BinOp::codegen()
{
	llvm::Value *left = lhs_->codegen();
  	llvm::Value *right = rhs_->codegen();

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


llvm::Value *CompOp::codegen()
{
	llvm::Value *left = lhs_->codegen();
  	llvm::Value *right = rhs_->codegen();

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
		return static_cast<std::string>("VAR:") + vars_[data_];
	case Lex_kind_t::FUNCTION:
		return static_cast<std::string>("FUNCTION:") + funcs_[data_];
	}
	return nullptr;
}


//---------------------------------------SHORT_NAMES---------------------------------------------------


std::string Lex_t::short_name() const
{
	switch (kind_)
	{
	case Lex_kind_t::VAR:
		return vars_[data_];
	case Lex_kind_t::FUNCTION:
		return funcs_[data_];
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

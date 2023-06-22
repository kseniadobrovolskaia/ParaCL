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
	AST_creator::IN_FUNCTION++;
	int res = 0;
	std::shared_ptr<Scope_table> func_table_ = std::make_shared<Scope_table>();

	const std::vector<std::shared_ptr<Lex_t>> &args = static_cast<Declaration*>(decl_.get())->get_args();
	
	if (args_.size() != args.size())
	{
		throw_exception("Incorrect number of function arguments\n", this->get_num());
	}

	for (int num_arg = 0, count_args = args.size(); num_arg < count_args; num_arg++)
	{
		std::string name = args[num_arg]->short_name();
		func_table_->init_var(name);

		int val = args_[num_arg]->calculate(istr, ostr);
		func_table_->get_var(name, args_[num_arg]->get_str()) = val;
	}

	std::shared_ptr<Lex_t> scope = static_cast<Declaration*>(decl_.get())->get_scope();

	std::shared_ptr<Scope_table> old_curr_scope = AST_creator::CURR_SCOPE;
	AST_creator::CURR_SCOPE = func_table_;

	const std::vector<std::shared_ptr<Statement>> &stmts_ = static_cast<Scope*>(scope.get())->get_lhs();

	for (auto &&stmt : stmts_)
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
	const llvm::APInt value(32, this->get_data(), true);

	return llvm::ConstantInt::get(*AST_creator::TheContext, value);
}


llvm::Value *Variable::codegen()
{
	std::string var_name = Lex_t::vars_table()[this->get_data()];

	llvm::Value *Var = AST_creator::NamedValues[var_name];

  	if (!Var)
  	{
  		throw_exception("Unknown variable name\n", this->get_num());
  	}

  	return Var;
}


llvm::Value *Negation::codegen()
{
	llvm::Value *right = rhs_->codegen();
	const llvm::APInt zero(32, 0, true);
  	
  	return AST_creator::Builder->CreateICmpNE(right, 
    llvm::ConstantInt::get(*AST_creator::TheContext, zero), "neg");
}


#if 0

I fix it
llvm::Value *Assign_node::codegen()
{
	Lex_t *var = lhs_.get();
	std::string var_name;

	int right_part = rhs_->calculate(istr, ostr);

	if (dynamic_cast<Variable*>(var))
	{
		var_name = vars[var->get_data()];
		CURR_SCOPE->init_var(var_name);
	}
	else
	{
		var = &get_variable();
		var_name = vars[var->get_data()];
	}

	lhs_->calculate(istr, ostr);

	CURR_SCOPE->get_var(var_name, var->get_num()) = right_part;

	return CURR_SCOPE->get_var(var_name, var->get_num());
}

#endif


llvm::Value *Scope::codegen()
{
	//I fix it
	if (dynamic_cast<If*>((stmts_[0]).get()))
	{
		return dynamic_cast<If*>((stmts_[0]).get())->codegen_if();
	}

	return (stmts_[0]->get_lhs()).codegen();
}


llvm::Value *Function::codegen()
{
	// or simple name()
	std::string func_name = static_cast<Declaration*>(decl_.get())->get_func_name();
	llvm::Function *Call = AST_creator::TheModule->getFunction(func_name);

	if (!Call)
	{
    	throw_exception("Unknown variable name\n", this->get_num());
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
	
	llvm::Value *left = lhs_->codegen();
	llvm::Value *right = llvm::ConstantInt::get(*AST_creator::TheContext, one);

  	if (!left || !right) 
  	{
  		throw_exception("Non-existing operand\n", this->get_num());
  	};

	switch (this->get_data())
	{
		case Statements_t::INC:
			return AST_creator::Builder->CreateAdd(left, right, "addtmp");
		case Statements_t::DEC:
			return AST_creator::Builder->CreateSub(left, right, "subtmp");
	}
			
	throw_exception("Error: it is not clear what is in function \"UnOp::Codegen\"\n", this->get_num());
	return nullptr;
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
			return AST_creator::Builder->CreateICmpULT(left, right, "lesstmp");
		case CompOp_t::GREATER:
			return AST_creator::Builder->CreateICmpUGT(left, right, "greatertmp");
		case CompOp_t::LESorEQ:
			return AST_creator::Builder->CreateICmpULE(left, right, "LESorEQtmp");
		case CompOp_t::GRorEQ:
			return AST_creator::Builder->CreateICmpUGE(left, right, "GRorEQtmp");
		case CompOp_t::EQUAL:
			return AST_creator::Builder->CreateICmpEQ(left, right, "EQtmp");
		case CompOp_t::NOT_EQUAL:
			return AST_creator::Builder->CreateICmpNE(left, right, "NEtmp");
	}

	throw_exception("Error: it is not clear what is in function \"CompOp::Codegen\"\n", this->get_num());
	return nullptr;
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

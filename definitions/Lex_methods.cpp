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


llvm::Value *Value::codegen(Codegen_creator &creator)
{
	return creator.codegen_value(*this);
}


llvm::Value *Variable::codegen(Codegen_creator &creator)
{
	return creator.codegen_variable(*this);
}


llvm::Value *Negation::codegen(Codegen_creator &creator)
{
	return creator.codegen_negation(*this);
}


llvm::Value *Assign_node::codegen(Codegen_creator &creator)
{
	return creator.codegen_assign(*this);
}


llvm::Value *Scope::codegen(Codegen_creator &creator)
{
	return creator.codegen_scope(*this);
}


llvm::Value *Function::codegen(Codegen_creator &creator)
{
	return creator.codegen_call(*this);
}


llvm::Value *UnOp::codegen(Codegen_creator &creator)
{
	return creator.codegen_unop(*this);
}


llvm::Value *BinOp::codegen(Codegen_creator &creator)
{
	return creator.codegen_binop(*this);
}


llvm::Value *CompOp::codegen(Codegen_creator &creator)
{
	return creator.codegen_compop(*this);
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

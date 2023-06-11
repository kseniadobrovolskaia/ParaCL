#include "Lex_methods.hpp"



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
	std::string var_name = vars[this->get_data()];

	return CURR_SCOPE->get_var(var_name, this->get_num());
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
	
	var_name = vars[var->get_data()];

	switch (this->get_data())
	{
		case Statements_t::INC:
			CURR_SCOPE->get_var(var_name, this->get_num()) += 1;
			return CURR_SCOPE->get_var(var_name, this->get_num());
		case Statements_t::DEC:
			CURR_SCOPE->get_var(var_name, this->get_num()) -= 1;
			return CURR_SCOPE->get_var(var_name, this->get_num());
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
	std::shared_ptr<Scope_table> scp_table = std::make_shared<Scope_table>(CURR_SCOPE);
	std::shared_ptr<Scope_table> old_table = CURR_SCOPE;

	CURR_SCOPE = scp_table;
	int res = 0;

	// auto run = [&](std::shared_ptr<Statement> stmt)
	// 			 { stmt->run_stmt(istr, ostr); return RETURN_COMMAND; };

	// std::find_if(stmts_.begin(), stmts_.end(), run); //this does not work:(

	for (auto &&stmt : stmts_)
	{
		res = stmt->run_stmt(istr, ostr);

		if (RETURN_COMMAND)
			break;
	}

	CURR_SCOPE = old_table;

	return res;
}


int Function::calculate(std::istream &istr, std::ostream &ostr) const
{
	IN_FUNCTION++;
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

	std::shared_ptr<Scope_table> old_curr_scope = CURR_SCOPE;
	CURR_SCOPE = func_table_;

	const std::vector<std::shared_ptr<Statement>> &stmts_ = static_cast<Scope*>(scope.get())->get_lhs();

	for (auto &&stmt : stmts_)
	{
		res = stmt->run_stmt(istr, ostr);

		if (RETURN_COMMAND)
		{
			RETURN_COMMAND = 0;
			break;
		}
	}

	CURR_SCOPE = old_curr_scope;

	IN_FUNCTION--;
	return res;
}

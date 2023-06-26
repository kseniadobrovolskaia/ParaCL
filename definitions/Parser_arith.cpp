#include "Parser_stmts.hpp"



//---------------------------------------------PARSERS-----------------------------------------------------


std::shared_ptr<Lex_t> Parser::parse_arithmetic()
{
	std::shared_ptr<Lex_t> root;

	root = parse_asgn();

	return root;
}


std::shared_ptr<Lex_t> Parser::parse_asgn()
{
	std::shared_ptr<Lex_t> R, as, L = parse_bool();
	bool is_as;
	
	while (true)
	{
		is_as = lex_array_->is_assign();
		if (!is_as)
		{
			return L;
		}
		else
		{
			std::shared_ptr<Lex_t> var = L;

			if (std::dynamic_pointer_cast<Ref_t>(var))
			{
				var = std::dynamic_pointer_cast<Ref_t>(var)->get_variable();
			}
			else if (var->get_kind() != Lex_kind_t::VAR)
			{
				throw_exception("You can not use assign without variable\n", lex_array_->get_num_curr_lex());	
			}

			as = lex_array_->get_curr_lex();
			lex_array_->inc_lex();
			R = parse_bool();

			L = std::make_shared<Assign_node>(L, R, var, *as);
		}
	}
}


std::shared_ptr<Lex_t> Parser::parse_bool()
{
	std::shared_ptr<Lex_t> R, comp, L = parse_E();
	int is_comp;
	
	while (true)
	{
		is_comp = lex_array_->is_compop();
		if (is_comp < 0)
		{
			return L;
		}
		else
		{
			comp = lex_array_->get_curr_lex();
			lex_array_->inc_lex();
			R = parse_E();	
			L = std::make_shared<CompOp>(L, R, *comp);
		}
	}
}


std::shared_ptr<Lex_t> Parser::parse_E()
{
	std::shared_ptr<Lex_t> R, expr, L = parse_M();
	int is_p_m;
	
	while (true)
	{
		is_p_m = lex_array_->is_plus_minus();
		if (is_p_m < 0)
		{
			return L;
		}
		else
		{
			expr = lex_array_->get_curr_lex();
			lex_array_->inc_lex();
			R = parse_M();	
			L = std::make_shared<BinOp>(L, R, *expr);
		}
	}
}


std::shared_ptr<Lex_t> Parser::parse_M()
{
	std::shared_ptr<Lex_t> R, mult, L = parse_negation();
	int is_m_d;

	while (true)
	{
		is_m_d = lex_array_->is_mul_div();
		if (is_m_d < 0)
		{
			return L;
		}
		else
		{
			mult = lex_array_->get_curr_lex();
			lex_array_->inc_lex();
			R = parse_negation();
			L = std::make_shared<BinOp>(L, R, *mult);
		}
	}
}


std::shared_ptr<Lex_t> Parser::parse_negation() // !expression
{
	std::shared_ptr<Lex_t> L, R, neg;
	bool is_neg;

	while (true)
	{
		is_neg = lex_array_->is_negation();

		if (!is_neg)
		{
			L = parse_negative();	
		}
		else
		{
			neg = lex_array_->get_curr_lex();
			lex_array_->inc_lex();
			R = parse_negative();	
			L = std::make_shared<Negation>(R, *neg);
		}

		return L;
	}
}


std::shared_ptr<Lex_t> Parser::parse_negative() // -expression
{
	std::shared_ptr<Lex_t> L, R, neg;

	int is_p_m = lex_array_->is_plus_minus();

	if ((is_p_m >= 0) && (static_cast<BinOp_t>(is_p_m) == BinOp_t::SUB))
	{
		neg = lex_array_->get_curr_lex();
		lex_array_->inc_lex();
		R = parse_unary();
		std::shared_ptr<Lex_t> nul = std::make_shared<Value>(Lex_t(Lex_kind_t::VALUE, 0, neg->get_str(), neg->get_num(), "0"), Value_type::NUMBER);
		L = std::make_shared<BinOp>(nul, R, *neg);
	}
	else
	{
		L = parse_unary();
	}

	return L;
}
			

std::shared_ptr<Lex_t> Parser::parse_unary()
{
	std::shared_ptr<Lex_t> unop, L = parse_T();
	int is_unary;

	while (true)
	{
		is_unary = lex_array_->is_unop();
		if (is_unary < 0)
		{
			return L;
		}
		else
		{
			std::shared_ptr<Lex_t> var = L;

			if (std::dynamic_pointer_cast<Ref_t>(var))
			{
				var = std::dynamic_pointer_cast<Ref_t>(var)->get_variable();
			}
			else if (var->get_kind() != Lex_kind_t::VAR)
			{
				throw_exception("You can not use unary operator without variable\n", lex_array_->get_num_curr_lex());	
			}

			unop = lex_array_->get_curr_lex();
			lex_array_->inc_lex();
			L = std::make_shared<UnOp>(L, var, *unop);
		}
	}
}


std::shared_ptr<Lex_t> Parser::parse_T()
{
	std::shared_ptr<Lex_t> T;

	if (lex_array_->is_brace() == Brace_t::LBRACE)
	{
		lex_array_->inc_lex();

		T = parse_asgn();

		if (lex_array_->is_brace() != Brace_t::RBRACE)
		{
			throw_exception("Invalid input: check brases in arithmetic expression\n", lex_array_->get_num_curr_lex());
		}

		lex_array_->inc_lex();

		return T;
	}
	
	switch (lex_array_->get_curr_lex()->get_kind())
	{
		case Lex_kind_t::VALUE:
		{
			T = std::make_shared<Value>(*(lex_array_->get_curr_lex()), Value_type::NUMBER);
			lex_array_->inc_lex();
			break;
		}
		case Lex_kind_t::VAR:
		{
			T = std::make_shared<Variable>(*(lex_array_->get_curr_lex()));
			lex_array_->inc_lex();
			break;
		}
		case Lex_kind_t::FUNCTION:
		{
			T = parse_function_call();
			break;
		}
		case Lex_kind_t::SYMBOL:
		{
			if (lex_array_->is_scan())
			{
				T = std::make_shared<Value>(*(lex_array_->get_curr_lex()), Value_type::INPUT);
				lex_array_->inc_lex();
				break;
			}
		}
		case Lex_kind_t::SCOPE:
		{
			if (lex_array_->is_scope() == Scope_t::LSCOPE)
			{
				T = parse_scope();
				break;
			}
		}
		default:
		{
			throw_exception("Something strange instead of a value/variable/scope\n", lex_array_->get_num_curr_lex());
		}
	}
	
	return T;
}


//-----------------------------------------PARSE_FUNCTION_CALL---------------------------------------------


std::shared_ptr<Lex_t> Parser::parse_function_call()
{
	std::shared_ptr<Lex_t> T;
	Lex_t &func = *(lex_array_->get_curr_lex());
	std::string name = func.short_name();
	std::vector <std::shared_ptr<Lex_t>> args;

	lex_array_->inc_lex();
	if (lex_array_->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Incorrect function call\n", lex_array_->get_num_curr_lex());
	}
	lex_array_->inc_lex();

	std::shared_ptr<Lex_t> arg;
	bool first_arg = 1;

	while (lex_array_->is_brace() != Brace_t::RBRACE)
	{
		if (first_arg)
		{
			first_arg = 0;
		}
		else
		{
			if (!lex_array_->is_comma())
			{
				throw_exception("Need comma between function args\n", lex_array_->get_num_curr_lex());
			}
			lex_array_->inc_lex();
		}
		arg = parse_arithmetic();
		args.push_back(arg);
	}

	lex_array_->inc_lex();

	T = std::make_shared<Function>(args, func);

	return T;
}


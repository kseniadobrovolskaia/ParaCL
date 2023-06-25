#include "Lex_methods.hpp"
#include <unordered_map>


std::shared_ptr<Lex_t> parse_function_call(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_arithmetic(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_negation(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_negative(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_unary(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_scope(std::shared_ptr<Lex_array_t> lex_array, bool reset = 0);
std::shared_ptr<Lex_t> parse_asgn(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_bool(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_E(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_M(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_T(std::shared_ptr<Lex_array_t> lex_array);


//---------------------------------------------PARSERS-----------------------------------------------------


std::shared_ptr<Lex_t> parse_arithmetic(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> root;

	root = parse_asgn(lex_array);

	return root;
}


std::shared_ptr<Lex_t> parse_asgn(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> R, as, L = parse_bool(lex_array);
	bool is_as;
	
	while (true)
	{
		is_as = lex_array->is_assign();
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
				throw_exception("You can not use assign without variable\n", lex_array->get_num_curr_lex());	
			}

			as = lex_array->get_curr_lex();
			lex_array->inc_lex();
			R = parse_bool(lex_array);

			L = std::make_shared<Assign_node>(L, R, var, *as);
		}
	}
}


std::shared_ptr<Lex_t> parse_bool(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> R, comp, L = parse_E(lex_array);
	int is_comp;
	
	while (true)
	{
		is_comp = lex_array->is_compop();
		if (is_comp < 0)
		{
			return L;
		}
		else
		{
			comp = lex_array->get_curr_lex();
			lex_array->inc_lex();
			R = parse_E(lex_array);	
			L = std::make_shared<CompOp>(L, R, *comp);
		}
	}
}


std::shared_ptr<Lex_t> parse_E(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> R, expr, L = parse_M(lex_array);
	int is_p_m;
	
	while (true)
	{
		is_p_m = lex_array->is_plus_minus();
		if (is_p_m < 0)
		{
			return L;
		}
		else
		{
			expr = lex_array->get_curr_lex();
			lex_array->inc_lex();
			R = parse_M(lex_array);	
			L = std::make_shared<BinOp>(L, R, *expr);
		}
	}
}


std::shared_ptr<Lex_t> parse_M(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> R, mult, L = parse_negation(lex_array);
	int is_m_d;

	while (true)
	{
		is_m_d = lex_array->is_mul_div();
		if (is_m_d < 0)
		{
			return L;
		}
		else
		{
			mult = lex_array->get_curr_lex();
			lex_array->inc_lex();
			R = parse_negation(lex_array);
			L = std::make_shared<BinOp>(L, R, *mult);
		}
	}
}


std::shared_ptr<Lex_t> parse_negation(std::shared_ptr<Lex_array_t> lex_array) // !expression
{
	std::shared_ptr<Lex_t> L, R, neg;
	bool is_neg;

	while (true)
	{
		is_neg = lex_array->is_negation();

		if (!is_neg)
		{
			L = parse_negative(lex_array);	
		}
		else
		{
			neg = lex_array->get_curr_lex();
			lex_array->inc_lex();
			R = parse_negative(lex_array);	
			L = std::make_shared<Negation>(R, *neg);
		}

		return L;
	}
}


std::shared_ptr<Lex_t> parse_negative(std::shared_ptr<Lex_array_t> lex_array) // -expression
{
	std::shared_ptr<Lex_t> L, R, neg;

	int is_p_m = lex_array->is_plus_minus();

	if ((is_p_m >= 0) && (static_cast<BinOp_t>(is_p_m) == BinOp_t::SUB))
	{
		neg = lex_array->get_curr_lex();
		lex_array->inc_lex();
		R = parse_unary(lex_array);
		std::shared_ptr<Lex_t> nul = std::make_shared<Value>(Lex_t(Lex_kind_t::VALUE, 0, neg->get_str(), neg->get_num(), "0"), Value_type::NUMBER);
		L = std::make_shared<BinOp>(nul, R, *neg);
	}
	else
	{
		L = parse_unary(lex_array);
	}

	return L;
}
			

std::shared_ptr<Lex_t> parse_unary(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> unop, L = parse_T(lex_array);
	int is_unary;

	while (true)
	{
		is_unary = lex_array->is_unop();
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
				throw_exception("You can not use unary operator without variable\n", lex_array->get_num_curr_lex());	
			}

			unop = lex_array->get_curr_lex();
			lex_array->inc_lex();
			L = std::make_shared<UnOp>(L, var, *unop);
		}
	}
}


std::shared_ptr<Lex_t> parse_T(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> T;

	if (lex_array->is_brace() == Brace_t::LBRACE)
	{
		lex_array->inc_lex();

		T = parse_asgn(lex_array);

		if (lex_array->is_brace() != Brace_t::RBRACE)
		{
			throw_exception("Invalid input: check brases in arithmetic expression\n", lex_array->get_num_curr_lex());
		}

		lex_array->inc_lex();

		return T;
	}
	
	switch (lex_array->get_curr_lex()->get_kind())
	{
		case Lex_kind_t::VALUE:
		{
			T = std::make_shared<Value>(*(lex_array->get_curr_lex()), Value_type::NUMBER);
			lex_array->inc_lex();
			break;
		}
		case Lex_kind_t::VAR:
		{
			T = std::make_shared<Variable>(*(lex_array->get_curr_lex()));
			lex_array->inc_lex();
			break;
		}
		case Lex_kind_t::FUNCTION:
		{
			T = parse_function_call(lex_array);
			break;
		}
		case Lex_kind_t::SYMBOL:
		{
			if (lex_array->is_scan())
			{
				T = std::make_shared<Value>(*(lex_array->get_curr_lex()), Value_type::INPUT);
				lex_array->inc_lex();
				break;
			}
		}
		case Lex_kind_t::SCOPE:
		{
			if (lex_array->is_scope() == Scope_t::LSCOPE)
			{
				T = parse_scope(lex_array);
				break;
			}
		}
		default:
		{
			throw_exception("Something strange instead of a value/variable/scope\n", lex_array->get_num_curr_lex());
		}
	}
	
	return T;
}


//-----------------------------------------PARSE_FUNCTION_CALL---------------------------------------------


std::shared_ptr<Lex_t> parse_function_call(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Lex_t> T;
	Lex_t &func = *(lex_array->get_curr_lex());
	std::string name = func.short_name();
	std::vector <std::shared_ptr<Lex_t>> args;

	lex_array->inc_lex();
	if (lex_array->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Incorrect function call\n", lex_array->get_num_curr_lex());
	}
	lex_array->inc_lex();

	std::shared_ptr<Lex_t> arg;
	bool first_arg = 1;

	while (lex_array->is_brace() != Brace_t::RBRACE)
	{
		if (first_arg)
		{
			first_arg = 0;
		}
		else
		{
			if (!lex_array->is_comma())
			{
				throw_exception("Need comma between function args\n", lex_array->get_num_curr_lex());
			}
			lex_array->inc_lex();
		}
		arg = parse_arithmetic(lex_array);
		args.push_back(arg);
	}

	lex_array->inc_lex();

	T = std::make_shared<Function>(args, func);

	return T;
}


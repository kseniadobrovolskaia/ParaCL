#include "Lex_methods.hpp"
#include <unordered_map>


std::shared_ptr<Lex_t> parse_function_call(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_arithmetic(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_negation(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_negative(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_unary(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_scope(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_asgn(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_bool(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_E(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_M(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_T(std::vector<std::shared_ptr<Lex_t>> &lex_array);

int is_plus_minus(Lex_t &node);
int is_mul_div(Lex_t &node);
int is_brace(Lex_t &node);
int is_compop(Lex_t &node);
int is_scope(Lex_t &node);
int is_unop(Lex_t &node);

bool is_negation(Lex_t &node);
bool is_semicol(Lex_t &node);
bool is_binop(Lex_t &node);
bool is_assign(Lex_t &node);
bool is_return(Lex_t &node);
bool is_comma(Lex_t &node);
bool is_colon(Lex_t &node);
bool is_scan(Lex_t &node);
bool is_else(Lex_t &node);


//---------------------------------------------PARSERS-----------------------------------------------------


std::shared_ptr<Lex_t> parse_arithmetic(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> root;

	if (is_scope(*lex_array[token_counter(USE_CURRENT)]) == Scope_t::RSCOPE)
	{
		root = parse_scope(lex_array);
	}
	else
	{
		root = parse_asgn(lex_array);
	}

	return root;
}


std::shared_ptr<Lex_t> parse_asgn(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> R, as, L = parse_bool(lex_array);
	int is_as;
	
	while (true)
	{
		is_as = is_assign(*lex_array[token_counter(USE_CURRENT)]);
		if (!is_as)
		{
			return L;
		}
		else
		{
			Lex_t *var = L.get();

			if (dynamic_cast<Ref_t*>(var))
			{
				var = &(dynamic_cast<Ref_t*>(var)->get_variable());
			}
			else if (var->get_kind() != Lex_kind_t::VAR)
			{
				throw_exception("You can not use assign without variable\n", token_counter(GET_CURRENT));	
			}

			as = lex_array[token_counter(USE_CURRENT)];
			token_counter(INCREMENT);
			R = parse_bool(lex_array);

			L = std::make_shared<Assign_node>(L, R, *var, *as);
		}
	}
}


std::shared_ptr<Lex_t> parse_bool(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> R, comp, L = parse_E(lex_array);
	int is_comp;
	
	while (true)
	{
		is_comp = is_compop(*lex_array[token_counter(USE_CURRENT)]);
		if (is_comp < 0)
		{
			return L;
		}
		else
		{
			comp = lex_array[token_counter(USE_CURRENT)];
			token_counter(INCREMENT);
			R = parse_E(lex_array);	
			L = std::make_shared<CompOp>(L, R, *comp);
		}
	}
}


std::shared_ptr<Lex_t> parse_E(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> R, expr, L = parse_M(lex_array);
	int is_p_m;
	
	while (true)
	{
		is_p_m = is_plus_minus(*lex_array[token_counter(USE_CURRENT)]);
		if (is_p_m < 0)
		{
			return L;
		}
		else
		{
			expr = lex_array[token_counter(USE_CURRENT)];
			token_counter(INCREMENT);
			R = parse_M(lex_array);	
			L = std::make_shared<BinOp>(L, R, *expr);
		}
	}
}


std::shared_ptr<Lex_t> parse_M(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> R, mult, L = parse_negation(lex_array);
	int is_m_d;

	while (true)
	{
		is_m_d = is_mul_div(*lex_array[token_counter(USE_CURRENT)]);
		if (is_m_d < 0)
		{
			return L;
		}
		else
		{
			mult = lex_array[token_counter(USE_CURRENT)];
			token_counter(INCREMENT);
			R = parse_negation(lex_array);
			L = std::make_shared<BinOp>(L, R, *mult);
		}
	}
}


std::shared_ptr<Lex_t> parse_negation(std::vector<std::shared_ptr<Lex_t>> &lex_array) // !expression
{
	std::shared_ptr<Lex_t> L, R, neg;
	int is_neg;

	while (true)
	{
		is_neg = is_negation(*lex_array[token_counter(USE_CURRENT)]);

		if (!is_neg)
		{
			L = parse_negative(lex_array);	
		}
		else
		{
			neg = lex_array[token_counter(USE_CURRENT)];
			token_counter(INCREMENT);
			R = parse_negative(lex_array);	
			L = std::make_shared<Negation>(R, *neg);
		}

		return L;
	}
}


std::shared_ptr<Lex_t> parse_negative(std::vector<std::shared_ptr<Lex_t>> &lex_array) // -expression
{
	std::shared_ptr<Lex_t> L, R, neg;

	int is_p_m = is_plus_minus(*lex_array[token_counter(USE_CURRENT)]);

	if ((is_p_m >= 0) && (static_cast<BinOp_t>(is_p_m) == BinOp_t::SUB))
	{
		neg = lex_array[token_counter(USE_CURRENT)];
		token_counter(INCREMENT);
		R = parse_unary(lex_array);
		std::shared_ptr<Lex_t> nul = std::make_shared<Value>(Lex_t(Lex_kind_t:: VALUE, 0, neg->get_str()), Value_type::NUMBER);
		L = std::make_shared<BinOp>(nul, R, *neg);
	}
	else
	{
		L = parse_unary(lex_array);
	}

	return L;
}
			

std::shared_ptr<Lex_t> parse_unary(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> unop, L = parse_T(lex_array);
	int is_unary;

	while (true)
	{
		is_unary = is_unop(*lex_array[token_counter(USE_CURRENT)]);
		if (is_unary < 0)
		{
			return L;
		}
		else
		{
			Lex_t *var = L.get();

			if (dynamic_cast<Ref_t*>(var))
			{
				var = &(dynamic_cast<Ref_t*>(var)->get_variable());
			}
			else if (var->get_kind() != Lex_kind_t::VAR)
			{
				throw_exception("You can not use unary operator without variable\n", token_counter(GET_CURRENT));	
			}

			unop = lex_array[token_counter(USE_CURRENT)];
			token_counter(INCREMENT);
			L = std::make_shared<UnOp>(L, *var, *unop);
		}
	}
}


std::shared_ptr<Lex_t> parse_T(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> T;

	if (is_brace(*lex_array[token_counter(USE_CURRENT)]) == Brace_t::LBRACE)
	{
		token_counter(INCREMENT);

		T = parse_asgn(lex_array);

		if (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::RBRACE)
		{
			throw_exception("Invalid input: check brases in arithmetic expression\n", token_counter(GET_CURRENT));
		}

		token_counter(INCREMENT);

		return T;
	}
	
	switch (lex_array[token_counter(USE_CURRENT)]->get_kind())
	{
		case Lex_kind_t::VALUE:
		{
			T = std::make_shared<Value>(*lex_array[token_counter(USE_CURRENT)], Value_type::NUMBER);
			token_counter(INCREMENT);
			break;
		}
		case Lex_kind_t::VAR:
		{
			T = std::make_shared<Variable>(*lex_array[token_counter(USE_CURRENT)]);
			token_counter(INCREMENT);

			if (is_brace(*lex_array[token_counter(USE_CURRENT)]) == Brace_t::LBRACE)
			{
				T = parse_function_call(lex_array);
			}
			break;
		}
		case Lex_kind_t::SYMBOL:
		{
			if (is_scan(*lex_array[token_counter(USE_CURRENT)]))
			{
				T = std::make_shared<Value>(*lex_array[token_counter(USE_CURRENT)], Value_type::INPUT);
				token_counter(INCREMENT);
				break;
			}
		}
		case Lex_kind_t::SCOPE:
		{
			if (is_scope(*lex_array[token_counter(USE_CURRENT)]) == Scope_t::LSCOPE)
			{
				T = parse_scope(lex_array);
				break;
			}
		}
		default:
		{
			throw_exception("Something strange instead of a value/variable/scope\n", token_counter(GET_CURRENT));
		}
	}
	
	return T;
}


//-----------------------------------------PARSE_FUNCTION_CALL---------------------------------------------


std::shared_ptr<Lex_t> parse_function_call(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Lex_t> T;
	Lex_t &func = *(lex_array[token_counter(USE_CURRENT) - 1]);
	std::string name = func.short_name();
	std::vector <std::shared_ptr<Lex_t>> args;

	std::shared_ptr<Statement> body = CURR_SCOPE->get_func_decl(name, token_counter(GET_CURRENT) - 1);
	
	if (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw_exception("I dont now how I jump in this function(parse_function_call)\n", token_counter(GET_CURRENT));
	}
	token_counter(INCREMENT);

	std::shared_ptr<Lex_t> arg;
	bool first_arg = 1;

	while (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::RBRACE)
	{
		if (first_arg)
		{
			first_arg = 0;
		}
		else
		{
			if (!is_comma(*lex_array[token_counter(USE_CURRENT)]))
			{
				throw_exception("Need comma between function args\n", token_counter(GET_CURRENT));
			}
			token_counter(INCREMENT);
		}
		arg = parse_arithmetic(lex_array);
		args.push_back(arg);
	}

	token_counter(INCREMENT);

	T = std::make_shared<Function>(body, args, func);

	return T;
}


//-----------------------------------------------ISERS-----------------------------------------------------


int is_plus_minus(Lex_t &node)
{
	if ((node.get_kind() != Lex_kind_t::BINOP)
	|| (node.get_data() > 1))
	{
		return -1;
	}
	return node.get_data();
}


int is_mul_div(Lex_t &node)
{
	if ((node.get_kind() != Lex_kind_t::BINOP)
	|| (node.get_data() < 2))
	{
		return -1;
	}
	return node.get_data();
}


int is_compop(Lex_t &node)
{
	if (node.get_kind() != Lex_kind_t::COMPOP)
	{
		return -1;
	}
	return node.get_data();
}


int is_brace(Lex_t &node)
{
	if (node.get_kind() != Lex_kind_t::BRACE)
	{
		return -1;
	}
	return node.get_data();
}


int is_unop(Lex_t &node)
{
	if (node.get_kind() != Lex_kind_t::UNOP)
	{
		return -1;
	}
	return node.get_data();
}


int is_scope(Lex_t &node)
{
	if (node.get_kind() != Lex_kind_t::SCOPE)
	{
		return -1;
	}
	return node.get_data();
}


bool is_scan(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::SYMBOL) 
		&& (node.get_data() == Symbols_t::SCAN);
}


bool is_negation(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::NEGATION);
}


bool is_binop(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::BINOP)
	   	|| (node.get_kind() == Lex_kind_t::COMPOP);
}


bool is_assign(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::STMT)
	  	&& (node.get_data() == Statements_t::ASSIGN);
}


bool is_semicol(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::SEMICOL);
}


bool is_return(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::STMT)
		&& (node.get_data() == Statements_t::RETURN);
}


bool is_comma(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::COMMA);
}


bool is_colon(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::COLON);
}


bool is_else(Lex_t &node)
{
	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::ELSE);
}


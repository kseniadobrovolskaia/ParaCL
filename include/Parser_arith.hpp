#ifndef PARSER_ARISH_H
#define PARSER_ARISH_H


#include "Lex_methods.hpp"
#include <unordered_map>


int program_size;


enum Move { INCREMENT, GET_CURRENT, USE_CURRENT, USE_NEXT, USE_NEXT_NEXT };

int token_counter(Move move);

Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array);
Lex_t *parse_unary(std::vector<Lex_t *> &lex_array);
Lex_t *parse_asgn(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
Lex_t *parse_E(std::vector<Lex_t *> &lex_array);
Lex_t *parse_M(std::vector<Lex_t *> &lex_array);
Lex_t *parse_T(std::vector<Lex_t *> &lex_array);

int is_plus_minus(Lex_t *node);
int is_semicol(Lex_t *node);
int is_mul_div(Lex_t *node);
int is_compop(Lex_t *node);
int is_assign(Lex_t *node);
int is_brace(Lex_t *node);
int is_binop(Lex_t *node);
int is_scope(Lex_t *node);
int is_scan(Lex_t *node);
int is_unop(Lex_t *node);


//-----------------------------------------TOKEN_COUNTER---------------------------------------------------


int token_counter(Move move) 
{
	static int curr_lex = 0;

	if (move == INCREMENT)
	{
		curr_lex++;
	}
	else if (move == GET_CURRENT)
	{
		return curr_lex;
	}
	else
	{
		if ((curr_lex + move - USE_CURRENT) >= program_size)
		{
			throw std::logic_error("Syntax error");
		}
	}
	
	return curr_lex;
}


//---------------------------------------------PARSERS-----------------------------------------------------


Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array)
{
	Lex_t *root = parse_asgn(lex_array);

	return root;
}


Lex_t *parse_asgn(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_bool(lex_array);
	Lex_t *R;
	int is_as;
	Assign_type type;
	
	while (1)
	{
		is_as = is_assign(lex_array[token_counter(USE_CURRENT)]);
		if (!is_as)
		{
			return L;
		}
		else
		{
			token_counter(INCREMENT);
			if (is_scan(lex_array[token_counter(USE_CURRENT)]))
			{
				type = INPUT;
				R = lex_array[token_counter(USE_CURRENT)];
				token_counter(INCREMENT);
			}
			else
			{
				type = ARITHMETIC;
				R = parse_bool(lex_array);	
			}
			
			L = new Assign_node(L, R, type);
		}
	}
}


Lex_t *parse_bool(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_E(lex_array);
	Lex_t *R;
	int is_comp;
	
	while (1)
	{
		is_comp = is_compop(lex_array[token_counter(USE_CURRENT)]);
		if (is_comp < 0)
		{
			return L;
		}
		else
		{
			token_counter(INCREMENT);
			R = parse_E(lex_array);	
			L = new CompOp(L, R, static_cast<CompOp_t>(is_comp));
		}
	}
}


Lex_t *parse_E(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_M(lex_array);
	Lex_t *R;
	int is_p_m;
	
	while (1)
	{
		is_p_m = is_plus_minus(lex_array[token_counter(USE_CURRENT)]);
		if (is_p_m < 0)
		{
			return L;
		}
		else
		{
			token_counter(INCREMENT);
			R = parse_M(lex_array);	
			L = new BinOp(L, R, static_cast<BinOp_t>(is_p_m));
		}
	}
}


Lex_t *parse_M(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_unary(lex_array);
	Lex_t *R;
	int is_m_d;

	while (1)
	{
		is_m_d = is_mul_div(lex_array[token_counter(USE_CURRENT)]);
		if (is_m_d < 0)
		{
			return L;
		}
		else
		{
			token_counter(INCREMENT);
			R = parse_unary(lex_array);
			L = new BinOp(L, R, static_cast<BinOp_t>(is_m_d));
		}
	}
}


Lex_t *parse_unary(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_T(lex_array);
	int is_unary;

	while (1)
	{
		is_unary = is_unop(lex_array[token_counter(USE_CURRENT)]);
		if (is_unary < 0)
		{
			return L;
		}
		else
		{
			token_counter(INCREMENT);
			L = new UnOp(L, static_cast<Statements_t>(is_unary));
		}
	}
}


Lex_t *parse_T(std::vector<Lex_t *> &lex_array)
{
	Lex_t *T;

	if (is_brace(lex_array[token_counter(USE_CURRENT)]) == Brace_t::LBRACE)
	{
		token_counter(INCREMENT);

		T = parse_asgn(lex_array);

		if (is_brace(lex_array[token_counter(USE_CURRENT)]) != Brace_t::RBRACE)
		{
			throw std::logic_error("Invalid input: check brases in arithmetic expression");
		}

		token_counter(INCREMENT);

		return T;
	}

	switch (lex_array[token_counter(USE_CURRENT)]->get_kind())
	{
		case Lex_kind_t::VALUE:
		{
			T = new Value(lex_array[token_counter(USE_CURRENT)]->get_data());
			break;
		}
		case Lex_kind_t::VAR:
		{
			int num_var = lex_array[token_counter(USE_CURRENT)]->get_data();
			T = new Variable(num_var);
			break;
		}
		default:
		{
			throw std::logic_error("Something strange instead of a value/variable");
			break;
		}
	}
	
	token_counter(INCREMENT);
	return T;
}


//-----------------------------------------------ISERS-----------------------------------------------------


int is_plus_minus(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::BINOP)
	{
		return -1;
	}
	if (node->get_data() == BinOp_t::ADD)
	{
		return BinOp_t::ADD;
	}
	if (node->get_data() == BinOp_t::SUB)
	{
		return BinOp_t::SUB;
	}
	return -1;
}


int is_mul_div(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::BINOP)
	{
		return -1;
	}
	if (node->get_data() == BinOp_t::MULT)
	{
		return BinOp_t::MULT;
	}
	if (node->get_data() == BinOp_t::DIV)
	{
		return BinOp_t::DIV;
	}
	return -1;
}


int is_scan(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::SYMBOL)
	{
		return 0;
	}
	if (node->get_data() != Symbols_t::SCAN)
	{
		return 0;
	}
	return 1;
}


int is_compop(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::COMPOP)
	{
		return -1;
	}
	return node->get_data();
}


int is_brace(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::BRACE)
	{
		return -1;
	}
	return node->get_data();
}


int is_unop(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::UNOP)
	{
		return -1;
	}
	return node->get_data();
}


int is_binop(Lex_t *node)
{
	if (node->get_kind() == Lex_kind_t::BINOP)
	{
		return 1;
	}
	if (node->get_kind() == Lex_kind_t::COMPOP)
	{
		return 1;
	}
	return 0;
}


int is_assign(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::STMT)
	{
		return 0;
	}
	if (node->get_data() != Statements_t::ASSIGN)
	{
		return 0;
	}
	return 1;
}


int is_scope(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::SCOPE)
	{
		return -1;
	}
	return node->get_data();
}


int is_semicol(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::SYMBOL)
	{
		return 0;
	}
	if (node->get_data() != Symbols_t::SEMICOL)
	{
		return 0;
	}
	return 1;
}




#endif

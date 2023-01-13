#ifndef PARSER_ARISH_H
#define PARSER_ARISH_H


#include "Lexer.hpp"
#include <unordered_map>


std::unordered_map<std::string, int>VARS; //VARS["elem"] = 5; VARS["elem"]++;


//----------------------------------------------------------------------------------------------------------

class Value : public Lex_t {

public:
	Value(int num) : Lex_t(Lex_kind_t::VALUE, num){};
};

//----------------------------------------------------------------------------------------------------------

class Variable : public Lex_t {

	std::string name_;
	
public:
 	Variable(Lex_t *parent, std::string name, int num_var) : Lex_t(Lex_kind_t::VAR, num_var), name_(name) {};
 	int get_num_var() const { return data_; };
 	std::string get_name() const { return name_; };
};


//----------------------------------------------------------------------------------------------------------


class BinOp : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	BinOp() = default;
  	BinOp(Lex_t *lhs, Lex_t *rhs, BinOp_t opcode) : Lex_t(Lex_kind_t::BINOP, opcode), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	CompOp() = default;
  	CompOp(Lex_t *lhs, Lex_t *rhs, CompOp_t opcode) : Lex_t(Lex_kind_t::COMPOP, opcode), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t {

  	Lex_t *rhs_;

public:
	UnOp(Lex_t *parent, UnOp_t opcode) : Lex_t(Lex_kind_t::UNOP, opcode) {};
};


//----------------------------------------------------------------------------------------------------------



Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
Lex_t *parse_E(std::vector<Lex_t *> &lex_array);
Lex_t *parse_M(std::vector<Lex_t *> &lex_array);
Lex_t *parse_T(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
int is_plus_minus(Lex_t *node);
int is_mul_div(Lex_t *node);
int is_brace(Lex_t *node);


enum Move { INCREMENT, GET_CURRENT };

int token_counter(Move move) 
{
	static int curr_lex = 0;

	if (move == Move::INCREMENT)
	{
		curr_lex++;
	}

	return curr_lex;
}


void check_brases(std::vector<Lex_t *> &lex_array)
{
	int brases = 0;
	int size = lex_array.size();

	for (int lex = 0; lex < size; lex++)
	{
		if ((lex_array)[lex]->get_kind() == Lex_kind_t::BRACE)
		{
			if ((lex_array)[lex]->get_data() == Brace_t::LBRACE)
			{
				brases++;
			}
			else
			{
				brases--;
			}
		}
	}

	if (brases != 0)
	{
		throw std::logic_error("Invalid input: check brases in arithmetic expression");
	}
}


Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array)
{
	Lex_t *root = parse_bool(lex_array);

	return root;
}

Lex_t *parse_bool(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_E(lex_array);
	Lex_t *R;
	int is_comp;
	int size = lex_array.size();

	if (token_counter(Move::GET_CURRENT) >= size)
	{
		return L;
	}
	
	while (1)
	{
		if (token_counter(Move::GET_CURRENT) >= size)
		{
			return L;
		}
		
		is_comp = (lex_array[token_counter(Move::GET_CURRENT)]->get_kind() == Lex_kind_t::COMPOP);
		if (is_comp == 0)
		{
			return L;
		}
		else
		{
			is_comp = lex_array[token_counter(Move::GET_CURRENT)]->get_data();
			token_counter(Move::INCREMENT);
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
	int size = lex_array.size();

	if (token_counter(Move::GET_CURRENT) >= size)
	{
		return L;
	}
	
	while (1)
	{
		if (token_counter(Move::GET_CURRENT) >= size)
		{
			return L;
		}
		
		is_p_m = is_plus_minus(lex_array[token_counter(Move::GET_CURRENT)]);
		if (is_p_m < 0)
		{
			return L;
		}
		else
		{
			token_counter(Move::INCREMENT);
			R = parse_M(lex_array);	
			L = new BinOp(L, R, static_cast<BinOp_t>(is_p_m));
		}
	}
}


Lex_t *parse_M(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_T(lex_array);
	Lex_t *R;
	int is_m_d;
	int size = lex_array.size();

	if (token_counter(Move::GET_CURRENT) >= size)
	{
		return L;
	}

	while (1)
	{
		if (token_counter(Move::GET_CURRENT) >= size)
		{
			return L;
		}

		is_m_d = is_mul_div(lex_array[token_counter(Move::GET_CURRENT)]);
		if (is_m_d < 0)
		{
			return L;
		}
		else
		{
			token_counter(Move::INCREMENT);
			R = parse_T(lex_array);
			L = new BinOp(L, R, static_cast<BinOp_t>(is_m_d));
		}
	}
}


Lex_t *parse_T(std::vector<Lex_t *> &lex_array)
{
	Lex_t *T;

	if (is_brace(lex_array[token_counter(Move::GET_CURRENT)]) == Brace_t::LBRACE)
	{
		token_counter(Move::INCREMENT);
		T = parse_bool(lex_array);
		if (is_brace(lex_array[token_counter(Move::GET_CURRENT)]) != Brace_t::RBRACE)
		{
			throw std::logic_error("Invalid input: check brases in arithmetic expression");
		}
		token_counter(Move::INCREMENT);
		return T;
	}
	int val = (lex_array[token_counter(Move::GET_CURRENT)]->get_kind() == Lex_kind_t::VALUE);
	if (!val)
	{
		throw std::logic_error("Invalid input");
	}

	T = new Value(lex_array[token_counter(Move::GET_CURRENT)]->get_data());
	token_counter(Move::INCREMENT);
	return T;
}


int calculate(Lex_t *tree)
{
	int left, right;
	if (tree->get_kind() == Lex_kind_t::VALUE)
	{
		return tree->get_data();
	}
	else
	{
		if (tree->get_kind() == Lex_kind_t::BINOP)
		{
			left = calculate((static_cast<BinOp*>(tree))->get_lhs());
			right = calculate((static_cast<BinOp*>(tree))->get_rhs());
			switch (tree->get_data())
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
						throw std::runtime_error("Error: division by zero");
					}
					return left / right;
			}
		}
		else
		{	
			left = calculate((static_cast<CompOp*>(tree))->get_lhs());
			right = calculate((static_cast<CompOp*>(tree))->get_rhs());
			switch (tree->get_data())
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
			}
		}
	}
	throw std::runtime_error("Error: it is not clear what is in function \"Calculate_arithmetic\"");

}


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


int is_brace(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::BRACE)
	{
		return -1;
	}
	
	return node->get_data();
}


#endif
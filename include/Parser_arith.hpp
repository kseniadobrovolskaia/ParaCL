#ifndef PARSER_ARISH_H
#define PARSER_ARISH_H


#include "Lexer.hpp"
#include <unordered_map>


int program_size;


class Value : public Lex_t {

public:
	Value(int num) : Lex_t(Lex_kind_t::VALUE, num){};
};


//----------------------------------------------------------------------------------------------------------


class Variable : public Lex_t {
	
public:
 	Variable(int num_var) : Lex_t(Lex_kind_t::VAR, num_var){};
};


//----------------------------------------------------------------------------------------------------------

class Statement;

class Scope : public Lex_t {

	std::vector<Statement*> lhs_;

public:
	Scope(std::vector <Statement*> lhs) : Lex_t(Lex_kind_t::SCOPE, Scope_t::LSCOPE), lhs_(lhs){};
	std::vector<Statement*> get_lhs() const { return lhs_; };
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

  	Variable *var_;

public:
	UnOp() = default;
  	UnOp(Variable *var, UnOp_t opcode) : Lex_t(Lex_kind_t::UNOP, opcode), var_(var){};
  	Lex_t *get_var() const { return var_; };
};


//----------------------------------------------------------------------------------------------------------


enum Move { INCREMENT, GET_CURRENT };


int token_counter(Move move);
int calculate(Lex_t *tree);
void check_brases(std::vector<Lex_t *> &lex_array);
Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
Lex_t *parse_E(std::vector<Lex_t *> &lex_array);
Lex_t *parse_M(std::vector<Lex_t *> &lex_array);
Lex_t *parse_T(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
int is_val_var(Lex_t *node, int *value);
int is_plus_minus(Lex_t *node);
int is_mul_div(Lex_t *node);
int is_compop(Lex_t *node);
int is_brace(Lex_t *node);
int is_unop(Lex_t *node);



int token_counter(Move move) 
{
	static int curr_lex = 0;

	if (move == INCREMENT)
	{
		curr_lex++;
		if (curr_lex > program_size)
		{
			throw std::logic_error("Syntax error");
		}
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

	if (token_counter(GET_CURRENT) >= program_size)
	{
		throw std::logic_error("Syntax error");
	}

	return root;
}


Lex_t *parse_bool(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_E(lex_array);
	Lex_t *R;
	int is_comp;
	int size = lex_array.size();

	if (token_counter(GET_CURRENT) >= size)
	{
		return L;
	}
	
	while (1)
	{
		if (token_counter(GET_CURRENT) >= size)
		{
			return L;
		}
		
		is_comp = is_compop(lex_array[token_counter(GET_CURRENT)]);
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
	int size = lex_array.size();

	if (token_counter(GET_CURRENT) >= size)
	{
		return L;
	}
	
	while (1)
	{
		if (token_counter(GET_CURRENT) >= size)
		{
			return L;
		}
		
		is_p_m = is_plus_minus(lex_array[token_counter(GET_CURRENT)]);
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
	Lex_t *L = parse_T(lex_array);
	Lex_t *R;
	int is_m_d;
	int size = lex_array.size();

	if (token_counter(GET_CURRENT) >= size)
	{
		return L;
	}

	while (1)
	{
		if (token_counter(GET_CURRENT) >= size)
		{
			return L;
		}

		is_m_d = is_mul_div(lex_array[token_counter(GET_CURRENT)]);
		if (is_m_d < 0)
		{
			return L;
		}
		else
		{
			token_counter(INCREMENT);
			R = parse_T(lex_array);
			L = new BinOp(L, R, static_cast<BinOp_t>(is_m_d));
		}
	}
}


Lex_t *parse_T(std::vector<Lex_t *> &lex_array)
{
	Lex_t *T;

	if (token_counter(GET_CURRENT) >= program_size)
	{
		throw std::logic_error("Syntax error");
	}

	if (is_brace(lex_array[token_counter(GET_CURRENT)]) == Brace_t::LBRACE)
	{
		if (token_counter(INCREMENT) >= program_size)
		{
			throw std::logic_error("Syntax error");
		}

		T = parse_bool(lex_array);

		if (is_brace(lex_array[token_counter(GET_CURRENT)]) != Brace_t::RBRACE)
		{
			throw std::logic_error("Invalid input: check brases in arithmetic expression");
		}

		if (token_counter(INCREMENT) >= program_size)
		{
			throw std::logic_error("Syntax error");
		}

		return T;
	}

	int value;
	int is_v_v = is_val_var(lex_array[token_counter(GET_CURRENT)], &value);

	if (is_v_v < 0)
	{
		throw std::logic_error("Invalid input");
	}

	if (token_counter(GET_CURRENT) + 1 >= program_size)
	{
		throw std::logic_error("Syntax error");
	}

	int is_un = is_unop(lex_array[token_counter(GET_CURRENT) + 1]);

	if (is_v_v == Lex_kind_t::VALUE)
	{
		if (is_un >= 0)
		{
			throw std::logic_error("Unop can use only with variables");
		}
		T = new Value(lex_array[token_counter(GET_CURRENT)]->get_data());
	}
	else if (is_v_v == Lex_kind_t::VAR)
	{
		int num_var = lex_array[token_counter(GET_CURRENT)]->get_data();
		T = new Variable(num_var);
		if (is_un >= 0)
		{
			T = new UnOp(static_cast<Variable*>(T), static_cast<UnOp_t>(is_un));
			token_counter(INCREMENT);
		}
	}
	
	if (token_counter(INCREMENT) >= program_size)
	{
		throw std::logic_error("Syntax error");
	}
	return T;
}


int calculate(Lex_t *tree)
{
	int left, right;
	if (tree->get_kind() == Lex_kind_t::VALUE)
	{
		return tree->get_data();
	}
	else if (tree->get_kind() == Lex_kind_t::VAR)
	{
		std::string name_var = vars[tree->get_data()];
		if (!(VARS.contains(name_var)))
		{
		 	throw std::runtime_error("Uninitialized variable");
		}
		return VARS[name_var];
	}
	else if (tree->get_kind() == Lex_kind_t::UNOP)
	{
		std::string name_var = vars[((static_cast<UnOp*>(tree))->get_var())->get_data()];
		if (!(VARS.contains(name_var)))
		{
		 	throw std::runtime_error("Uninitialized variable");
		}
		switch (tree->get_data())
		{
			case UnOp_t::INC:
				VARS[name_var]++;
				return VARS[name_var];
			case UnOp_t::DEC:
				VARS[name_var]--;
				return VARS[name_var];
		}

	}
	else if (tree->get_kind() == Lex_kind_t::BINOP)
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
	else if (tree->get_kind() == Lex_kind_t::COMPOP)
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


int is_val_var(Lex_t *node, int *value)
{
	if (node->get_kind() == Lex_kind_t::VALUE)
	{	
		*value = node->get_data();
		return Lex_kind_t::VALUE;
	}
	if (node->get_kind() == Lex_kind_t::VAR)
	{
		*value = VARS[vars[node->get_data()]];
		return Lex_kind_t::VAR;
	}
	return -1;
}


#endif

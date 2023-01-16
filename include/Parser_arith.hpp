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
 	virtual int calculate() override;
};


//----------------------------------------------------------------------------------------------------------

enum Assign_type { INPUT, ARITHMETIC };

class Assign_node : public Lex_t {

  	Lex_t *lhs_, *rhs_;
  	Assign_type type_;

public:
	Assign_node() = default;
  	Assign_node(Lex_t *lhs, Lex_t *rhs, Assign_type type) : Lex_t(Lex_kind_t::KEYWD, Keywords_t::ASSIGN), lhs_(lhs), rhs_(rhs), type_(type) {};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	Assign_type get_type() const { return type_; };
  	virtual int calculate() override;
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
  	virtual int calculate() override;
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	CompOp() = default;
  	CompOp(Lex_t *lhs, Lex_t *rhs, CompOp_t opcode) : Lex_t(Lex_kind_t::COMPOP, opcode), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	virtual int calculate() override;
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t {

  	Variable *var_;

public:
	UnOp() = default;
  	UnOp(Variable *var, UnOp_t opcode) : Lex_t(Lex_kind_t::UNOP, opcode), var_(var){};
  	Lex_t *get_var() const { return var_; };
  	virtual int calculate() override;
};


//----------------------------------------------------------------------------------------------------------


enum Move { INCREMENT, GET_CURRENT };


int token_counter(Move move);
//int calculate(Lex_t *tree);
void check_brases(std::vector<Lex_t *> &lex_array);
Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
Lex_t *parse_E(std::vector<Lex_t *> &lex_array);
Lex_t *parse_M(std::vector<Lex_t *> &lex_array);
Lex_t *parse_T(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
int is_val_var(Lex_t *node, int *value, std::vector<Lex_t *> &lex_array);
int is_plus_minus(Lex_t *node);
int is_mul_div(Lex_t *node);
int is_assign(Lex_t *node);
int is_compop(Lex_t *node);
int is_brace(Lex_t *node);
int is_scan(Lex_t *node);
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
	
	while (1)
	{
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
	
	while (1)
	{
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

	while (1)
	{
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
			std::cout << "Попались здесь во время " << lex_array[token_counter(GET_CURRENT)]->name() << std::endl;
			std::cout << "И после " << lex_array[token_counter(GET_CURRENT) - 1]->name() << std::endl;
			
			throw std::logic_error("Invalid input: check brases in arithmetic expression");
		}

		if (token_counter(INCREMENT) >= program_size)
		{
			throw std::logic_error("Syntax error");
		}

		return T;
	}

	if ((token_counter(GET_CURRENT) + 1) >= program_size)
	{
		throw std::logic_error("Syntax error");
	}

	int value;
	int is_v_v = is_val_var(lex_array[token_counter(GET_CURRENT)], &value, lex_array);

	if (is_v_v < 0)
	{
		throw std::logic_error("Invalid input");
	}

	int is_un = is_unop(lex_array[token_counter(GET_CURRENT) + 1]);
	int is_as = is_assign(lex_array[token_counter(GET_CURRENT) + 1]);

	if (is_v_v == Lex_kind_t::VALUE)
	{
		if (is_un >= 0)
		{
			throw std::logic_error("Unop can use only with variables");
		}
		if (is_as)
		{
			throw std::logic_error("Assign can use only with variables");
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
			if (is_un == UnOp_t::INC)
			{
				VARS[vars[num_var]]++;
			}
			else
			{
				VARS[vars[num_var]]--;
			}
			token_counter(INCREMENT);
		}
		if (is_as)
		{
			if ((token_counter(GET_CURRENT) + 1) >= program_size)
			{
				throw std::logic_error("Syntax error");
			}
			token_counter(INCREMENT);
			token_counter(INCREMENT);
			if (is_scan(lex_array[token_counter(GET_CURRENT)]))
			{
				T = new Assign_node(T, lex_array[token_counter(GET_CURRENT)], Assign_type::INPUT);
			}
			else
			{
				T = new Assign_node(T, parse_arithmetic(lex_array), Assign_type::ARITHMETIC);
				return T;
			}
		}
	}
	
	token_counter(INCREMENT);
	return T;
}


int Variable::calculate()
{
	std::string var_name = vars[this->get_data()];

	if (!(VARS.contains(var_name)))
	{
	 	throw std::runtime_error("Uninitialized variable");
	}

	return VARS[var_name];
}


int Assign_node::calculate()
{
	lhs_->calculate();

	std::string var_name = vars[lhs_->get_data()];

	if (type_ == Assign_type::INPUT)
	{
		std::cin >> std::ws;
		std::cin >> VARS[var_name];
	}
	else
	{
		VARS[var_name] = rhs_->calculate();
	}

	return VARS[var_name];
}


int BinOp::calculate()
{
	int left = lhs_->calculate();
	int right = rhs_->calculate();

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
				throw std::runtime_error("Error: division by zero");
			}
			return left / right;
	}
	throw std::logic_error("Error: it is not clear what is in function \"BinOp::calculate\"");
}


int UnOp::calculate()
{
	std::string var_name = vars[var_->get_data()];

	if (!(VARS.contains(var_name)))
	{
	 	throw std::logic_error("Uninitialized variable");
	}

	switch (this->get_data())
	{
		case UnOp_t::INC:
			VARS[var_name]++;
			return VARS[var_name];
		case UnOp_t::DEC:
			VARS[var_name]--;
			return VARS[var_name];
	}
	throw std::logic_error("Error: it is not clear what is in function \"UnOp::calculate\"");
}


int CompOp::calculate()
{
	int left = lhs_->calculate();
	int right = rhs_->calculate();

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
	throw std::logic_error("Error: it is not clear what is in function \"CompOp::calculate\"");
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


int is_scan(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::KEYWD)
	{
		return 0;
	}
	if (node->get_data() != Keywords_t::SCAN)
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


int is_assign(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::KEYWD)
	{
		return 0;
	}
	if (node->get_data() != Keywords_t::ASSIGN)
	{
		return 0;
	}
	return 1;
}


int is_val_var(Lex_t *node, int *value, std::vector<Lex_t *> &lex_array)
{
	if (node->get_kind() == Lex_kind_t::VALUE)
	{	
		*value = node->get_data();
		return Lex_kind_t::VALUE;
	}
	if (node->get_kind() == Lex_kind_t::VAR)
	{
		if (!VARS.contains(vars[node->get_data()]) && !is_assign(lex_array[token_counter(GET_CURRENT) + 1]))
		{
			throw std::logic_error("Uninitialized variable");
		}
		*value = VARS[vars[node->get_data()]];
		return Lex_kind_t::VAR;
	}
	return -1;
}




#endif

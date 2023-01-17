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
  	Assign_node(Lex_t *lhs, Lex_t *rhs, Assign_type type) : Lex_t(Lex_kind_t::STMT, Statements_t::ASSIGN), lhs_(lhs), rhs_(rhs), type_(type) {};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	virtual Lex_t *get_var() const override { return lhs_; };
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

  	Lex_t *var_;

public:
	UnOp() = default;
  	UnOp(Lex_t *var, Statements_t opcode) : Lex_t(Lex_kind_t::UNOP, opcode), var_(var){};
  	virtual Lex_t *get_var() const override { return var_; };
  	virtual int calculate() override;
};


//----------------------------------------------------------------------------------------------------------


enum Move { INCREMENT, GET_CURRENT, USE_CURRENT, USE_NEXT, USE_NEXT_NEXT };


int token_counter(Move move);
void check_brases(std::vector<Lex_t *> &lex_array);
Lex_t *parse_arithmetic(std::vector<Lex_t *> &lex_array);
Lex_t *parse_asgn(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
Lex_t *parse_E(std::vector<Lex_t *> &lex_array);
Lex_t *parse_M(std::vector<Lex_t *> &lex_array);
Lex_t *parse_T(std::vector<Lex_t *> &lex_array);
Lex_t *parse_bool(std::vector<Lex_t *> &lex_array);
int is_assign(Lex_t *node);
int is_plus_minus(Lex_t *node);
int is_mul_div(Lex_t *node);
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
	Lex_t *L = parse_asgn(lex_array);
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
			R = parse_asgn(lex_array);	
			L = new CompOp(L, R, static_cast<CompOp_t>(is_comp));
		}
	}
}


Lex_t *parse_asgn(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = parse_E(lex_array);
	Lex_t *R;
	int is_as;
	Assign_type type;
	
	while (1)
	{
		is_as = is_assign(lex_array[token_counter(GET_CURRENT)]);
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
				R = parse_E(lex_array);	
			}
			
			L = new Assign_node(L, R, type);
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

	if (is_brace(lex_array[token_counter(USE_CURRENT)]) == Brace_t::LBRACE)
	{
		token_counter(INCREMENT);

		T = parse_bool(lex_array);

		if (is_brace(lex_array[token_counter(USE_CURRENT)]) != Brace_t::RBRACE)
		{
			throw std::logic_error("Invalid input: check brases in arithmetic expression");
		}

		token_counter(INCREMENT);

		int is_unary = is_unop(lex_array[token_counter(USE_CURRENT)]);

		if (is_unary >= 0)
		{
			if (dynamic_cast<Variable*>(T) || dynamic_cast<Assign_node*>(T) || dynamic_cast<UnOp*>(T))
			{
				token_counter(INCREMENT);
				return new UnOp(T, static_cast<Statements_t>(is_unary));
			}
			else
			{
				throw std::logic_error("Unary operator can use only with variables(or assignment or unary operator)");
			}

			Lex_t *var = T, *tmpvar;

			int flag = 1;
			while (flag)
			{
				if ((tmpvar = dynamic_cast<Variable*>(var)))
				{
					flag = 0;
				}
				else
				{
					var = var->get_var();
				}
			}

			std::string var_name = vars[var->get_data()];

			if (!VARS.contains(var_name))
			{
				throw std::logic_error("Uninitialized variable");
			}
		}

		return T;
	}

	int is_unary = is_unop(lex_array[token_counter(USE_NEXT) + 1]);
	int is_assignment;
	if (is_unary < 0)
	{
		is_assignment = is_assign(lex_array[token_counter(USE_NEXT) + 1]);
	}
	else
	{
		is_assignment = is_assign(lex_array[token_counter(USE_NEXT_NEXT) + 2]);
	}
	
	switch (lex_array[token_counter(USE_CURRENT)]->get_kind())
	{
		case Lex_kind_t::VALUE:
		{
			if (is_assignment)
			{
				throw std::logic_error("Assignment can use only with variables");
			}

			T = new Value(lex_array[token_counter(USE_CURRENT)]->get_data());
			break;
		}
		case Lex_kind_t::VAR:
		{
			int num_var = lex_array[token_counter(USE_CURRENT)]->get_data();
			T = new Variable(num_var);

			if (is_unary >= 0)
			{
				if (!VARS.contains(vars[num_var]))
				{
					throw std::logic_error("Uninitialized variable");
				}
				T = new UnOp(T, static_cast<Statements_t>(is_unary));
				token_counter(INCREMENT);
			}
			if (is_assignment)
			{
				VARS[vars[num_var]] = 0;
				token_counter(INCREMENT);
				token_counter(INCREMENT);
				if (is_scan(lex_array[token_counter(USE_CURRENT)]))
				{
					T = new Assign_node(T, lex_array[token_counter(USE_CURRENT)], Assign_type::INPUT);
				}
				else
				{
					T = new Assign_node(T, parse_arithmetic(lex_array), Assign_type::ARITHMETIC);
					return T;
				}
			}
			break;
		}
		default:
		{
			throw std::logic_error("Something strange instead of a variable");
			break;
		}
	}
	
	token_counter(INCREMENT);
	return T;
}


int Variable::calculate()
{
	std::string var_name = vars[this->get_data()];

	return VARS[var_name];
}


int Assign_node::calculate()
{
	lhs_->calculate();
	int flag = 1;
	Lex_t *var = lhs_, *tmpvar;
	std::string var_name;

	while (flag)
	{
		if ((tmpvar = dynamic_cast<Variable*>(var)))
		{
			var_name = vars[var->get_data()];
			flag = 0;
		}
		else
		{
			var = var->get_var();
		}
	}

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
				throw std::runtime_error("Division by zero");
			}
			return left / right;
	}
	throw std::logic_error("Error: it is not clear what is in function \"BinOp::calculate\"");
}


int UnOp::calculate()
{
	var_->calculate();
	Lex_t *var = var_, *tmpvar;

	int flag = 1;
	std::string var_name;
	
	while (flag)
	{
		if ((tmpvar = dynamic_cast<Variable*>(var)))
		{
			var_name = vars[var->get_data()];
			flag = 0;
		}
		else
		{
			var = static_cast<UnOp*>(var)->get_var();
		}
	}

	switch (this->get_data())
	{
		case Statements_t::INC:
			VARS[var_name] += 1;
			return VARS[var_name];
		case Statements_t::DEC:
			VARS[var_name] -= 1;
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





#endif

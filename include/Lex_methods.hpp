#ifndef LEX_METHODS_H
#define LEX_METHODS_H


#include "Lexer.hpp"


//--------------------------------------------LEXEME_CLASSES------------------------------------------------


class Value : public Lex_t {

public:
	Value(int num) : Lex_t(Lex_kind_t::VALUE, num){};
};


//----------------------------------------------------------------------------------------------------------


class Variable : public Lex_t {
	
public:
 	Variable(int num_var) : Lex_t(Lex_kind_t::VAR, num_var){};
 	virtual int calculate(std::istream & istr) override;
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
  	virtual int calculate(std::istream & istr) override;
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
  	virtual int calculate(std::istream & istr) override;
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	CompOp() = default;
  	CompOp(Lex_t *lhs, Lex_t *rhs, CompOp_t opcode) : Lex_t(Lex_kind_t::COMPOP, opcode), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	virtual int calculate(std::istream & istr) override;
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t {

  	Lex_t *var_;

public:
	UnOp() = default;
  	UnOp(Lex_t *var, Statements_t opcode) : Lex_t(Lex_kind_t::UNOP, opcode), var_(var){};
  	virtual Lex_t *get_var() const override { return var_; };
  	virtual int calculate(std::istream & istr) override;
};


//---------------------------------------------CALCULATE----------------------------------------------------


int Variable::calculate(std::istream & istr)
{
	std::string var_name = vars[this->get_data()];

	if (!VARS.contains(var_name))
	{
		throw std::logic_error("Uninitialized variable");
	}

	return VARS[var_name];
}


int Assign_node::calculate(std::istream & istr)
{
	Lex_t *var = lhs_;
	std::string var_name;

	if (dynamic_cast<Variable*>(var))
	{
		var_name = vars[var->get_data()];
		if (!VARS.contains(var_name))
		{
			VARS[var_name] = 0;
		}
	}
	else
	{
		while (!dynamic_cast<Variable*>(var))
		{
			var = var->get_var();	
		}
		var_name = vars[var->get_data()];
	}

	lhs_->calculate(istr);

	if (type_ == Assign_type::INPUT)
	{		
		istr >> std::ws;
		istr >> VARS[var_name];
		
		return VARS[var_name];
	}

	VARS[var_name] = rhs_->calculate(istr);

	return VARS[var_name];
}


int BinOp::calculate(std::istream & istr)
{
	int left = lhs_->calculate(istr);
	int right = rhs_->calculate(istr);

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


int UnOp::calculate(std::istream & istr)
{
	var_->calculate(istr);
	Lex_t *var = var_;

	std::string var_name;
	
	while (!dynamic_cast<Variable*>(var))
	{
		var = var->get_var();
	}
	
	var_name = vars[var->get_data()];

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


int CompOp::calculate(std::istream & istr)
{
	int left = lhs_->calculate(istr);
	int right = rhs_->calculate(istr);

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



#endif


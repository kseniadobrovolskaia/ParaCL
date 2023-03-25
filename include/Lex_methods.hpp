#ifndef LEX_METHODS_H
#define LEX_METHODS_H


#include "Throw_exception.hpp"


//--------------------------------------------LEXEME_CLASSES------------------------------------------------

enum Value_type { INPUT, NUMBER };


class Value : public Lex_t {

	Value_type type_;

public:
	Value(const Lex_t &val, Value_type type) : Lex_t(val), type_(type){};
	Value_type get_type() const { return type_; }
	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Variable : public Lex_t {
	
public:
 	Variable(const Lex_t &var) : Lex_t(var){};
 	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Negation : public Lex_t {

	Lex_t *rhs_;
	
public:
 	Negation(Lex_t *rhs, const Lex_t &neg) : Lex_t(neg), rhs_(rhs){};
 	Lex_t *get_rhs() const { return rhs_; };
 	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
 	virtual Lex_t *get_var() const override { return rhs_; };
};


//----------------------------------------------------------------------------------------------------------


class Assign_node : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	Assign_node() = default;
  	Assign_node(Lex_t *lhs, Lex_t *rhs, const Lex_t &ass) : Lex_t(ass), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	virtual Lex_t *get_var() const override { return lhs_; };
  	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//--------------------------------------------ABSTRACT_STATEMENT_CLASS-----------------------


//class Statement;
class Statement{

	Statements_t kind_;

public:
	Statement(Statements_t kind) : kind_(kind) {};
	virtual ~Statement() = default;

	Statements_t get_kind() const { return kind_; };
	virtual Lex_t *get_lhs() const = 0;

	virtual std::string name() const = 0;
	virtual int run_stmt(std::istream & istr, std::ostream & ostr) = 0;
};


//----------------------------------------------------------------------------------------------------------


class Scope : public Lex_t {

	std::vector<Statement*> stmts_;

public:
	Scope(std::vector <Statement*> lhs, const Lex_t &scope) : Lex_t(scope), stmts_(lhs){};
	std::vector<Statement*> get_lhs() const { return stmts_; };
	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Function : public Lex_t {

	Lex_t *scope_;
	std::vector<Variable*> args_;

public:
	Function(const Lex_t &var, std::vector <Variable*> args, Lex_t *scope) : Lex_t(var), scope_(scope), args_(args){};
	Lex_t *get_rhs() const { return scope_; };
	std::vector<Variable*> get_args() const { return args_; };
	void print_args() const { for (int i = 0; i < static_cast<int>(args_.size()); i++)
								{std::cout << args_[i]->short_name() << " ";} std::cout << std::endl;}
};


//----------------------------------------------------------------------------------------------------------


class BinOp : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	BinOp() = default;
  	BinOp(Lex_t *lhs, Lex_t *rhs, const Lex_t &binop) : Lex_t(binop), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	Lex_t *lhs_, *rhs_;

public:
	CompOp() = default;
  	CompOp(Lex_t *lhs, Lex_t *rhs, const Lex_t &compop) : Lex_t(compop), lhs_(lhs), rhs_(rhs){};
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t {

  	Lex_t *var_;

public:
	UnOp() = default;
  	UnOp(Lex_t *var, const Lex_t &unop) : Lex_t(unop), var_(var){};
  	virtual Lex_t *get_var() const override { return var_; };
  	virtual int calculate(std::istream & istr, std::ostream & ostr) override;
};


//---------------------------------------------CALCULATE----------------------------------------------------


int Value::calculate(std::istream & istr, std::ostream & ostr)
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


int Variable::calculate(std::istream & istr, std::ostream & ostr)
{
	std::string var_name = vars[this->get_data()];

	if (!VARS.contains(var_name))
	{
		throw_exception("Uninitialized variable\n", this->get_num());
	}

	return VARS[var_name];
}


int Negation::calculate(std::istream & istr, std::ostream & ostr)
{
	int right = rhs_->calculate(istr, ostr);

	return !right;
}


int Assign_node::calculate(std::istream & istr, std::ostream & ostr)
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
	
	lhs_->calculate(istr, ostr);

	VARS[var_name] = rhs_->calculate(istr, ostr);

	return VARS[var_name];
}


int BinOp::calculate(std::istream & istr, std::ostream & ostr)
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


int UnOp::calculate(std::istream & istr, std::ostream & ostr)
{
	var_->calculate(istr, ostr);
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

	throw_exception("Error: it is not clear what is in function \"UnOp::calculate\"\n", this->get_num());
	return 1;
}


int CompOp::calculate(std::istream & istr, std::ostream & ostr)
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


int Scope::calculate(std::istream & istr, std::ostream & ostr)
{
	int res;
	int size_prog = stmts_.size();
	
	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		res = stmts_[prog_elem]->run_stmt(istr, ostr);
	}

	return res;
}



#endif


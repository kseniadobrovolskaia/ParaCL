#ifndef STMT_METHODS_H
#define STMT_METHODS_H


#include "Parser_arith.hpp"



//------------------------------------ABSTRACT_STATEMENT_CLASS---------------------------------------------------
#if 0

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
#endif

//---------------------------------------STATEMENT_CLASSES-------------------------------------------------


class If final : public Statement {

	Lex_t *lhs_, *rhs_;
	Lex_t *else_ = nullptr;

public:
	If(Lex_t *lhs, Lex_t *rhs, Lex_t * Else) : 
	Statement(Statements_t::IF), lhs_(lhs), rhs_(rhs), else_(Else){};
	virtual ~If() = default;

	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	Lex_t *get_else() const { return else_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class While final : public Statement {

	Lex_t *lhs_, *rhs_;

public:
	While(Lex_t *lhs, Lex_t *rhs) :
	Statement(Statements_t::WHILE), lhs_(lhs), rhs_(rhs){};
	virtual ~While() = default;
	
	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Print final : public Statement {

	Lex_t *lhs_;

public:
	Print(Lex_t *lhs) :
	Statement(Statements_t::PRINT), lhs_(lhs) {};
	virtual ~Print() = default;
	
	virtual Lex_t *get_lhs() const override { return lhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Arithmetic final : public Statement {

	Lex_t *lhs_;

public:
	Arithmetic(Lex_t *lhs) :
	Statement(Statements_t::ARITHMETIC), lhs_(lhs) {};
	virtual ~Arithmetic() = default;
	
	virtual Lex_t *get_lhs() const override { return lhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//--------------------------------------------RUN_STATEMENTS------------------------------------------------


int If::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int res;
	int condition = lhs_->calculate(istr, ostr);

	if (condition)
	{
		res = rhs_->calculate(istr, ostr);
	}
	else
	{
		if (else_)
		{
			res = else_->calculate(istr, ostr);
		}
	}

	return res;
}


int While::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int res;
	int condition = lhs_->calculate(istr, ostr);

	while (condition)
	{
		rhs_->calculate(istr, ostr);
		res = (condition = lhs_->calculate(istr, ostr));
	}

	return res;
}


int Print::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int val = lhs_->calculate(istr, ostr);

	ostr << val << std::endl;

	return val;
}


int Arithmetic::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int res = lhs_->calculate(istr, ostr);

	return res;
}


//-------------------------------------------------NAMES----------------------------------------------------


std::string If::name() const
{
	return "if";
}

std::string While::name() const
{
	return "while";
}

std::string Print::name() const
{
	return "print";
}

std::string Arithmetic::name() const
{
	return lhs_->short_name();
}


#endif


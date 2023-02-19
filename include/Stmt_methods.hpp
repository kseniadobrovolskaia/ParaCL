#ifndef STMT_METHODS_H
#define STMT_METHODS_H


#include "Parser_arith.hpp"



//---------------------------------------STATEMENT_CLASS---------------------------------------------------


class Statement{

	Statements_t kind_;

public:
	Statement(Statements_t kind) : kind_(kind) {};
	virtual ~Statement() = default;

	Statements_t get_kind() const { return kind_; };
	virtual Lex_t *get_lhs() const = 0;

	virtual std::string name() const = 0;
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) = 0;
};


//---------------------------------------STATEMENT_CLASSES-------------------------------------------------


class Assign final : public Statement {

	Lex_t *lhs_;

public:
	Assign(Lex_t *lhs) :
	Statement(Statements_t::ASSIGN), lhs_(lhs) {};
	virtual ~Assign() = default;

	virtual Lex_t *get_lhs() const override { return lhs_; };
	
	virtual std::string name() const override;
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


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
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) override;
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
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) override;
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
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Inc_Dec final : public Statement {

	Lex_t *lhs_;

public:
	Inc_Dec(Lex_t *lhs) :
	Statement(static_cast<Statements_t>(lhs->get_data())), lhs_(lhs) {};
	virtual ~Inc_Dec() = default;
	
	virtual Lex_t *get_lhs() const override { return lhs_; };

	virtual std::string name() const override;
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) override;
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
	virtual void run_stmt(std::istream & istr, std::ostream & ostr) override;
};


//--------------------------------------------RUN_STATEMENTS------------------------------------------------


void run_program(std::vector<Statement*> prog, std::istream & istr, std::ostream & ostr);


void Assign::run_stmt(std::istream & istr, std::ostream & ostr)
{
	lhs_->calculate(istr);
}


void If::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int condition = lhs_->calculate(istr);

	if (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs(), istr, ostr);
	}
	else
	{
		if (else_)
		{
			run_program(static_cast<Scope*>(else_)->get_lhs(), istr, ostr);
		}
	}
}


void While::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int condition = lhs_->calculate(istr);

	while (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs(), istr, ostr);
		condition = lhs_->calculate(istr);
	}
}


void Print::run_stmt(std::istream & istr, std::ostream & ostr)
{
	int val = lhs_->calculate(istr);
	
	ostr << val << std::endl;
}


void Inc_Dec::run_stmt(std::istream & istr, std::ostream & ostr)
{
	lhs_->calculate(istr);
}

void Arithmetic::run_stmt(std::istream & istr, std::ostream & ostr)
{
	lhs_->calculate(istr);
}


//-------------------------------------------------NAMES----------------------------------------------------


std::string Assign::name() const
{
	return lhs_->short_name();
}

std::string Arithmetic::name() const
{
	return lhs_->short_name();
}

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

std::string Inc_Dec::name() const
{
	return lhs_->short_name();
}


#endif


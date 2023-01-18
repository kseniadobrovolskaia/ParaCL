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
	virtual std::string name() const = 0;
	virtual void run_stmt() = 0;
	virtual Lex_t *get_lhs() const = 0;
};


//---------------------------------------STATEMENT_CLASSES-------------------------------------------------


class Assign final : public Statement {

	Lex_t *lhs_;

public:
	Assign(Lex_t *lhs) :
	Statement(Statements_t::ASSIGN), lhs_(lhs) {};
	virtual ~Assign() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Assign_type get_type() const { return static_cast<Assign_node*>(lhs_)->get_type(); };
};


//----------------------------------------------------------------------------------------------------------


class If final : public Statement {

	Lex_t *lhs_, *rhs_;
	Lex_t *else_ = nullptr;

public:
	If(Lex_t *lhs, Lex_t *rhs, Lex_t * Else) : 
	Statement(Statements_t::IF), lhs_(lhs), rhs_(rhs), else_(Else){};
	virtual ~If() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	Lex_t *get_else() const { return else_; };
};


//----------------------------------------------------------------------------------------------------------


class While final : public Statement {

	Lex_t *lhs_, *rhs_;

public:
	While(Lex_t *lhs, Lex_t *rhs) :
	Statement(Statements_t::WHILE), lhs_(lhs), rhs_(rhs){};
	virtual ~While() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
};


//----------------------------------------------------------------------------------------------------------


class Print final : public Statement {

	Lex_t *lhs_;

public:
	Print(Lex_t *lhs) :
	Statement(Statements_t::PRINT), lhs_(lhs) {};
	virtual ~Print() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
};


//----------------------------------------------------------------------------------------------------------


class Inc_Dec final : public Statement {

	Lex_t *lhs_;

public:
	Inc_Dec(Lex_t *lhs) :
	Statement(static_cast<Statements_t>(lhs->get_data())), lhs_(lhs) {};
	virtual ~Inc_Dec() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
};


//--------------------------------------------RUN_STATEMENTS------------------------------------------------

void run_program(std::vector<Statement*> prog);


void Assign::run_stmt()
{
	lhs_->calculate();
}


void If::run_stmt()
{
	int condition = lhs_->calculate();

	if (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs());
	}
	else
	{
		if (else_)
		{
			run_program(static_cast<Scope*>(else_)->get_lhs());
		}
	}
}


void While::run_stmt()
{
	int condition = lhs_->calculate();

	while (condition)
	{
		run_program(static_cast<Scope*>(rhs_)->get_lhs());
		condition = lhs_->calculate();
	}
}


void Print::run_stmt()
{
	int val = lhs_->calculate();

	std::cout << val << std::endl;
}


void Inc_Dec::run_stmt()
{
	lhs_->calculate();
}


//-------------------------------------------------NAMES----------------------------------------------------


std::string Assign::name() const
{
	return "=";
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
	if (this->get_kind() == Statements_t::INC)
	{
		return "++";
	}
	return "--";
}


#endif


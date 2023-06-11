#ifndef STMT_METHODS_H
#define STMT_METHODS_H

#include "Lex_methods.hpp"


//------------------------------------ABSTRACT_STATEMENT_CLASS---------------------------------------------------
#if 0 //Definition in file "Lex_methods.hpp", left here for comfort

class Statement{

	Statements_t kind_;

public:
	Statement(Statements_t kind) : kind_(kind) {};
	virtual ~Statement() = default;

	Statements_t get_kind() const { return kind_; };
	virtual Lex_t &get_lhs() const = 0;

	virtual std::string name() const = 0;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const = 0;
};

#endif

//---------------------------------------STATEMENT_CLASSES-------------------------------------------------


class If final : public Statement {

	std::shared_ptr<Lex_t> lhs_, rhs_, else_ = nullptr;

public:
	If(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, std::shared_ptr<Lex_t> Else) : 
	Statement(Statements_t::IF), lhs_(lhs), rhs_(rhs), else_(Else){};
	virtual ~If() = default;

	virtual Lex_t &get_lhs() const override { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	Lex_t &get_else() const { if (!else_) throw_exception("This operator has no \"else\" \n", lhs_->get_num() - 1); return *else_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class While final : public Statement {

	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	While(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs) :
	Statement(Statements_t::WHILE), lhs_(lhs), rhs_(rhs){};
	virtual ~While() = default;
	
	virtual Lex_t &get_lhs() const override { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Print final : public Statement {

	std::shared_ptr<Lex_t> lhs_;

public:
	Print(std::shared_ptr<Lex_t> lhs) :
	Statement(Statements_t::PRINT), lhs_(lhs) {};
	virtual ~Print() = default;
	
	virtual Lex_t &get_lhs() const override { return *lhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Return final : public Statement {

	std::shared_ptr<Lex_t> lhs_;

public:
	Return(std::shared_ptr<Lex_t> lhs) :
	Statement(Statements_t::RETURN), lhs_(lhs) {};
	virtual ~Return() = default;
	
	virtual Lex_t &get_lhs() const override { return *lhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Arithmetic final : public Statement {

	std::shared_ptr<Lex_t> lhs_;

public:
	Arithmetic(std::shared_ptr<Lex_t> lhs) :
	Statement(Statements_t::ARITHMETIC), lhs_(lhs) {};
	virtual ~Arithmetic() = default;
	
	virtual Lex_t &get_lhs() const override { return *lhs_; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const override;
};


#endif


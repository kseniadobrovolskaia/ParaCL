#ifndef LEX_METHODS_H
#define LEX_METHODS_H


#include "Scope_table.hpp"


extern std::shared_ptr<Scope_table> CURR_SCOPE;
extern bool RETURN_COMMAND;
extern int IN_FUNCTION;


//--------------------------------------------LEXEME_CLASSES------------------------------------------------

enum Value_type { INPUT, NUMBER };


class Value : public Lex_t {

	Value_type type_;

public:
	Value(Lex_t val, Value_type type) : Lex_t(val), type_(type){};
	Value_type get_type() const { return type_; }
	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//--------------------------------------------ABSTRACT_STATEMENT_CLASS-----------------------


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


//-----------------------------------------------DECLARATION---------------------------------


class Declaration final : public Statement {

	std::shared_ptr<Lex_t> func_;
	std::shared_ptr<Lex_t> scope_;
	std::vector<std::shared_ptr<Lex_t>> vars_;
	std::shared_ptr<Statement> this_;

public:
	Declaration(std::shared_ptr<Lex_t> func, std::vector<std::shared_ptr<Lex_t>> &vars) : 
	Statement(Statements_t::FUNC), func_(func), vars_(std::move(vars)) {};
	virtual ~Declaration() = default;
	
	void set_decl(std::shared_ptr<Statement> This) { this_ = This; };
	virtual Lex_t &get_lhs() const override { return *func_; };
	std::shared_ptr<Lex_t> get_scope() const { return scope_; };
	const std::vector<std::shared_ptr<Lex_t>> &get_args() const { return vars_; };
	void add_scope(std::shared_ptr<Lex_t> scope) { scope_ = scope; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Scope : public Lex_t {

	std::vector<std::shared_ptr<Statement>> stmts_;

public:
	Scope(std::vector<std::shared_ptr<Statement>> lhs, const Lex_t &scope) : Lex_t(scope), stmts_(std::move(lhs)){};
	const std::vector<std::shared_ptr<Statement>> &get_lhs() const { return stmts_; };
	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Function : public Lex_t {

	std::shared_ptr<Statement> decl_;
	std::vector<std::shared_ptr<Lex_t>> args_;

public:
	Function(std::shared_ptr<Statement> decl, std::vector<std::shared_ptr<Lex_t>> &args, const Lex_t &func) : Lex_t(func), decl_(decl), args_(std::move(args)){};
	std::shared_ptr<Statement> get_rhs() const { return decl_; };
	const std::vector<std::shared_ptr<Lex_t>> &get_args() const { return args_; };
	void print_args() const { std::for_each(args_.begin(), args_.end(), [](std::shared_ptr<Lex_t> arg){ std::cout << arg->name() << " ";});};
	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class BinOp : public Lex_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	BinOp() = default;
  	BinOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, const Lex_t &binop) : Lex_t(binop), lhs_(lhs), rhs_(rhs){};
  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	CompOp() = default;
  	CompOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, const Lex_t &compop) : Lex_t(compop), lhs_(lhs), rhs_(rhs){};
  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Negation : public Lex_t {

	std::shared_ptr<Lex_t> rhs_;
	
public:
 	Negation(std::shared_ptr<Lex_t> rhs, const Lex_t &neg) : Lex_t(neg), rhs_(rhs){};
 	Lex_t &get_rhs() const { return *rhs_; };
 	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------REF_TYPE----------------------------------------------------


class Ref_t
{
	Lex_t &var_;

public:
	Ref_t(Lex_t &var) : var_(var) {};
	virtual Lex_t &get_variable() const { return var_; };
};


//----------------------------------CLASSES_THAT_RETURN_A_REFERENCE-----------------------------------------


class Variable : public Lex_t {
	
public:
 	Variable(const Lex_t &var) : Lex_t(var) {};
 	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class Assign_node : public Lex_t, public Ref_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	Assign_node() = default;
  	Assign_node(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, Lex_t &var, const Lex_t &ass) : Lex_t(ass), Ref_t(var), lhs_(lhs), rhs_(rhs){};
  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t, public Ref_t {

  	std::shared_ptr<Lex_t> lhs_;

public:
	UnOp() = default;
  	UnOp(std::shared_ptr<Lex_t> lhs, Lex_t &var, const Lex_t &unop) : Lex_t(unop), Ref_t(var), lhs_(lhs) {};
  	virtual int calculate(std::istream &istr, std::ostream &ostr) const override;
};


#endif


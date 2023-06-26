#ifndef LEX_METHODS_H
#define LEX_METHODS_H


#include "Scope_table.hpp"


//--------------------------------------------LEXEME_CLASSES------------------------------------------------


enum Value_type { INPUT, NUMBER };


class Value : public Lex_t {

	Value_type type_;

public:
	Value(Lex_t val, Value_type type) : Lex_t(val), type_(type){};

	virtual ~Value() = default;

	Value_type get_type() const { return type_; }

	llvm::Value *codegen(AST_creator &creator) override;
	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class Scope : public Lex_t {

	std::vector<std::shared_ptr<Statement>> stmts_;

public:
	Scope(std::vector<std::shared_ptr<Statement>> lhs, const Lex_t &scope) : 
	Lex_t(scope), stmts_(std::move(lhs)){};

	virtual ~Scope() = default;

	const std::vector<std::shared_ptr<Statement>> &get_lhs() const { return stmts_; };

	llvm::Value *codegen(AST_creator &creator) override;
	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class Function : public Lex_t {

	///Arguments
	std::vector<std::shared_ptr<Lex_t>> args_;

public:
	Function(std::vector<std::shared_ptr<Lex_t>> &args, const Lex_t &func) : 
	Lex_t(func), args_(std::move(args)){};

	virtual ~Function() = default;

	const std::vector<std::shared_ptr<Lex_t>> &get_args() const   { return args_; };
	void                                       print_args() const { std::for_each(args_.begin(), args_.end(), [](std::shared_ptr<Lex_t> arg){ std::cout << arg->name() << " ";});};

	llvm::Value *codegen(AST_creator &creator) override;
	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class BinOp : public Lex_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
  	BinOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, const Lex_t &binop) : 
  	Lex_t(binop), lhs_(lhs), rhs_(rhs){};

  	virtual ~BinOp() = default;

  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };

  	llvm::Value *codegen(AST_creator &creator) override;
  	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
  	CompOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, const Lex_t &compop) : 
  	Lex_t(compop), lhs_(lhs), rhs_(rhs){};

  	virtual ~CompOp() = default;

  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };

  	llvm::Value *codegen(AST_creator &creator) override;
  	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class Negation : public Lex_t {

	std::shared_ptr<Lex_t> rhs_;
	
public:
 	Negation(std::shared_ptr<Lex_t> rhs, const Lex_t &neg) : 
 	Lex_t(neg), rhs_(rhs){};

 	virtual ~Negation() = default;

 	Lex_t &get_rhs() const { return *rhs_; };

 	llvm::Value *codegen(AST_creator &creator) override;
 	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------REF_TYPE----------------------------------------------------


/**
 * @brief class Ref_t - to represent objects that return the variable they are used on
 * 
 */
class Ref_t
{
	std::shared_ptr<Lex_t> var_;

public:
	Ref_t(std::shared_ptr<Lex_t> var) : var_(var) {};

	virtual ~Ref_t() = default;

	virtual std::shared_ptr<Lex_t> get_variable() const { return var_; };
};


//----------------------------------CLASSES_THAT_RETURN_A_VARIABLE-----------------------------------------


class Variable : public Lex_t {
	
public:
 	Variable(const Lex_t &var) : Lex_t(var) {};

 	virtual ~Variable() = default;

 	llvm::Value *codegen(AST_creator &creator) override;
 	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class Assign_node : public Lex_t, public Ref_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
  	Assign_node(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, std::shared_ptr<Lex_t> var, const Lex_t &ass) : 
  	Lex_t(ass), Ref_t(var), lhs_(lhs), rhs_(rhs){};

  	virtual ~Assign_node() = default;

  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };

  	llvm::Value *codegen(AST_creator &creator) override;
  	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t, public Ref_t {

  	std::shared_ptr<Lex_t> lhs_;

public:
  	UnOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> var, const Lex_t &unop) : 
  	Lex_t(unop), Ref_t(var), lhs_(lhs) {};

  	virtual ~UnOp() = default;
  	
  	llvm::Value *codegen(AST_creator &creator) override;
  	virtual int  calculate(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


#endif//LEX_METHODS_H


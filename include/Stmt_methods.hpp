#ifndef STMT_METHODS_H
#define STMT_METHODS_H

#include "Lex_methods.hpp"


//------------------------------------ABSTRACT_STATEMENT_CLASS---------------------------------------------------


/**
 * @brief class Statement - program and any scope consists of an array of Statements
 * 
 */
class Statement{

	Statements_t kind_;

public:
	Statement(Statements_t kind) : kind_(kind) {};

	virtual ~Statement() = default;

	Statements_t        get_kind() const { return kind_; };
	virtual Lex_t      &get_lhs()  const = 0;
	virtual std::string name()     const = 0;

	virtual llvm::Function *codegen_func(Codegen_creator &creator) { return nullptr; };
	virtual llvm::Value    *codegen(Codegen_creator &creator) = 0;
	virtual int             run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const = 0;
};


//-----------------------------------------STATEMENT_CLASSES---------------------------------------------------------------------------------------------


class Declaration final : public Statement {

	///No global at the place of declaration
	std::shared_ptr<Lex_t> func_;

	///Function
	std::shared_ptr<Lex_t> scope_;

	///To place a pointer to this declaration in a Scope_table
	std::weak_ptr<Statement> this_;

	std::string global_name_;

	///Function arguments
	std::vector<std::shared_ptr<Lex_t>> vars_;


public:
	Declaration(std::shared_ptr<Lex_t> func, std::vector<std::shared_ptr<Lex_t>> &vars) : 
	Statement(Statements_t::FUNC), func_(func), vars_(std::move(vars)) {};

	Declaration(std::shared_ptr<Lex_t> func, std::string gl_name, std::vector<std::shared_ptr<Lex_t>> &vars) : 
	Statement(Statements_t::FUNC), func_(func), global_name_(gl_name), vars_(std::move(vars)) {};

	virtual ~Declaration() = default;
	
	void                                       set_decl(std::weak_ptr<Statement> This) { this_ = This; };
	virtual Lex_t                             &get_lhs() const override                { return *func_; };
	std::shared_ptr<Lex_t>                     get_scope() const                       { return scope_; };
	const std::vector<std::shared_ptr<Lex_t>> &get_args() const                        { return vars_; };
	void                                       add_scope(std::shared_ptr<Lex_t> scope) { scope_ = scope; };
	virtual std::string                        name() const override;
	std::string 							   get_func_name() const                   { return func_->short_name(); };
	std::string                                get_global_name() const                 { return global_name_; };

	llvm::Value    *codegen(Codegen_creator &creator) override;
	llvm::Function *codegen_func(Codegen_creator &creator) override;
	virtual int     run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------


class If final : public Statement {

	std::shared_ptr<Lex_t> lhs_, rhs_, else_ = nullptr;

public:
	If(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, std::shared_ptr<Lex_t> Else) : 
	Statement(Statements_t::IF), lhs_(lhs), rhs_(rhs), else_(Else){};

	virtual ~If() = default;

	virtual Lex_t          &get_lhs() const override { return *lhs_; };
  	Lex_t                  &get_rhs() const          { return *rhs_; };
  	Lex_t                  &get_else() const         { if (!else_) throw_exception("This \"if\" has no \"else\"\n", lhs_->get_num() - 1); return *else_; };
  	std::shared_ptr<Lex_t> get_Else() const      { return else_; };
	virtual std::string    name() const override;

	llvm::Value *codegen(Codegen_creator &creator) override;
	virtual int  run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//--------------------------------------------------------------------------------------------------------------------------------------------------------


class While final : public Statement {

	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	While(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs) :
	Statement(Statements_t::WHILE), lhs_(lhs), rhs_(rhs){};

	virtual ~While() = default;
	
	virtual Lex_t      &get_lhs() const override { return *lhs_; };
  	Lex_t              &get_rhs() const          { return *rhs_; };
	virtual std::string name() const override;

	llvm::Value *codegen(Codegen_creator &creator) override;
	virtual int  run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------


class Print final : public Statement {

	std::shared_ptr<Lex_t> lhs_;

public:
	Print(std::shared_ptr<Lex_t> lhs) :
	Statement(Statements_t::PRINT), lhs_(lhs) {};

	virtual ~Print() = default;
	
	virtual Lex_t      &get_lhs() const override { return *lhs_; };
	virtual std::string name() const override;

	llvm::Value *codegen(Codegen_creator &creator) override;
	virtual int  run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------


class Return final : public Statement {

	std::shared_ptr<Lex_t> lhs_;

public:
	Return(std::shared_ptr<Lex_t> lhs) :
	Statement(Statements_t::RETURN), lhs_(lhs) {};

	virtual ~Return() = default;
	
	virtual Lex_t      &get_lhs() const override { return *lhs_; };
	virtual std::string name() const override;

	llvm::Value *codegen(Codegen_creator &creator) override;
	virtual int  run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------


class Arithmetic final : public Statement {

	std::shared_ptr<Lex_t> lhs_;

public:
	Arithmetic(std::shared_ptr<Lex_t> lhs) :
	Statement(Statements_t::ARITHMETIC), lhs_(lhs) {};

	virtual ~Arithmetic() = default;
	
	virtual Lex_t      &get_lhs() const override { return *lhs_; };
	virtual std::string name() const override;

	llvm::Value    *codegen(Codegen_creator &creator) override;
	llvm::Function *codegen_func(Codegen_creator &creator) override;
	virtual int     run_stmt(std::istream &istr, std::ostream &ostr, AST_creator &creator) const override;
};


#endif//STMT_METHODS_H


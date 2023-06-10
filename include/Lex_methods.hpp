#ifndef LEX_METHODS_H
#define LEX_METHODS_H


#include "Scope_table.hpp"


Scope_table *CURR_SCOPE;
bool RETURN_COMMAND = 0;
int IN_FUNCTION = 0;


//--------------------------------------------LEXEME_CLASSES------------------------------------------------

enum Value_type { INPUT, NUMBER };


class Value : public Lex_t {

	Value_type type_;

public:
	Value(Lex_t val, Value_type type) : Lex_t(val), type_(type){};
	Value_type get_type() const { return type_; }
	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
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
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) = 0;
};


//-----------------------------------------------DECLARATION---------------------------------


class Declaration final : public Statement {

	std::shared_ptr<Lex_t> func_;
	std::shared_ptr<Lex_t> scope_;
	std::vector<std::shared_ptr<Lex_t>> vars_;

public:
	Declaration(std::shared_ptr<Lex_t> func, std::vector<std::shared_ptr<Lex_t>> &vars) : 
	Statement(Statements_t::FUNC), func_(func), vars_(std::move(vars)) {};

	Declaration(std::shared_ptr<Lex_t> func, std::vector<std::shared_ptr<Lex_t>> &vars, std::shared_ptr<Lex_t> scope) :
	Statement(Statements_t::FUNC), func_(func), scope_(scope), vars_(vars) {};
	virtual ~Declaration() = default;
	
	virtual Lex_t &get_lhs() const override { return *func_; };
	std::shared_ptr<Lex_t> get_scope() const { return scope_; };
	std::vector<std::shared_ptr<Lex_t>> &get_args() { return vars_; };
	void add_scope(std::shared_ptr<Lex_t> scope) { scope_ = scope; };

	virtual std::string name() const override;
	virtual int run_stmt(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Scope : public Lex_t {

	std::vector<Statement*> stmts_;

public:
	Scope(std::vector <Statement*> lhs, const Lex_t &scope) : Lex_t(scope), stmts_(std::move(lhs)){};
	std::vector<Statement*> get_lhs() const { return stmts_; };
	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Function : public Lex_t {

	Statement *decl_; // i think this is no nessesary
	std::vector<std::shared_ptr<Lex_t>> args_;

public:
	Function(Statement *decl, std::vector<std::shared_ptr<Lex_t>> &args, const Lex_t &func) : Lex_t(func), decl_(decl), args_(std::move(args)){};
	Statement *get_rhs() const { return decl_; };
	std::vector<std::shared_ptr<Lex_t>> &get_args() { return args_; };
	void print_args() const { for (int i = 0; i < static_cast<int>(args_.size()); i++)
								{ std::cout << args_[i] << " ";} std::cout << std::endl;}
	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class BinOp : public Lex_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	BinOp() = default;
  	BinOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, const Lex_t &binop) : Lex_t(binop), lhs_(lhs), rhs_(rhs){};
  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class CompOp : public Lex_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	CompOp() = default;
  	CompOp(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, const Lex_t &compop) : Lex_t(compop), lhs_(lhs), rhs_(rhs){};
  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Negation : public Lex_t {

	std::shared_ptr<Lex_t> rhs_;
	
public:
 	Negation(std::shared_ptr<Lex_t> rhs, const Lex_t &neg) : Lex_t(neg), rhs_(rhs){};
 	Lex_t &get_rhs() const { return *rhs_; };
 	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------REF_TYPE----------------------------------------------------


class Ref_t
{
	Lex_t &var_;

public:
	Ref_t(Lex_t &var) : var_(var) {};
	virtual Lex_t &get_variable() { return var_; };
};


//----------------------------------CLASSES_THAT_RETURN_A_REFERENCE-----------------------------------------


class Variable : public Lex_t {
	
public:
 	Variable(const Lex_t &var) : Lex_t(var) {};
 	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class Assign_node : public Lex_t, public Ref_t {

  	std::shared_ptr<Lex_t> lhs_, rhs_;

public:
	Assign_node() = default;
  	Assign_node(std::shared_ptr<Lex_t> lhs, std::shared_ptr<Lex_t> rhs, Lex_t &var, const Lex_t &ass) : Lex_t(ass), Ref_t(var), lhs_(lhs), rhs_(rhs){};
  	Lex_t &get_lhs() const { return *lhs_; };
  	Lex_t &get_rhs() const { return *rhs_; };
  	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//----------------------------------------------------------------------------------------------------------


class UnOp : public Lex_t, public Ref_t {

  	std::shared_ptr<Lex_t> lhs_;

public:
	UnOp() = default;
  	UnOp(std::shared_ptr<Lex_t> lhs, Lex_t &var, const Lex_t &unop) : Lex_t(unop), Ref_t(var), lhs_(lhs) {};
  	virtual int calculate(std::istream &istr, std::ostream &ostr) override;
};


//---------------------------------------------CALCULATE----------------------------------------------------


int Value::calculate(std::istream &istr, std::ostream &ostr)
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


int Variable::calculate(std::istream &istr, std::ostream &ostr)
{
	std::string var_name = vars[this->get_data()];

	return CURR_SCOPE->get_var(var_name, this->get_num());
}


int Negation::calculate(std::istream &istr, std::ostream &ostr)
{
	int right = rhs_->calculate(istr, ostr);

	return !right;
}


int Assign_node::calculate(std::istream &istr, std::ostream &ostr)
{
	Lex_t *var = lhs_.get();
	std::string var_name;

	int right_part = rhs_->calculate(istr, ostr);

	if (dynamic_cast<Variable*>(var))
	{
		var_name = vars[var->get_data()];
		CURR_SCOPE->init_var(var_name);
	}
	else
	{
		var = &get_variable();
		var_name = vars[var->get_data()];
	}

	lhs_->calculate(istr, ostr);

	CURR_SCOPE->get_var(var_name, var->get_num()) = right_part;

	return CURR_SCOPE->get_var(var_name, var->get_num());
}


int BinOp::calculate(std::istream &istr, std::ostream &ostr)
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


int UnOp::calculate(std::istream &istr, std::ostream &ostr)
{
	lhs_->calculate(istr, ostr);
	Lex_t *var = lhs_.get();

	std::string var_name;
	
	if (!dynamic_cast<Variable*>(var))
	{
		var = &get_variable();
	}
	
	var_name = vars[var->get_data()];

	switch (this->get_data())
	{
		case Statements_t::INC:
			CURR_SCOPE->get_var(var_name, this->get_num()) += 1;
			return CURR_SCOPE->get_var(var_name, this->get_num());
		case Statements_t::DEC:
			CURR_SCOPE->get_var(var_name, this->get_num()) -= 1;
			return CURR_SCOPE->get_var(var_name, this->get_num());
	}

	throw_exception("Error: it is not clear what is in function \"UnOp::calculate\"\n", this->get_num());
	return 1;
}


int CompOp::calculate(std::istream &istr, std::ostream &ostr)
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


int Scope::calculate(std::istream &istr, std::ostream &ostr)
{
	int res = 0;
	int size_program = stmts_.size();

	Scope_table *scp_table = new Scope_table(CURR_SCOPE);
	Scope_table *old_table = CURR_SCOPE;

	CURR_SCOPE = scp_table;

	for (int prog_elem = 0; prog_elem < size_program; prog_elem++)
	{
		res = (stmts_[prog_elem])->run_stmt(istr, ostr);

		if (RETURN_COMMAND)
			break;
	}

	CURR_SCOPE = old_table;

	return res;
}


int Function::calculate(std::istream &istr, std::ostream &ostr)
{
	IN_FUNCTION++;
	int res = 0;
	Scope_table *func_table_ = new Scope_table();


	const std::vector<std::shared_ptr<Lex_t>> &args = static_cast<Declaration*>(decl_)->get_args();
	int count_args = args.size();

	if (static_cast<int>(args_.size()) != count_args)
	{
		throw_exception("Incorrect number of function arguments\n", this->get_num());
	}

	for (int num_arg = 0; num_arg < count_args; num_arg++)
	{
		std::string name = args[num_arg]->short_name();

		func_table_->init_var(name);

		int val = args_[num_arg]->calculate(istr, ostr);
		func_table_->get_var(name, args_[num_arg]->get_str()) = val;
	}

	std::shared_ptr<Lex_t> scope = static_cast<Declaration*>(decl_)->get_scope();


	Scope_table *old_curr_scope = CURR_SCOPE;
	CURR_SCOPE = func_table_;

	std::vector<Statement*> stmts_ = static_cast<Scope*>(scope.get())->get_lhs();

	for (int prog_elem = 0; prog_elem < static_cast<int>(stmts_.size()); prog_elem++)
	{
		res = (stmts_[prog_elem])->run_stmt(istr, ostr);

		if (RETURN_COMMAND)
		{
			RETURN_COMMAND = 0;
			break;
		}
	}

	CURR_SCOPE = old_curr_scope;

	IN_FUNCTION--;
	return res;
}


#endif


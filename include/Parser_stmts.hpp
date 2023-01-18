#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H



#include "Parser_arith.hpp"



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


//----------------------------------------------------------------------------------------------------------


class Assign final : public Statement {

	Lex_t *lhs_;
	//Assign_type type_;

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

public:
	If(Lex_t *lhs, Lex_t *rhs) :
	Statement(Statements_t::IF), lhs_(lhs), rhs_(rhs){};
	virtual ~If() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
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
	virtual Lex_t *get_lhs() const override{ return lhs_; };
};


//----------------------------------------------------------------------------------------------------------


class Inc_Dec final : public Statement {

	Lex_t *lhs_;

public:
	Inc_Dec(Lex_t *lhs, Statements_t type) :
	Statement(type), lhs_(lhs) {};
	virtual ~Inc_Dec() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
};


//----------------------------------------------------------------------------------------------------------



std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array);
Statement *parse_if_while(std::vector<Lex_t *> &lex_array, Statements_t type);
Statement *parse_assign(std::vector<Lex_t *> &lex_array);
Statement *parse_print(std::vector<Lex_t *> &lex_array);
Statement *parse_unop(std::vector<Lex_t *> &lex_array);
int is_unop_stmt(Lex_t *node);
int is_semicol(Lex_t *node);
int is_scope(Lex_t *node);



Statement *parse_if(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	if (is_brace(lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw std::logic_error("Syntax error");
	}

	Lex_t* L = parse_arithmetic(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::LSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"if\"");
	}

	token_counter(INCREMENT);

	std::vector<Statement*> scope = parse_program(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"if\"");
	}

	token_counter(INCREMENT);

	Lex_t *R = new Scope(scope);

	return new If(L, R);
}


Statement *parse_while(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	if (is_brace(lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw std::logic_error("Syntax error");
	}

	Lex_t* L = parse_arithmetic(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::LSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"while\"");
	}

	token_counter(INCREMENT);

	std::vector<Statement*> scope = parse_program(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"while\"");
	}

	token_counter(INCREMENT);

	Lex_t *R = new Scope(scope);

	return new While(L, R);
}


Statement *parse_print(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	Lex_t* L = parse_arithmetic(lex_array);

	if(!is_semicol(lex_array[token_counter(USE_CURRENT)]))
	{
		throw std::logic_error("Invalid input: bad semicols in \"print\"");
	}

	token_counter(INCREMENT);

	return new Print(L);
}


std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array)
{
	std::vector<Statement*> prog_elems;
	int size = lex_array.size();
	Statement *stmt;

	while (token_counter(GET_CURRENT) < size)
	{		
		switch (lex_array[token_counter(USE_CURRENT)]->get_kind())
		{
			case Lex_kind_t::BRACE:
			case Lex_kind_t::VAR:
			{
				Lex_t *Stmt = parse_arithmetic(lex_array);

				switch (Stmt->get_kind())
				{
					case Lex_kind_t::UNOP:
					{
						stmt = new Inc_Dec(static_cast<UnOp*>(Stmt)->get_var(),
						static_cast<Statements_t>(Stmt->get_data()));
						break;
					}
					case Lex_kind_t::STMT:
					{
						if (Stmt->get_data() != Statements_t::ASSIGN)
						{
							throw std::logic_error("Invalid input");
						}
						stmt = new Assign(Stmt);
						break;
					}
					default:
					{
						std::cout << "В качестве утверждения было передано " << Stmt->name() << std::endl;
						throw std::logic_error("Arithmetic expression is not a statement");
					}
				}
				
				if(!is_semicol(lex_array[token_counter(USE_CURRENT)]))
				{
					throw std::logic_error("Invalid input: bad semicols");
				}
				token_counter(INCREMENT);
				break;
			}
			case Lex_kind_t::STMT:
			{
				switch (lex_array[token_counter(USE_CURRENT)]->get_data())
				{
				case Statements_t::IF:
					stmt = parse_if(lex_array);
					break;
				case Statements_t::WHILE:
					stmt = parse_while(lex_array);
					break;
				case Statements_t::PRINT:
					stmt = parse_print(lex_array);
					break;
				}
				break;
			}
			default:
			{
				if(is_scope(lex_array[token_counter(USE_CURRENT)]) == Scope_t::RSCOPE)
				{
					return prog_elems;
				}
				else
				{
					throw std::logic_error("Invalid input: bad program building");
				}
			}
		}

		prog_elems.push_back(stmt);
	}
	return prog_elems;
}


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


int is_unop_stmt(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::UNOP)
	{
		return 0;
	}
	if (node->get_data() == Statements_t::INC)
	{
		return Statements_t::INC;
	}
	return Statements_t::DEC;
}


int is_scope(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::SCOPE)
	{
		return -1;
	}
	return node->get_data();
}


int is_semicol(Lex_t *node)
{
	if (node->get_kind() != Lex_kind_t::SYMBOL)
	{
		return 0;
	}
	if (node->get_data() != Symbols_t::SEMICOL)
	{
		return 0;
	}
	return 1;
}

#endif

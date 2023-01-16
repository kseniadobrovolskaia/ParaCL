#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H



#include "Parser_arith.hpp"



class Statement{

	Keywords_t kind_;

public:
	Statement(Keywords_t kind) : kind_(kind) {};
	virtual ~Statement() = default;
	Keywords_t get_kind() const { return kind_; };
	virtual std::string name() const = 0;
	virtual void run_stmt() = 0;
	virtual Lex_t *get_lhs() const = 0;
};


//----------------------------------------------------------------------------------------------------------


class Assign final : public Statement {

	Lex_t *lhs_, *rhs_;
	Assign_type type_;

public:
	Assign(Lex_t *lhs, Lex_t *rhs, Assign_type type) : Statement(Keywords_t::ASSIGN), lhs_(lhs), rhs_(rhs), type_(type) {};
	virtual ~Assign() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
  	Assign_type get_type() const { return type_; };
};


//----------------------------------------------------------------------------------------------------------


class If final : public Statement {

	Lex_t *lhs_, *rhs_;

public:
	If(Lex_t *lhs, Lex_t *rhs) : Statement(Keywords_t::IF), lhs_(lhs), rhs_(rhs){};
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
	While(Lex_t *lhs, Lex_t *rhs) : Statement(Keywords_t::WHILE), lhs_(lhs), rhs_(rhs){};
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
	Print(Lex_t *lhs) : Statement(Keywords_t::PRINT), lhs_(lhs) {};
	virtual ~Print() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override{ return lhs_; };
};


//----------------------------------------------------------------------------------------------------------


class Inc_Dec final : public Statement {

	Lex_t *lhs_;

public:
	Inc_Dec(Lex_t *lhs, Keywords_t type) : Statement(type), lhs_(lhs) {};
	virtual ~Inc_Dec() = default;
	virtual std::string name() const override;
	virtual void run_stmt() override;
	virtual Lex_t *get_lhs() const override{ return lhs_; };
};


//----------------------------------------------------------------------------------------------------------



std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array);
Statement *parse_if_while(std::vector<Lex_t *> &lex_array, Keywords_t type);
Statement *parse_assign(std::vector<Lex_t *> &lex_array);
Statement *parse_print(std::vector<Lex_t *> &lex_array);
Statement *parse_unop(std::vector<Lex_t *> &lex_array);
int is_unop_stmt(Lex_t *node);
int is_semicol(Lex_t *node);
int is_scope(Lex_t *node);


Statement *parse_assign(std::vector<Lex_t *> &lex_array)
{
	Lex_t *R;
	Lex_t *L = lex_array[token_counter(USE_CURRENT)];

	token_counter(INCREMENT);

	std::string name_lhs = vars[L->get_data()];

	if (!(VARS.contains(name_lhs)))
	{
	 	VARS[name_lhs] = 0;
	}

	L = new Variable(L->get_data());

	if(!is_assign(lex_array[token_counter(USE_CURRENT)]))
	{
		throw std::logic_error("Invalid input: bad assignment");
	}

	token_counter(INCREMENT);

	Assign_type type;

	if (is_scan(lex_array[token_counter(USE_CURRENT)]))
	{
		type = Assign_type::INPUT;

		R = new Lex_t(Lex_kind_t::KEYWD, Keywords_t::SCAN);

		token_counter(INCREMENT);
	}
	else
	{
		type = Assign_type::ARITHMETIC;

		R = parse_arithmetic(lex_array);
	}

	return new Assign(L, R, type);
}


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
		throw std::logic_error("Invalid input: bad scope");
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
		throw std::logic_error("Invalid input: bad scope");
	}

	token_counter(INCREMENT);

	std::vector<Statement*> scope = parse_program(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"if\"");
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
		throw std::logic_error("Invalid input: bad semicols");
	}

	token_counter(INCREMENT);

	return new Print(L);
}


Statement *parse_unop(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = lex_array[token_counter(USE_CURRENT)];

	token_counter(INCREMENT);

	std::string name_lhs = vars[L->get_data()];

	if (!(VARS.contains(name_lhs)))
	{
	 	throw std::runtime_error("Uninitialized variable witn unop");
	}

	L = new Variable(L->get_data());
	
	int is_un = is_unop_stmt(lex_array[token_counter(USE_CURRENT)]);

	if (is_un == Keywords_t::INCREM)
	{
		VARS[name_lhs]++;
	}
	else
	{
		VARS[name_lhs]--;
	}
	
	token_counter(INCREMENT);

	return new Inc_Dec(L, static_cast<Keywords_t>(is_un));

}


std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array)
{
	std::vector<Statement*> prog_elems;
	int size = lex_array.size();
	Statement *stmt;

	while (token_counter(GET_CURRENT) < size)
	{		
		if (lex_array[token_counter(USE_CURRENT)]->get_kind() == Lex_kind_t::VAR)
		{
			if(is_unop(lex_array[token_counter(USE_NEXT) + 1]) >= 0) 
			{
				stmt = parse_unop(lex_array);
			}
			else
			{
				stmt = parse_assign(lex_array);
			}
			
			if(!is_semicol(lex_array[token_counter(USE_CURRENT)]))
			{
				throw std::logic_error("Invalid input: bad semicols");
			}
			token_counter(INCREMENT);
		}
		else if (lex_array[token_counter(USE_CURRENT)]->get_kind() == Lex_kind_t::KEYWD)
		{
			switch (lex_array[token_counter(USE_CURRENT)]->get_data())
			{
			case Keywords_t::IF:
				stmt = parse_if(lex_array);
				break;
			case Keywords_t::WHILE:
				stmt = parse_while(lex_array);
				break;
			case Keywords_t::PRINT:
				stmt = parse_print(lex_array);
				break;
			}
		}
		else
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
	if (this->get_kind() == Keywords_t::INCREM)
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
	if (node->get_data() == UnOp_t::INC)
	{
		return Keywords_t::INCREM;
	}
	return Keywords_t::DECREM;
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
	if (node->get_kind() != Lex_kind_t::KEYWD)
	{
		return 0;
	}
	if (node->get_data() != Keywords_t::SEMICOL)
	{
		return 0;
	}
	return 1;
}

#endif

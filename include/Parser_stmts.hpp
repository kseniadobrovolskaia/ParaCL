#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H



#include "Lexer.hpp"
#include "Parser_arith.hpp"


class Statement{

	Keywords_t kind_;
	Lex_t *lhs_, *rhs_;

public:
	Statement(Lex_t *lhs, Lex_t *rhs, Keywords_t kind) : kind_(kind), lhs_(lhs), rhs_(rhs){};
	virtual ~Statement() = default;
	Keywords_t get_kind() const { return kind_; };
	std::string name() const;
  	Lex_t *get_lhs() const { return lhs_; };
  	Lex_t *get_rhs() const { return rhs_; };
};



//----------------------------------------------------------------------------------------------------------




Statement *parse_assign(std::vector<Lex_t *> &lex_array)
{
	Lex_t *L = lex_array[token_counter(GET_CURRENT)];
	token_counter(INCREMENT);

	if (L->get_kind() != Lex_kind_t::VAR)
	{
		throw std::logic_error("Invalid input: bad assignment");
	}

	std::string name_lhs = vars[L->get_data()];

	if (!(VARS.contains(name_lhs)))
	{
	 	VARS[name_lhs] = 0;
	}

	int var_num = L->get_data();
	L = new Variable(var_num);

	if((lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::KEYWD) 
	|| (lex_array[token_counter(GET_CURRENT)]->get_data() != Keywords_t::ASSIGN))
	{
		throw std::logic_error("Invalid input: bad assignment");
	}
	token_counter(INCREMENT);

	Lex_t *R = parse_arithmetic(lex_array);

	VARS[name_lhs] = calculate(R);

	return new Statement(L, R, Keywords_t::ASSIGN);
}


Statement *parse_if(std::vector<Lex_t *> &lex_array)
{


}


std::vector<Statement*> parse_prog(std::vector<Lex_t *> &lex_array)
{
	std::vector<Statement*> prog_elems;
	int size = lex_array.size();
	Statement *stmt;

	while (token_counter(GET_CURRENT) < size)
	{
		if (lex_array[token_counter(GET_CURRENT)]->get_kind() == Lex_kind_t::VAR)
		{
			stmt = parse_assign(lex_array);
		}
		else if (lex_array[token_counter(GET_CURRENT)]->get_kind() == Lex_kind_t::KEYWD)
		{
			switch (lex_array[token_counter(GET_CURRENT)]->get_data())
			{
			case Keywords_t::IF:
				stmt = parse_if(lex_array);
				break;
			case Keywords_t::WHILE:
				break;
			case Keywords_t::PRINT:
				break;
			case Keywords_t::SCAN:
				break;
			}
		}
		else
		{
			throw std::logic_error("Invalid input: bad program building");
		}

		prog_elems.push_back(stmt);
		if((lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::KEYWD) 
		|| (lex_array[token_counter(GET_CURRENT)]->get_data() != Keywords_t::SEMICOL))
		{
			throw std::logic_error("Invalid input: bad semicols");
		}
		token_counter(INCREMENT);
	}

	return prog_elems;
}


std::string Statement::name() const
{
	switch (kind_)
	{
	case Keywords_t::ASSIGN:
		return "=";
	case Keywords_t::IF:
		return "if";
	case Keywords_t::WHILE:
		return "while";
	case Keywords_t::PRINT:
		return "print";
	case Keywords_t::SCAN:
		return "?";
	case Keywords_t::SEMICOL:
		return ";";
	}

	return nullptr;
}

#endif

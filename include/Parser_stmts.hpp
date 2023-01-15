#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H



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


std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array);
Statement *parse_if_while(std::vector<Lex_t *> &lex_array, Keywords_t type);
Statement *parse_assign(std::vector<Lex_t *> &lex_array);
Statement *parse_print(std::vector<Lex_t *> &lex_array);
Statement *parse_unop(std::vector<Lex_t *> &lex_array);



Statement *parse_assign(std::vector<Lex_t *> &lex_array)
{
	Lex_t *R;
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

	if ((lex_array[token_counter(GET_CURRENT)]->get_kind() == Lex_kind_t::KEYWD)
	&& (lex_array[token_counter(GET_CURRENT)]->get_data() == Keywords_t::SCAN))
	{
		R = new Lex_t(Lex_kind_t::KEYWD, Keywords_t::SCAN);
		token_counter(INCREMENT);
	}
	else
	{
		R = parse_arithmetic(lex_array);

		VARS[name_lhs] = calculate(R);
	}

	return new Statement(L, R, Keywords_t::ASSIGN);
}


Statement *parse_if_while(std::vector<Lex_t *> &lex_array, Keywords_t type)
{
	token_counter(INCREMENT);

	Lex_t* L = parse_arithmetic(lex_array);

	if((lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::SCOPE) 
	|| (lex_array[token_counter(GET_CURRENT)]->get_data() != Scope_t::LSCOPE))
	{
		throw std::logic_error("Invalid input: bad scope");
	}

	token_counter(INCREMENT);

	std::vector<Statement*> scope = parse_program(lex_array);

	if((lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::SCOPE) 
	|| (lex_array[token_counter(GET_CURRENT)]->get_data() != Scope_t::RSCOPE))
	{
		throw std::logic_error("Invalid input: bad scope in \"if\"");
	}

	token_counter(INCREMENT);

	Lex_t *R = new Scope(scope);

	return new Statement(L, R, type);
}


Statement *parse_print(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	Lex_t* L = parse_arithmetic(lex_array);

	if((lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::KEYWD) 
	|| (lex_array[token_counter(GET_CURRENT)]->get_data() != Keywords_t::SEMICOL))
	{
		throw std::logic_error("Invalid input: bad semicols");
	}

	token_counter(INCREMENT);

	return new Statement(L, nullptr, Keywords_t::PRINT);
}


Statement *parse_unop(std::vector<Lex_t *> &lex_array)
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
	 	throw std::runtime_error("Uninitialized variable witn unop");
	}

	int var_num = L->get_data();
	L = new Variable(var_num);

	if(lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::UNOP) 
	{
		throw std::logic_error("Invalid input: bad unop");
	}

	if (lex_array[token_counter(GET_CURRENT)]->get_data() == UnOp_t::INC)
	{
		VARS[name_lhs]++;
		token_counter(INCREMENT);

		return new Statement(L, nullptr, Keywords_t::INCREM);
	}
	
	VARS[name_lhs]--;
	token_counter(INCREMENT);

	return new Statement(L, nullptr, Keywords_t::DECREM);

}


std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array)
{
	std::vector<Statement*> prog_elems;
	int size = lex_array.size();
	Statement *stmt;

	while (token_counter(GET_CURRENT) < size)
	{
		if (lex_array[token_counter(GET_CURRENT)]->get_kind() == Lex_kind_t::VAR)
		{
			if ((token_counter(GET_CURRENT) + 1) > program_size)
			{
				throw std::logic_error("Syntax error");
			}
			if(lex_array[token_counter(GET_CURRENT) + 1]->get_kind() == Lex_kind_t::UNOP) 
			{
				stmt = parse_unop(lex_array);
			}
			else
			{
				stmt = parse_assign(lex_array);
			}
			
			if((lex_array[token_counter(GET_CURRENT)]->get_kind() != Lex_kind_t::KEYWD) 
			|| (lex_array[token_counter(GET_CURRENT)]->get_data() != Keywords_t::SEMICOL))
			{
				throw std::logic_error("Invalid input: bad semicols");
			}
			token_counter(INCREMENT);
		}
		else if (lex_array[token_counter(GET_CURRENT)]->get_kind() == Lex_kind_t::KEYWD)
		{
			switch (lex_array[token_counter(GET_CURRENT)]->get_data())
			{
			case Keywords_t::IF:
				stmt = parse_if_while(lex_array, Keywords_t::IF);
				break;
			case Keywords_t::WHILE:
				stmt = parse_if_while(lex_array, Keywords_t::WHILE);
				break;
			case Keywords_t::PRINT:
				stmt = parse_print(lex_array);
				break;
			}
		}
		else
		{
			if((lex_array[token_counter(GET_CURRENT)]->get_kind() == Lex_kind_t::SCOPE) 
			&& (lex_array[token_counter(GET_CURRENT)]->get_data() == Scope_t::RSCOPE))
			{
				return prog_elems;
			}
			else
			{
				throw std::logic_error("Invalid input: bad program building");
			}
			std::cout << "Мы получили тип " << lex_array[token_counter(GET_CURRENT)]->name() << std::endl;
		}

		prog_elems.push_back(stmt);

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
	case Keywords_t::INCREM:
		return "++";
	case Keywords_t::DECREM:
		return "--";
	}

	return nullptr;
}

#endif

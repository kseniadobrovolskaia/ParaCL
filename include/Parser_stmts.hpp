#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H


#include "Stmt_methods.hpp"


std::vector<Statement*> parse_program(std::vector<Lex_t *> &lex_array);
Statement *parse_while(std::vector<Lex_t *> &lex_array);
Statement *parse_print(std::vector<Lex_t *> &lex_array);
Statement *parse_if(std::vector<Lex_t *> &lex_array);


//--------------------------------------------PARSERS---------------------------------------------------


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
						stmt = new Inc_Dec(Stmt);
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


Statement *parse_if(std::vector<Lex_t *> &lex_array)
{
	Lex_t *scop;

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

	scop = lex_array[token_counter(USE_CURRENT)];

	token_counter(INCREMENT);

	std::vector<Statement*> scope = parse_program(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"if\"");
	}

	token_counter(INCREMENT);

	Lex_t *R = new Scope(scope, *scop);
	Lex_t *Else = nullptr;

	if (token_counter(GET_CURRENT) < program_size && is_else(lex_array[token_counter(USE_CURRENT)]))
	{
		token_counter(INCREMENT);

		if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::LSCOPE)
		{
			throw std::logic_error("Invalid input: bad scope in \"else\"");
		}

		scop = lex_array[token_counter(USE_CURRENT)];

		token_counter(INCREMENT);

		std::vector<Statement*> scope = parse_program(lex_array);

		if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
		{
			throw std::logic_error("Invalid input: bad scope in \"else\"");
		}

		token_counter(INCREMENT);
		Else = new Scope(scope, *scop);
	}

	return new If(L, R, Else);
}


Statement *parse_while(std::vector<Lex_t *> &lex_array)
{
	Lex_t *scop;

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

	scop = lex_array[token_counter(USE_CURRENT)];

	token_counter(INCREMENT);

	std::vector<Statement*> scope = parse_program(lex_array);

	if(is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
	{
		throw std::logic_error("Invalid input: bad scope in \"while\"");
	}

	token_counter(INCREMENT);

	Lex_t *R = new Scope(scope, *scop);

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



#endif

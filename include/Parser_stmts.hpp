#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H


#include "Stmt_methods.hpp"


int MAIN = 1;


Scope *parse_program(std::vector<Lex_t *> &lex_array);
Statement *parse_while(std::vector<Lex_t *> &lex_array);
Statement *parse_print(std::vector<Lex_t *> &lex_array);
Statement *parse_if(std::vector<Lex_t *> &lex_array);


//--------------------------------------------PARSERS---------------------------------------------------


Scope *parse_program(std::vector<Lex_t *> &lex_array)
{
	std::vector<Statement*> prog_elems;
	int size = lex_array.size();
	Statement *stmt;
	Lex_t *scop;
	int With_braces = 0;
	int ONE_STMT = 0;

	if (is_scope(lex_array[token_counter(USE_CURRENT)]) == Scope_t::LSCOPE)
	{
		With_braces = 1;
		scop = lex_array[token_counter(USE_CURRENT)];
		token_counter(INCREMENT);
	}
	else if (MAIN)
	{
		MAIN = 0;
		scop = new Lex_t(Lex_kind_t::SCOPE, Scope_t::LSCOPE, lex_array[token_counter(USE_CURRENT)]->get_num());
	}
	else
	{
		scop = new Lex_t(Lex_kind_t::SCOPE, Scope_t::LSCOPE, lex_array[token_counter(USE_CURRENT)]->get_num());
		ONE_STMT = 1;
	}

	while (token_counter(GET_CURRENT) < size)
	{	
		switch (lex_array[token_counter(USE_CURRENT)]->get_kind())
		{
			case Lex_kind_t::BINOP:
			{
				if (is_plus_minus(lex_array[token_counter(USE_CURRENT)]) != BinOp_t::SUB)
				{
					throw_exception("You can not start statement with this operator\n", token_counter(GET_CURRENT));
				}
			}
			case Lex_kind_t::SYMBOL:
			{
				if (is_semicol(lex_array[token_counter(USE_CURRENT)]))
				{
					token_counter(INCREMENT);
					stmt = nullptr;
					break;
				}
				
				if (lex_array[token_counter(USE_CURRENT)]->get_data() == Symbols_t::ELSE)
				{
					throw_exception("You can not use \"else\" without \"if\"\n", token_counter(GET_CURRENT));
				}
			}
			case Lex_kind_t::BRACE:
			case Lex_kind_t::VAR:
			case Lex_kind_t::VALUE:
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
							throw_exception("Invalid assign\n", Stmt->get_num());
						}
						stmt = new Assign(Stmt);
						break;
					}
					default:
					{
						stmt = new Arithmetic(Stmt);
						break;
					}
				}
				
				if(!is_semicol(lex_array[token_counter(USE_CURRENT)]))
				{
					throw_exception("Invalid input: bad semicols\n", token_counter(GET_CURRENT) - 1);
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
				if (With_braces)
				{
					With_braces = 0;
					
					if (is_scope(lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
					{
						throw_exception("Invalid input: bad braces \n", token_counter(GET_CURRENT));
					}
					token_counter(INCREMENT);

					return new Scope(prog_elems, *scop);
				}
				
				throw_exception("Invalid input: bad program building\n", token_counter(GET_CURRENT));
				
			}
		}

		if (stmt)
		{
			prog_elems.push_back(stmt);
		}
		
		if (ONE_STMT)
		{
			ONE_STMT = 0;
			return new Scope(prog_elems, *scop);
		}
		
	}

	if (With_braces)
	{
		throw_exception("Invalid input: bad braces \n", token_counter(GET_CURRENT));
	}

	if (token_counter(GET_CURRENT) >= size)
	{
		MAIN = 1;
	}

	return new Scope(prog_elems, *scop);
}


Statement *parse_if(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	if (is_brace(lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"if\"\n", token_counter(GET_CURRENT));
	}

	Lex_t* L = parse_arithmetic(lex_array);
	Lex_t *R = parse_program(lex_array);
	Lex_t *Else = nullptr;

	if ((token_counter(GET_CURRENT) < static_cast<int>(lex_array.size())) && is_else(lex_array[token_counter(USE_CURRENT)]))
	{
		token_counter(INCREMENT);
		Else = parse_program(lex_array);
	}

	return new If(L, R, Else);
}


Statement *parse_while(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	if (is_brace(lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"while\"\n", token_counter(GET_CURRENT));
	}

	Lex_t* L = parse_arithmetic(lex_array);
	Lex_t *R = parse_program(lex_array);

	return new While(L, R);
}


Statement *parse_print(std::vector<Lex_t *> &lex_array)
{
	token_counter(INCREMENT);

	Lex_t* L = parse_arithmetic(lex_array);

	if(!is_semicol(lex_array[token_counter(USE_CURRENT)]))
	{
		throw_exception("Invalid input: bad semicols in \"print\"\n", token_counter(GET_CURRENT));
	}

	token_counter(INCREMENT);

	return new Print(L);
}



#endif

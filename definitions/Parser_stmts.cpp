#include "Parser_stmts.hpp"


//--------------------------------------------RUN_PROGRAM--------------------------------------------------


void run_program(std::shared_ptr<Lex_t> prog, std::istream & istr, std::ostream & ostr)
{
	prog->calculate(istr, ostr);
}


//--------------------------------------------PARSER_STMTS-------------------------------------------------


std::shared_ptr<Lex_t> parse_scope(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::vector<std::shared_ptr<Statement>> prog_elems;
	int size = lex_array.size();
	std::shared_ptr<Statement> stmt;
	
	std::shared_ptr<Lex_t> scop;
	bool main = 0;
	bool ONE_STMT = 0;


	if (MAIN)
	{
		MAIN = 0;
		main = 1;
		scop = std::make_shared<Lex_t>(Lex_kind_t::SCOPE, Scope_t::LSCOPE, lex_array[token_counter(USE_CURRENT)]->get_num());
	}
	else if (is_scope(*lex_array[token_counter(USE_CURRENT)]) == Scope_t::LSCOPE)
	{
		scop = lex_array[token_counter(USE_CURRENT)];
		token_counter(INCREMENT);
	}
	else
	{
		scop = std::make_shared<Lex_t>(Lex_kind_t::SCOPE, Scope_t::LSCOPE, lex_array[token_counter(USE_CURRENT)]->get_num());
		ONE_STMT = 1;
	}

	while (token_counter(GET_CURRENT) < size)
	{	
		switch (lex_array[token_counter(USE_CURRENT)]->get_kind())
		{
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
				case Statements_t::RETURN:
					stmt = parse_return(lex_array);
					break;
				}
				break;
			}
			case Lex_kind_t::FUNCTION:
			{
				stmt = parse_declaration(lex_array);
				break;
			}
			case Lex_kind_t::BINOP:
			{
				if (is_plus_minus(*lex_array[token_counter(USE_CURRENT)]) != BinOp_t::SUB)
				{
					throw_exception("You can not start statement with this operator\n", token_counter(GET_CURRENT));
				}
			}
			case Lex_kind_t::SYMBOL:
			{
				if (is_semicol(*lex_array[token_counter(USE_CURRENT)]))
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
			case Lex_kind_t::SCOPE:
			case Lex_kind_t::BRACE:
			case Lex_kind_t::VAR:
			case Lex_kind_t::VALUE:
			{
				if (is_scope(*lex_array[token_counter(USE_CURRENT)]) != Scope_t::RSCOPE)
				{
					std::shared_ptr<Lex_t> Stmt = parse_arithmetic(lex_array);

					stmt = std::make_shared<Arithmetic>(Stmt);
					
					if (is_scope(*lex_array[token_counter(GET_CURRENT) - 1]) == Scope_t::RSCOPE)
					{
						break;
					}
					else if (is_semicol(*lex_array[token_counter(USE_CURRENT)]))
					{
						token_counter(INCREMENT);
					}
					else
					{
						throw_exception("Invalid input: bad semicols!\n", token_counter(GET_CURRENT) - 1);
					}
					
					break;
				}
				else
				{

					if (main)
					{
						throw_exception("Invalid input: bad program building\n", token_counter(GET_CURRENT));
					}

					token_counter(INCREMENT);

					return std::make_shared<Scope>(prog_elems, *scop);
				}
			}
			default:
			{
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
			return std::make_shared<Scope>(prog_elems, *scop);
		}
	}

	if (!main)
	{
		throw_exception("I dont understand everything\n", token_counter(GET_CURRENT));
	}
	
	return std::make_shared<Scope>(prog_elems, *scop);
}


std::shared_ptr<Statement> parse_if(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	token_counter(INCREMENT);

	if (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"if\"\n", token_counter(GET_CURRENT));
	}

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);
	std::shared_ptr<Lex_t> R = parse_scope(lex_array);
	std::shared_ptr<Lex_t> Else = nullptr;

	if ((token_counter(GET_CURRENT) < static_cast<int>(lex_array.size())) && is_else(*lex_array[token_counter(USE_CURRENT)]))
	{
		token_counter(INCREMENT);
		Else = parse_scope(lex_array);
	}

	return std::make_shared<If>(L, R, Else);
}


std::shared_ptr<Statement> parse_while(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	token_counter(INCREMENT);

	if (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"while\"\n", token_counter(GET_CURRENT));
	}

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);
	std::shared_ptr<Lex_t> R = parse_scope(lex_array);

	return std::make_shared<While>(L, R);
}


std::shared_ptr<Statement> parse_print(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	token_counter(INCREMENT);

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);

	if(!is_semicol(*lex_array[token_counter(USE_CURRENT)]))
	{
		throw_exception("Invalid input: bad semicols in \"print\"\n", token_counter(GET_CURRENT));
	}

	token_counter(INCREMENT);

	return std::make_shared<Print>(L);
}


std::shared_ptr<Statement> parse_return(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	token_counter(INCREMENT);

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);

	if(!is_semicol(*lex_array[token_counter(USE_CURRENT)]))
	{
		throw_exception("Invalid input: bad semicols in \"return\"\n", token_counter(GET_CURRENT));
	}

	token_counter(INCREMENT);

	return std::make_shared<Return>(L);
}


std::shared_ptr<Statement> parse_declaration(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	std::shared_ptr<Statement> stmt;
	std::shared_ptr<Lex_t> func = lex_array[token_counter(USE_CURRENT)];

	token_counter(INCREMENT);
	token_counter(INCREMENT);
	token_counter(INCREMENT);

	if (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in function declaration\n", token_counter(GET_CURRENT));
	}
	token_counter(INCREMENT);

	bool first_arg = 1;
	std::shared_ptr<Lex_t> arg;
	std::vector<std::shared_ptr<Lex_t>> vars_in_func;

	while (is_brace(*lex_array[token_counter(USE_CURRENT)]) != Brace_t::RBRACE)
	{
		if (first_arg)
		{
			first_arg = 0;
		}
		else
		{
			if (!is_comma(*lex_array[token_counter(USE_CURRENT)]))
			{
				throw_exception("Need comma between function args\n", token_counter(GET_CURRENT));
			}
			token_counter(INCREMENT);
		}
		arg = lex_array[token_counter(USE_CURRENT)];

		if (arg->get_kind() != Lex_kind_t::VAR)
		{
			throw_exception("Args in function declaration must be variables\n", token_counter(GET_CURRENT));
		}
		token_counter(INCREMENT);
		
		vars_in_func.push_back(arg);
	}
	token_counter(INCREMENT);

	stmt = std::make_shared<Declaration>(func, vars_in_func);
	static_cast<Declaration*>(stmt.get())->set_decl(stmt);

	if (is_colon(*lex_array[token_counter(USE_CURRENT)]))
	{
		token_counter(INCREMENT);

		if (lex_array[token_counter(USE_CURRENT)]->get_kind() == Lex_kind_t::VAR)
		{
			std::string function_name = lex_array[token_counter(USE_CURRENT)]->short_name();
			FUNCTIONS[function_name] = stmt;
		}
		else
		{
			throw_exception("This should be the global function name\n", token_counter(GET_CURRENT));
		}

		token_counter(INCREMENT);
	}

	std::shared_ptr<Lex_t> scope = parse_scope(lex_array);

	static_cast<Declaration*>(stmt.get())->add_scope(scope);

	CURR_SCOPE->init_func(func->short_name(), stmt);

	return stmt;
}

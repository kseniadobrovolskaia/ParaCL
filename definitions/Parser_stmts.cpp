#include "Parser_stmts.hpp"


//--------------------------------------------PARSER_STMTS-------------------------------------------------


std::shared_ptr<Lex_t> parse_scope(std::shared_ptr<Lex_array_t> lex_array, bool reset)
{
	static bool MAIN = 1;

	if (reset)
	{
		MAIN = 1;
		return nullptr;
	}

	bool main = 0;
	bool ONE_STMT = 0;

	std::vector<std::shared_ptr<Statement>> prog_elems;
	int size = lex_array->size();
	std::shared_ptr<Statement> stmt;
	
	std::shared_ptr<Lex_t> scop;


	if (MAIN)
	{
		MAIN = 0;
		main = 1;
		scop = std::make_shared<Lex_t>(Lex_kind_t::SCOPE, Scope_t::LSCOPE, (lex_array->get_curr_lex())->get_str(), (lex_array->get_curr_lex())->get_num());
	}
	else if (lex_array->is_scope() == Scope_t::LSCOPE)
	{
		scop = (lex_array->get_curr_lex());
		lex_array->inc_lex();
	}
	else
	{
		scop = std::make_shared<Lex_t>(Lex_kind_t::SCOPE, Scope_t::LSCOPE, (lex_array->get_curr_lex())->get_str(), (lex_array->get_curr_lex())->get_num());
		ONE_STMT = 1;
	}

	int curr_num;
	
	while ((curr_num = lex_array->get_num_curr_lex()) < size)
	{	
		std::shared_ptr<Lex_t> Curr = lex_array->get_curr_lex();

		switch (Curr->get_kind())
		{
			case Lex_kind_t::STMT:
			{
				switch (Curr->get_data())
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
			case Lex_kind_t::BINOP:
			{
				if (lex_array->is_plus_minus() != BinOp_t::SUB)
				{
					throw_exception("You can not start statement with this operator\n", lex_array->get_num_curr_lex());
				}
			}
			case Lex_kind_t::SYMBOL:
			{
				if (lex_array->is_semicol())
				{
					lex_array->inc_lex();
					stmt = nullptr;
					break;
				}
				
				if ((lex_array->get_curr_lex())->get_data() == Symbols_t::ELSE)
				{
					throw_exception("You can not use \"else\" without \"if\"\n", lex_array->get_num_curr_lex());
				}
			}
			case Lex_kind_t::FUNCTION:
			{
				if (((lex_array->get_next_lex())->get_kind() == Lex_kind_t::STMT)
				&&  ((lex_array->get_next_lex())->get_data() == Statements_t::ASSIGN))
				{
					stmt = parse_declaration(lex_array);
					break;
				}
			}
			case Lex_kind_t::SCOPE:
			case Lex_kind_t::BRACE:
			case Lex_kind_t::VAR:
			case Lex_kind_t::VALUE:
			{
				if (lex_array->is_scope() != Scope_t::RSCOPE)
				{
					std::shared_ptr<Lex_t> Stmt = parse_arithmetic(lex_array);

					stmt = std::make_shared<Arithmetic>(Stmt);

					int cur_num = lex_array->get_num_curr_lex();
					lex_array->set_curr_lex(cur_num - 1);
					int is_scp = lex_array->is_scope();
					lex_array->set_curr_lex(cur_num);
					
					if (is_scp == Scope_t::RSCOPE)
					{
						break;
					}
					else if (lex_array->is_semicol())
					{
						lex_array->inc_lex();
					}
					else
					{
						throw_exception("Invalid input: bad semicols!\n", lex_array->get_num_curr_lex() - 1);
					}
					
					break;
				}
				else
				{

					if (main)
					{
						throw_exception("Invalid input: bad program building\n", lex_array->get_num_curr_lex());
					}

					lex_array->inc_lex();

					return std::make_shared<Scope>(prog_elems, *scop);
				}
			}
			default:
			{
				throw_exception("Invalid input: bad program building\n", lex_array->get_num_curr_lex());
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
		throw_exception("I dont understand everything\n", lex_array->get_num_curr_lex());
	}
	
	return std::make_shared<Scope>(prog_elems, *scop);
}


std::shared_ptr<Statement> parse_if(std::shared_ptr<Lex_array_t> lex_array)
{
	lex_array->inc_lex();

	if (lex_array->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"if\"\n", lex_array->get_num_curr_lex());
	}

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);
	std::shared_ptr<Lex_t> R = parse_scope(lex_array);
	std::shared_ptr<Lex_t> Else = nullptr;

	if ((lex_array->get_num_curr_lex() < lex_array->size()) && lex_array->is_else())
	{
		lex_array->inc_lex();
		Else = parse_scope(lex_array);
	}

	return std::make_shared<If>(L, R, Else);
}


std::shared_ptr<Statement> parse_while(std::shared_ptr<Lex_array_t> lex_array)
{
	lex_array->inc_lex();

	if (lex_array->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"while\"\n", lex_array->get_num_curr_lex());
	}

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);
	std::shared_ptr<Lex_t> R = parse_scope(lex_array);

	return std::make_shared<While>(L, R);
}


std::shared_ptr<Statement> parse_print(std::shared_ptr<Lex_array_t> lex_array)
{
	lex_array->inc_lex();

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);

	if(!lex_array->is_semicol())
	{
		throw_exception("Invalid input: bad semicols in \"print\"\n", lex_array->get_num_curr_lex());
	}

	lex_array->inc_lex();

	return std::make_shared<Print>(L);
}


std::shared_ptr<Statement> parse_return(std::shared_ptr<Lex_array_t> lex_array)
{
	lex_array->inc_lex();

	std::shared_ptr<Lex_t> L = parse_arithmetic(lex_array);

	if(!lex_array->is_semicol())
	{
		throw_exception("Invalid input: bad semicols in \"return\"\n", lex_array->get_num_curr_lex());
	}

	lex_array->inc_lex();

	return std::make_shared<Return>(L);
}


std::shared_ptr<Statement> parse_declaration(std::shared_ptr<Lex_array_t> lex_array)
{
	std::shared_ptr<Statement> stmt;
	std::shared_ptr<Lex_t> func = lex_array->get_curr_lex();

	lex_array->inc_lex();
	lex_array->inc_lex();
	lex_array->inc_lex();

	if (lex_array->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in function declaration\n", lex_array->get_num_curr_lex());
	}
	lex_array->inc_lex();

	bool first_arg = 1;
	std::shared_ptr<Lex_t> arg;
	std::vector<std::shared_ptr<Lex_t>> vars_in_func;

	while (lex_array->is_brace() != Brace_t::RBRACE)
	{
		if (first_arg)
		{
			first_arg = 0;
		}
		else
		{
			if (!lex_array->is_comma())
			{
				throw_exception("Need comma between function args\n", lex_array->get_num_curr_lex());
			}
			lex_array->inc_lex();
		}
		arg = lex_array->get_curr_lex();

		if (arg->get_kind() != Lex_kind_t::VAR)
		{
			throw_exception("Args in function declaration must be variables\n", lex_array->get_num_curr_lex());
		}
		lex_array->inc_lex();
		
		vars_in_func.push_back(arg);
	}
	lex_array->inc_lex();

	stmt = std::make_shared<Declaration>(func, vars_in_func);
	static_cast<Declaration*>(stmt.get())->set_decl(std::weak_ptr(stmt));

	if (lex_array->is_colon())
	{
		lex_array->inc_lex();

		if ((lex_array->get_curr_lex())->get_kind() == Lex_kind_t::FUNCTION)
		{
			std::string function_name = (lex_array->get_curr_lex())->short_name();
			AST_creator::FUNCTIONS[function_name] = stmt;
		}
		else
		{
			throw_exception("This should be the global function name\n", lex_array->get_num_curr_lex());
		}

		lex_array->inc_lex();
	}

	std::shared_ptr<Lex_t> scope = parse_scope(lex_array);

	static_cast<Declaration*>(stmt.get())->add_scope(scope);

	AST_creator::CURR_SCOPE->init_func(func->short_name(), stmt);

	return stmt;
}

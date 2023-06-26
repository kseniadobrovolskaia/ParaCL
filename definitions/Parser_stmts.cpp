#include "Parser.hpp"



//--------------------------------------------PARSER_STMTS-----------------------------------------------------------


std::shared_ptr<Lex_t> Parser::parse_scope(bool reset)
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
	int size;

	if (!(size = lex_array_->size()))
	{
		return nullptr;
	}

	std::shared_ptr<Statement> stmt;
	
	std::shared_ptr<Lex_t> scop;


	if (MAIN)
	{
		MAIN = 0;
		main = 1;
		scop = std::make_shared<Lex_t>(Lex_kind_t::SCOPE, Scope_t::LSCOPE, 
			  (lex_array_->get_curr_lex())->get_str(), (lex_array_->get_curr_lex())->get_num(), "SCOPE");
	}
	else if (lex_array_->is_scope() == Scope_t::LSCOPE)
	{
		scop = lex_array_->get_curr_lex();
		lex_array_->inc_lex();
	}
	else
	{
		scop = std::make_shared<Lex_t>(Lex_kind_t::SCOPE, Scope_t::LSCOPE, 
			  (lex_array_->get_curr_lex())->get_str(), (lex_array_->get_curr_lex())->get_num(), "SCOPE");
		ONE_STMT = 1;
	}

	int curr_num;
	
	while ((curr_num = lex_array_->get_num_curr_lex()) < size)
	{	
		std::shared_ptr<Lex_t> Curr = lex_array_->get_curr_lex();

		switch (Curr->get_kind())
		{
			case Lex_kind_t::STMT:
			{
				switch (Curr->get_data())
				{
				case Statements_t::IF:
					stmt = parse_if();
					break;
				case Statements_t::WHILE:
					stmt = parse_while();
					break;
				case Statements_t::PRINT:
					stmt = parse_print();
					break;
				case Statements_t::RETURN:
					stmt = parse_return();
					break;
				}
				break;
			}
			case Lex_kind_t::BINOP:
			{
				if (lex_array_->is_plus_minus() != BinOp_t::SUB)
				{
					throw_exception("You can not start statement with this operator\n", lex_array_->get_num_curr_lex());
				}
			}
			case Lex_kind_t::SYMBOL:
			{
				if (lex_array_->is_semicol())
				{
					lex_array_->inc_lex();
					stmt = nullptr;
					break;
				}
				
				if ((lex_array_->get_curr_lex())->get_data() == Symbols_t::ELSE)
				{
					throw_exception("You can not use \"else\" without \"if\"\n", lex_array_->get_num_curr_lex());
				}
			}
			case Lex_kind_t::FUNCTION:
			{
				if (((lex_array_->get_next_lex())->get_kind() == Lex_kind_t::STMT)
				&&  ((lex_array_->get_next_lex())->get_data() == Statements_t::ASSIGN))
				{
					stmt = parse_declaration();
					break;
				}
			}
			case Lex_kind_t::SCOPE:
			case Lex_kind_t::BRACE:
			case Lex_kind_t::VAR:
			case Lex_kind_t::VALUE:
			{
				if (lex_array_->is_scope() != Scope_t::RSCOPE)
				{
					std::shared_ptr<Lex_t> Stmt = parse_arithmetic();

					stmt = std::make_shared<Arithmetic>(Stmt);

					int cur_num = lex_array_->get_num_curr_lex();
					lex_array_->set_curr_lex(cur_num - 1);
					int is_scp = lex_array_->is_scope();
					lex_array_->set_curr_lex(cur_num);

					if (is_scp == Scope_t::RSCOPE)
					{
						break;
					}
					else if (lex_array_->is_semicol())
					{
						lex_array_->inc_lex();
					}
					else
					{
						throw_exception("Invalid input: bad semicols!\n", lex_array_->get_num_curr_lex() - 1);
					}
					
					break;
				}
				else
				{
					if (main)
					{
						throw_exception("Invalid input: bad program building\n", lex_array_->get_num_curr_lex());
					}

					lex_array_->inc_lex();
					
					return std::make_shared<Scope>(prog_elems, *scop);
				}
			}
			default:
			{
				throw_exception("Invalid input: bad program building\n", lex_array_->get_num_curr_lex());
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
		throw_exception("Forgot to close scope \"}\"\n", lex_array_->get_num_curr_lex());
	}
	
	return std::make_shared<Scope>(prog_elems, *scop);
}


std::shared_ptr<Statement> Parser::parse_if()
{
	lex_array_->inc_lex();

	if (lex_array_->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"if\"\n", lex_array_->get_num_curr_lex());
	}

	std::shared_ptr<Lex_t> L = parse_arithmetic();
	std::shared_ptr<Lex_t> R = parse_scope();
	std::shared_ptr<Lex_t> Else = nullptr;

	if ((lex_array_->get_num_curr_lex() < lex_array_->size()) && lex_array_->is_else())
	{
		lex_array_->inc_lex();
		Else = parse_scope();
	}

	return std::make_shared<If>(L, R, Else);
}


std::shared_ptr<Statement> Parser::parse_while()
{
	lex_array_->inc_lex();

	if (lex_array_->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in \"while\"\n", lex_array_->get_num_curr_lex());
	}

	std::shared_ptr<Lex_t> L = parse_arithmetic();
	std::shared_ptr<Lex_t> R = parse_scope();

	return std::make_shared<While>(L, R);
}


std::shared_ptr<Statement> Parser::parse_print()
{
	lex_array_->inc_lex();

	std::shared_ptr<Lex_t> L = parse_arithmetic();

	if(!lex_array_->is_semicol())
	{
		throw_exception("Invalid input: bad semicols in \"print\"\n", lex_array_->get_num_curr_lex());
	}

	lex_array_->inc_lex();

	return std::make_shared<Print>(L);
}


std::shared_ptr<Statement> Parser::parse_return()
{
	if (!IN_FUNCTION)
	{
		throw_exception("You can return only from functions\n", lex_array_->get_num_curr_lex());
	}

	lex_array_->inc_lex();

	std::shared_ptr<Lex_t> L = parse_arithmetic();

	if(!lex_array_->is_semicol())
	{
		throw_exception("Invalid input: bad semicols in \"return\"\n", lex_array_->get_num_curr_lex());
	}

	lex_array_->inc_lex();

	return std::make_shared<Return>(L);
}


std::shared_ptr<Statement> Parser::parse_declaration()
{
	std::string global_name;

	std::shared_ptr<Statement> stmt;
	std::shared_ptr<Lex_t> func = lex_array_->get_curr_lex();

	lex_array_->inc_lex();
	lex_array_->inc_lex();
	lex_array_->inc_lex();

	if (lex_array_->is_brace() != Brace_t::LBRACE)
	{
		throw_exception("Syntax error in function declaration\n", lex_array_->get_num_curr_lex());
	}
	lex_array_->inc_lex();

	bool first_arg = 1;
	std::shared_ptr<Lex_t> arg;
	std::vector<std::shared_ptr<Lex_t>> vars_in_func;

	while (lex_array_->is_brace() != Brace_t::RBRACE)
	{
		if (first_arg)
		{
			first_arg = 0;
		}
		else
		{
			if (!lex_array_->is_comma())
			{
				throw_exception("Need comma between function args\n", lex_array_->get_num_curr_lex());
			}
			lex_array_->inc_lex();
		}
		arg = lex_array_->get_curr_lex();

		if (arg->get_kind() != Lex_kind_t::VAR)
		{
			throw_exception("Args in function declaration must be variables\n", lex_array_->get_num_curr_lex());
		}
		lex_array_->inc_lex();
		
		vars_in_func.push_back(arg);
	}
	lex_array_->inc_lex();

	if (lex_array_->is_colon())
	{
		lex_array_->inc_lex();

		if ((lex_array_->get_curr_lex())->get_kind() == Lex_kind_t::FUNCTION)
		{
			global_name = (lex_array_->get_curr_lex())->short_name();
		}
		else
		{
			throw_exception("This should be the global function name\n", lex_array_->get_num_curr_lex());
		}

		lex_array_->inc_lex();
	}

	stmt = std::make_shared<Declaration>(func, global_name, vars_in_func);
	std::static_pointer_cast<Declaration>(stmt)->set_decl(std::weak_ptr(stmt));

	IN_FUNCTION++;

	std::shared_ptr<Lex_t> scope = parse_scope();

	IN_FUNCTION--;

	std::static_pointer_cast<Declaration>(stmt)->add_scope(scope);

	if (global_name.size() > 0)
	{
		FUNCTIONS[global_name] = stmt;
	}

	return stmt;
}

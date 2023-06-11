#include "Lexer.hpp"



//-----------------------------------------TOKEN_COUNTER---------------------------------------------------


int token_counter(Move move)
{
	static int curr_lex = 0;

	switch (move)
	{
	case Move::INCREMENT:
		curr_lex++;
	case Move::GET_CURRENT:
		return curr_lex;
	case Move::RESET:
		curr_lex = 0;
		return 0;
	case Move::USE_CURRENT:
		if (curr_lex >= static_cast<int>(lex_array.size()))
		{
			throw_exception("Syntax error in last line\n", token_counter(GET_CURRENT) - 1);
		}
		return curr_lex;
	default:
		throw std::logic_error("Something strange in function \"token_counter\"");
		return 1;
	}
}


//-----------------------------------------PUSH_IN_LEX_ARRAY---------------------------------------------------


void push_binop(std::vector<std::shared_ptr<Lex_t>> &lex_array, BinOp_t binop, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::BINOP, binop, num_str));
}

void push_unop(std::vector<std::shared_ptr<Lex_t>> &lex_array, Statements_t unop, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::UNOP, unop, num_str));
}

void push_brace(std::vector<std::shared_ptr<Lex_t>> &lex_array, Brace_t brace, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::BRACE, brace, num_str));
}

void push_scope(std::vector<std::shared_ptr<Lex_t>> &lex_array, Scope_t scope, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::SCOPE, scope, num_str));
}

void push_stmt(std::vector<std::shared_ptr<Lex_t>> &lex_array, Statements_t kw, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::STMT, kw, num_str));
}

void push_var(std::vector<std::shared_ptr<Lex_t>> &lex_array, int num_var, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::VAR, num_var, num_str));
}

void push_function(std::vector<std::shared_ptr<Lex_t>> &lex_array, int num_func, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::FUNCTION, num_func, num_str));
}

void push_value(std::vector<std::shared_ptr<Lex_t>> &lex_array, int value, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::VALUE, value, num_str));
}

void push_compop(std::vector<std::shared_ptr<Lex_t>> &lex_array, CompOp_t compop, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::COMPOP, compop, num_str));
}

void push_symbol(std::vector<std::shared_ptr<Lex_t>> &lex_array, Symbols_t symbol, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::SYMBOL, symbol, num_str));
}

void push_bad_symbol(std::vector<std::shared_ptr<Lex_t>> &lex_array, int symbol, int num_str)
{
	lex_array.push_back(std::make_shared<Lex_t>(Lex_kind_t::BAD_SYMBOL, symbol, num_str));
}


//---------------------------------------------LEX_STRING---------------------------------------------------


void handler(int signo)
{
	EoF = 1;
}


std::vector<std::shared_ptr<Lex_t>> lex_string(std::istream &istr)
{
	int num_var = 0, num_funcs = 0, num_str = 1;
	char elem, prev = '\0';
	std::string word;

	signal(SIGINT, handler);

	istr >> std::noskipws;
	
	while (istr >> elem)
	{	
		if (EoF)
		{
			break;
		}
		
		switch (elem)
		{
		case '\n':
			num_str++;
			break;
		case ';':
			push_symbol(lex_array, SEMICOL, num_str);
			break;
		case ':':
			push_symbol(lex_array, COLON, num_str);
			break;
		case ',':
			push_symbol(lex_array, COMMA, num_str);
			break;
		case '=':
			switch(prev)
			{
			case '!':
				lex_array.pop_back();
				push_compop(lex_array, NOT_EQUAL, num_str);
				break;				
			case '=':
				lex_array.pop_back();
				push_compop(lex_array, EQUAL, num_str);
				break;
			case '<':
				lex_array.pop_back();
				push_compop(lex_array, LESorEQ, num_str);
				break;
			case '>':
				lex_array.pop_back();
				push_compop(lex_array, GRorEQ, num_str);
				break;
			default:
				push_stmt(lex_array, ASSIGN, num_str);
			}
			break;
		case '<':
			push_compop(lex_array, LESS, num_str);
			break;
		case '>':
			push_compop(lex_array, GREATER, num_str);
			break;
		case '!':
			push_symbol(lex_array, NEGATION, num_str);
			break;
		case '+':
		{
			if (prev == '+')
			{
				lex_array.pop_back();
				push_unop(lex_array, INC, num_str);
			}
			else
			{
				push_binop(lex_array, ADD, num_str);
			}

			break;
		}
		case '-':
			if (prev == '-')
			{
				lex_array.pop_back();
				push_unop(lex_array, DEC, num_str);
				break;
			}
			else
			{
				push_binop(lex_array, SUB, num_str);
			}

			break;
		case '*':
			if(prev == '/')
			{
				lex_array.pop_back();
				while (elem != '/' || prev != '*')
				{
					prev = elem;
					istr >> elem;
					if (elem == '\n')
					{
						num_str++;
					}
				}
			}
			else
			{
				push_binop(lex_array, MULT, num_str);
			}
			break;
		case '/':
			if(prev == '/')
			{
				lex_array.pop_back();
				while (elem != '\n')
				{
					istr >> elem;
				}
				num_str++;
			}
			else
			{
				push_binop(lex_array, DIV, num_str);
			}
			break;
		case '(':
			push_brace(lex_array, LBRACE, num_str);
			break;
		case ')':
			push_brace(lex_array, RBRACE, num_str);
			break;
		case '{':
			push_scope(lex_array, LSCOPE, num_str);
			break;
		case '}':
			push_scope(lex_array, RSCOPE, num_str);
			break;
		case '?':
			push_symbol(lex_array, SCAN, num_str);
			break;
		default:
		{
			if (std::isdigit(elem))
			{
				while (std::isdigit(elem))
				{
					word += elem;
					prev = elem;
					istr >> elem;
				}

				istr.putback(elem);

				push_value(lex_array, std::stoi(word), num_str);
				word.clear();
				continue;
			}
			else if (std::isalpha(elem) || elem == '_')
			{
				while (std::isalpha(elem) || std::isdigit(elem) || elem == '_')
				{
					word += elem;
					prev = elem;
					istr >> elem;
				}

				istr.putback(elem);

				if (word == "if")
				{
					push_stmt(lex_array, IF, num_str);
				}
				else if (word == "else")
				{
					push_symbol(lex_array, ELSE, num_str);
				}
				else if (word == "return")
				{
					push_stmt(lex_array, RETURN, num_str);
				}
				else if (word == "while")
				{
					push_stmt(lex_array, WHILE, num_str);
				}
				else if (word == "print")
				{
					push_stmt(lex_array, PRINT, num_str);
				}
				else if (word == "func")
				{
					if (lex_array.size() < 2)
					{
						push_stmt(lex_array, FUNC, num_str);
						throw_exception("Bad function declaration\n", lex_array.size() - 1);
					}
					if (lex_array.back()->get_kind() != Lex_kind_t::STMT 
					 || lex_array.back()->get_data() != Statements_t::ASSIGN)
					{
						push_stmt(lex_array, FUNC, num_str);
						throw_exception("Bad function declaration\n", lex_array.size() - 1);
					}
					std::shared_ptr<Lex_t> ass = lex_array.back();
					lex_array.pop_back();

					if (lex_array.back()->get_kind() != Lex_kind_t::VAR)
					{
						push_stmt(lex_array, ASSIGN, ass->get_str());
						push_stmt(lex_array, FUNC, num_str);
						throw_exception("Bad function declaration\n", lex_array.size() - 1);
					}
					
					std::shared_ptr<Lex_t> func = lex_array.back();
					lex_array.pop_back();

					if (func->get_data() == (num_var - 1))
					{
						funcs.push_back(vars[num_var - 1]);
						num_funcs++;
						num_var--;
						vars.pop_back();
					}

					push_function(lex_array, num_funcs - 1, func->get_str());
					push_stmt(lex_array, ASSIGN, ass->get_str());
					push_stmt(lex_array, FUNC, num_str);
				}
				else
				{
					std::vector<std::string>::iterator itr = std::find(vars.begin(), vars.end(), word);
					if (itr == vars.end())
					{
						push_var(lex_array, num_var, num_str);
						vars.push_back(word);
						num_var++;					
					}
					else
					{
						push_var(lex_array, std::distance(vars.begin(), itr), num_str);
					}
				}

				word.clear();
				continue;
			}
			else if (!isspace(elem))
			{
				istr.putback(elem);

				push_bad_symbol(lex_array, static_cast<int>(elem), num_str);

				std::string mess, line;

				mess += "No such symbol exists\n";
		
				throw_exception(mess, lex_array.size() - 1);
			}
		}
		}

	    prev = elem;
	}

	return lex_array;
}


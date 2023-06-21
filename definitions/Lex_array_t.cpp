#include "Lex_array.hpp"




//-----------------------------------------CONSTRUCTOR---------------------------------------------------


void handler(int signo)
{
	Lex_array_t::EoF_ = 1;
}


Lex_array_t::Lex_array_t(std::istream &istr)
{
	/// That there is nothing from the previous lexical analysis
	Lex_t::vars_table().clear();
	Lex_t::funcs_table().clear();
	Lex_array_t::EoF_ = 0;


	int num_var = 0, num_funcs = 0;
	char elem, prev = '\0';
	std::string word;

	signal(SIGINT, handler);

	istr >> std::noskipws;
	
	while (istr >> elem)
	{	
		if (EoF_)
		{
			break;
		}
		
		switch (elem)
		{
		case '\n':
			num_str_++;
			break;
		case ';':
			push_symbol(SEMICOL);
			break;
		case ':':
			push_symbol(COLON);
			break;
		case ',':
			push_symbol(COMMA);
			break;
		case '=':
			switch(prev)
			{
			case '!':
				lex_array_.pop_back();
				push_compop(NOT_EQUAL);
				break;				
			case '=':
				lex_array_.pop_back();
				push_compop(EQUAL);
				break;
			case '<':
				lex_array_.pop_back();
				push_compop(LESorEQ);
				break;
			case '>':
				lex_array_.pop_back();
				push_compop(GRorEQ);
				break;
			default:
				push_stmt(ASSIGN);
			}
			break;
		case '<':
			push_compop(LESS);
			break;
		case '>':
			push_compop(GREATER);
			break;
		case '!':
			push_symbol(NEGATION);
			break;
		case '+':
		{
			if (prev == '+')
			{
				lex_array_.pop_back();
				push_unop(INC);
			}
			else
			{
				push_binop(ADD);
			}

			break;
		}
		case '-':
			if (prev == '-')
			{
				lex_array_.pop_back();
				push_unop(DEC);
				break;
			}
			else
			{
				push_binop(SUB);
			}

			break;
		case '*':
			if(prev == '/')
			{
				lex_array_.pop_back();
				while (elem != '/' || prev != '*')
				{
					prev = elem;
					istr >> elem;
					if (elem == '\n')
					{
						num_str_++;
					}
				}
			}
			else
			{
				push_binop(MULT);
			}
			break;
		case '/':
			if(prev == '/')
			{
				lex_array_.pop_back();
				while (elem != '\n')
				{
					istr >> elem;
				}
				num_str_++;
			}
			else
			{
				push_binop(DIV);
			}
			break;
		case '(':
			if (lex_array_.back()->get_kind() == Lex_kind_t::VAR)
			{
				std::shared_ptr<Lex_t> func = lex_array_.back();
				lex_array_.pop_back();

				if (func->get_data() == (num_var - 1))
				{
					Lex_t::funcs_table().push_back(Lex_t::vars_table()[num_var - 1]);
					num_funcs++;
					num_var--;
					Lex_t::vars_table().pop_back();
				}

				push_function(num_funcs - 1);
			}	
			push_brace(LBRACE);
			break;
		case ')':
			push_brace(RBRACE);
			break;
		case '{':
			if (lex_array_.size() > 0 && (lex_array_.back()->get_kind() == Lex_kind_t::VAR))
			{
				std::shared_ptr<Lex_t> func = lex_array_.back();
				lex_array_.pop_back();

				if (func->get_data() == (num_var - 1))
				{
					Lex_t::funcs_table().push_back(Lex_t::vars_table()[num_var - 1]);
					num_funcs++;
					num_var--;
					Lex_t::vars_table().pop_back();
				}

				push_function(num_funcs - 1);
			}
			push_scope(LSCOPE);
			break;
		case '}':
			push_scope(RSCOPE);
			break;
		case '?':
			push_symbol(SCAN);
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

				push_value(std::stoi(word));
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
					push_stmt(IF);
				}
				else if (word == "else")
				{
					push_symbol(ELSE);
				}
				else if (word == "return")
				{
					push_stmt(RETURN);
				}
				else if (word == "while")
				{
					push_stmt(WHILE);
				}
				else if (word == "print")
				{
					push_stmt(PRINT);
				}
				else if (word == "func")
				{
					if (lex_array_.size() < 2)
					{
						throw_exception("Set lex_array", 0, lex_array_); //This need to set lex_array in function "throw_exception"
						throw_exception("Bad function declaration\n", lex_array_.size() - 1);
					}

					if ((lex_array_.back()->get_kind() != Lex_kind_t::STMT)
					 || (lex_array_.back()->get_data() != Statements_t::ASSIGN))
					{
						push_stmt(FUNC);

						throw_exception("Set lex_array", 0, lex_array_);
						throw_exception("Bad function declaration\n", lex_array_.size() - 1);
					}
					std::shared_ptr<Lex_t> ass = lex_array_.back();
					lex_array_.pop_back();

					if (lex_array_.back()->get_kind() != Lex_kind_t::VAR)
					{
						push_stmt(ASSIGN);
						push_stmt(FUNC);

						throw_exception("Set lex_array", 0, lex_array_);
						throw_exception("Bad function declaration\n", lex_array_.size() - 1);
					}
					
					std::shared_ptr<Lex_t> func = lex_array_.back();
					lex_array_.pop_back();

					if (func->get_data() == (num_var - 1))
					{
						Lex_t::funcs_table().push_back(Lex_t::vars_table()[num_var - 1]);
						num_funcs++;
						num_var--;
						Lex_t::vars_table().pop_back();
					}

					push_function(num_funcs - 1);
					push_stmt(ASSIGN);
					push_stmt(FUNC);
				}
				else
				{
					std::vector<std::string>::iterator itr = std::find(Lex_t::vars_table().begin(), Lex_t::vars_table().end(), word);
					if (itr == Lex_t::vars_table().end())
					{
						push_var(num_var);
						Lex_t::vars_table().push_back(word);
						num_var++;					
					}
					else
					{
						push_var(std::distance(Lex_t::vars_table().begin(), itr));
					}
				}

				word.clear();
				continue;
			}
			else if (!isspace(elem))
			{
				istr.putback(elem);

				push_bad_symbol(static_cast<int>(elem));

				std::string mess, line;

				mess += "No such symbol exists\n";

				throw_exception("Set lex_array", 0, lex_array_);
				throw_exception(mess, lex_array_.size() - 1);
			}
		}
		}

	    prev = elem;
	}

	throw_exception("Set lex_array", 0, lex_array_); //This need to set static lex_array in function "throw_exception"

}


//-----------------------------------------PUSH_IN_LEX_ARRAY---------------------------------------------------


void Lex_array_t::push_binop(BinOp_t binop)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::BINOP, binop, num_str_, lex_array_.size()));
}

void Lex_array_t::push_unop(Statements_t unop)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::UNOP, unop, num_str_, lex_array_.size()));
}

void Lex_array_t::push_brace(Brace_t brace)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::BRACE, brace, num_str_, lex_array_.size()));
}

void Lex_array_t::push_scope(Scope_t scope)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::SCOPE, scope, num_str_, lex_array_.size()));
}

void Lex_array_t::push_stmt(Statements_t kw)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::STMT, kw, num_str_, lex_array_.size()));
}

void Lex_array_t::push_var(int num_var)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::VAR, num_var, num_str_, lex_array_.size()));
}

void Lex_array_t::push_function(int num_func)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::FUNCTION, num_func, num_str_, lex_array_.size()));
}

void Lex_array_t::push_value(int value)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::VALUE, value, num_str_, lex_array_.size()));
}

void Lex_array_t::push_compop(CompOp_t compop)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::COMPOP, compop, num_str_, lex_array_.size()));
}

void Lex_array_t::push_symbol(Symbols_t symbol)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::SYMBOL, symbol, num_str_, lex_array_.size()));
}

void Lex_array_t::push_bad_symbol(int symbol)
{
	lex_array_.push_back(std::make_shared<Lex_t>(Lex_kind_t::BAD_SYMBOL, symbol, num_str_, lex_array_.size()));
}


//-----------------------------------------------ISERS-----------------------------------------------------


int Lex_array_t::is_plus_minus() const
{
	Lex_t &node = *lex_array_[curr_lex_];
	
	if ((node.get_kind() != Lex_kind_t::BINOP)
	|| (node.get_data() > 1))
	{
		return -1;
	}
	return node.get_data();
}


int Lex_array_t::is_mul_div() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	if ((node.get_kind() != Lex_kind_t::BINOP)
	|| (node.get_data() < 2))
	{
		return -1;
	}
	return node.get_data();
}


int Lex_array_t::is_compop() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	if (node.get_kind() != Lex_kind_t::COMPOP)
	{
		return -1;
	}
	return node.get_data();
}


int Lex_array_t::is_brace() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	if (node.get_kind() != Lex_kind_t::BRACE)
	{
		return -1;
	}
	return node.get_data();
}


int Lex_array_t::is_unop() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	if (node.get_kind() != Lex_kind_t::UNOP)
	{
		return -1;
	}
	return node.get_data();
}


int Lex_array_t::is_scope() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	if (node.get_kind() != Lex_kind_t::SCOPE)
	{
		return -1;
	}
	return node.get_data();
}


bool Lex_array_t::is_scan() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::SYMBOL) 
		&& (node.get_data() == Symbols_t::SCAN);
}


bool Lex_array_t::is_negation() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::NEGATION);
}


bool Lex_array_t::is_binop() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::BINOP)
	   	|| (node.get_kind() == Lex_kind_t::COMPOP);
}


bool Lex_array_t::is_assign() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::STMT)
	  	&& (node.get_data() == Statements_t::ASSIGN);
}


bool Lex_array_t::is_semicol() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::SEMICOL);
}


bool Lex_array_t::is_return() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::STMT)
		&& (node.get_data() == Statements_t::RETURN);
}


bool Lex_array_t::is_comma() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::COMMA);
}


bool Lex_array_t::is_colon() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::COLON);
}


bool Lex_array_t::is_else() const
{
	Lex_t &node = *lex_array_[curr_lex_];

	return (node.get_kind() == Lex_kind_t::SYMBOL)
		&& (node.get_data() == Symbols_t::ELSE);
}


//-----------------------------------------OTHER_METHODS---------------------------------------------------


void Lex_array_t::print() const
{
	for (auto &&elem : lex_array_)
	{
		std::cout << elem->name() << " ";
	}
	std::cout << "\n";
}


void Lex_array_t::clear()
{ 
	lex_array_.clear(); 
	curr_lex_ = 0; 
	num_str_ = 1;
}


void Lex_array_t::set_curr_lex(int n)
{ 
	if (n >= static_cast<int>(lex_array_.size()))
	{
		throw_exception("The number is greater than the size of the array\n", lex_array_.size() - 1);
	}

	curr_lex_ = n;
}


void Lex_array_t::inc_lex()
{ 
	if ((curr_lex_ - 1) >= static_cast<int>(lex_array_.size()))
	{
		throw_exception("The number is greater than the size of the array\n", lex_array_.size() - 1);
	}

	curr_lex_++;
}


std::shared_ptr<Lex_t> Lex_array_t::operator[](int n) const
{
	if (n >= static_cast<int>(lex_array_.size()))
	{
		throw_exception("The number is greater than the size of the array\n", lex_array_.size() - 1);
	}

	return lex_array_[n];
}


std::shared_ptr<Lex_t> Lex_array_t::get_elem(int n) const
{ 
	if (n >= static_cast<int>(lex_array_.size()))
	{
		throw_exception("The number is greater than the size of the array\n", lex_array_.size() - 1);
	}

	return lex_array_[n];
}


std::shared_ptr<Lex_t> Lex_array_t::get_next_lex() const
{ 
	if ((curr_lex_ + 1) >= static_cast<int>(lex_array_.size()))
	{
		throw_exception("The number is greater than the size of the array\n", lex_array_.size() - 1);
	}
	return lex_array_[curr_lex_ + 1];
}


std::shared_ptr<Lex_t> Lex_array_t::get_prev_lex() const
{ 
	if (curr_lex_ == 0)
	{
		throw_exception("Previos elem is not exist\n", 0);
	}
	return lex_array_[curr_lex_ - 1];
}


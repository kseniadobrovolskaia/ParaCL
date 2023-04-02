#ifndef LEXER_H
#define LEXER_H


#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <string>
#include <vector>
#include <signal.h>

int EoF = 0;

std::vector<std::string> vars;					//variables appeared in lexical analysis


enum Move { INCREMENT, GET_CURRENT, USE_CURRENT, RESET };

int token_counter(Move move);					//to move in lex_array during parsing

void throw_exception(std::string mess, int error_elem);


//--------------------------------------------LEX_KIND--------------------------------------------------------


enum Lex_kind_t {
	BRACE,
	VAR,
	STMT,
	VALUE,
	SCOPE,
	UNOP,
	BINOP,
	COMPOP,
	SYMBOL,
	BAD_SYMBOL,

};


//--------------------------------------------LEX_DATA--------------------------------------------------------


enum BinOp_t { ADD, SUB, MULT, DIV };

enum Brace_t { LBRACE, RBRACE };

enum Scope_t { LSCOPE, RSCOPE };

enum CompOp_t { LESS, GREATER, LESorEQ, GRorEQ, EQUAL, NOT_EQUAL };

enum Statements_t { ASSIGN, IF, WHILE, PRINT, INC, DEC, ARITHMETIC };

enum Symbols_t { SEMICOL, SCAN, ELSE, NEGATION };



//-----------------------------------------GLOBAL_LEX_ARRAY---------------------------------------------------


class Lex_t;
std::vector<Lex_t*> lex_array;


//--------------------------------------------LEX_CLASS-------------------------------------------------------


class Lex_t
{
	Lex_kind_t kind_;
	int data_;              //for example, Scope_t::LSCOPE
	int num_str_;			//line number with this token
	int num_;				//index this token in lex_array

public:
	Lex_t(Lex_kind_t kind, int data, int num) : kind_(kind), data_(data), num_str_(num){ num_ = lex_array.size(); };
	Lex_t(const Lex_t &rhs) : kind_(rhs.kind_), data_(rhs.data_), num_str_(rhs.num_str_), num_(rhs.num_){};
	virtual ~Lex_t() = default;
	std::string name() const;
	std::string short_name() const;
	Lex_kind_t get_kind() const { return kind_; };
	int get_str() const { return num_str_; };
	int get_data() const { return data_; };
	int get_num() const { return num_; };
	virtual Lex_t* get_var() const { return nullptr; };
	virtual int calculate(std::istream & istr, std::ostream & ostr) { return data_; };
};


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


void push_binop(std::vector<Lex_t*> &lex_array, BinOp_t binop, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BINOP, binop, num_str));
}

void push_unop(std::vector<Lex_t*> &lex_array, Statements_t unop, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::UNOP, unop, num_str));
}

void push_brace(std::vector<Lex_t*> &lex_array, Brace_t brace, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BRACE, brace, num_str));
}

void push_scope(std::vector<Lex_t*> &lex_array, Scope_t scope, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::SCOPE, scope, num_str));
}

void push_stmt(std::vector<Lex_t*> &lex_array, Statements_t kw, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::STMT, kw, num_str));
}

void push_var(std::vector<Lex_t*> &lex_array, int num_var, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::VAR, num_var, num_str));
}

void push_value(std::vector<Lex_t*> &lex_array, int value, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::VALUE, value, num_str));
}

void push_compop(std::vector<Lex_t*> &lex_array, CompOp_t compop, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::COMPOP, compop, num_str));
}

void push_symbol(std::vector<Lex_t*> &lex_array, Symbols_t symbol, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::SYMBOL, symbol, num_str));
}

void push_bad_symbol(std::vector<Lex_t*> &lex_array, int symbol, int num_str)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BAD_SYMBOL, symbol, num_str));
}


//---------------------------------------------LEX_STRING---------------------------------------------------


void handler(int signo)
{
	EoF = 1;
}


std::vector<Lex_t*> lex_string(std::istream & istr)
{
	int num_var = 0, num_str = 1;
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
				else if (word == "while")
				{
					push_stmt(lex_array, WHILE, num_str);
				}
				else if (word == "print")
				{
					push_stmt(lex_array, PRINT, num_str);
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


//------------------------------------------------NAMES---------------------------------------------------


std::string Lex_t::name() const
{
	switch (kind_)
	{
	case Lex_kind_t::BRACE:
		return static_cast<std::string>("BRACE:") + ((data_ == Brace_t::LBRACE) ? "LBRACE" : "RBRACE");
	case Lex_kind_t::UNOP:
		return static_cast<std::string>("UNOP:") + ((data_ == Statements_t::INC) ? "INC" : "DEC");
	case Lex_kind_t::SCOPE:
		return static_cast<std::string>("SCOPE:") + ((data_ == Scope_t::LSCOPE) ? "LSCOPE" : "RSCOPE");
	case Lex_kind_t::VALUE:
		return static_cast<std::string>("VALUE:") + std::to_string(data_);
	case Lex_kind_t::STMT:
	{
		std::string type;
		switch (data_)
		{
		case Statements_t::ASSIGN:
			type = "=";
			break;
		case Statements_t::IF:
			type = "if";
			break;
		case Statements_t::WHILE:
			type = "while";
			break;
		case Statements_t::PRINT:
			type = "print";
			break;
		}
		return static_cast<std::string>("STMT:") + type;
	}
	case Lex_kind_t::SYMBOL:
	{
		std::string type;
		switch (data_)
		{
		case Symbols_t::ELSE:
			type = "else";
			break;
		case Symbols_t::SCAN:
			type = "?";
			break;
		case Symbols_t::SEMICOL:
			type = ";";
			break;
		case Symbols_t::NEGATION:
			type = "!";
			break;
		}
		return static_cast<std::string>("SYMBOL:") + type;
	}
	case Lex_kind_t::BAD_SYMBOL:
	{
		std::string ret;
		ret.push_back(data_);
		return ret;
		return static_cast<std::string>("STMT:") + ret;
	}
	case Lex_kind_t::BINOP:
	{
		std::string type;
		switch (data_)
		{
		case BinOp_t::ADD:
			type = "+";
			break;
		case BinOp_t::SUB:
			type = "-";
			break;
		case BinOp_t::MULT:
			type = "*";
			break;
		case BinOp_t::DIV:
			type = "/";
			break;
		}
		return static_cast<std::string>("BINOP:") + type;
	}
	case Lex_kind_t::COMPOP:
	{
		std::string type;
		switch (data_)
		{
		case CompOp_t::LESS:
			type = "<";
			break;
		case CompOp_t::GREATER:
			type = ">";
			break;
		case CompOp_t::LESorEQ:
			type = "<=";
			break;
		case CompOp_t::GRorEQ:
			type = ">=";
			break;
		case CompOp_t::EQUAL:
			type = "==";
			break;
		case CompOp_t::NOT_EQUAL:
			type = "!=";
			break;

		}
		return static_cast<std::string>("COMPOP:") + type;
	}
	case Lex_kind_t::VAR:
		return static_cast<std::string>("VAR:") + vars[data_];
	}
	return nullptr;
}


//---------------------------------------SHORT_NAMES---------------------------------------------------


std::string Lex_t::short_name() const
{
	switch (kind_)
	{
	case Lex_kind_t::VAR:
		return vars[data_];
	case Lex_kind_t::BRACE:
		return ((data_ == Brace_t::LBRACE) ? "(" : ")");
	case Lex_kind_t::UNOP:
		return ((data_ == Statements_t::INC) ? "++" : "--");
	case Lex_kind_t::SCOPE:
		return ((data_ == Scope_t::LSCOPE) ? "{" : "}");
	case Lex_kind_t::VALUE:
		return std::to_string(data_);
	case Lex_kind_t::STMT:
		switch (data_)
		{
		case Statements_t::ASSIGN:
			return "=";
		case Statements_t::PRINT:
			return "print";
		case Statements_t::IF:
			return "if";
		case Statements_t::WHILE:
			return "while";
		}
	case Lex_kind_t::SYMBOL:
		switch (data_)
		{
		case Symbols_t::ELSE:
			return "else";
		case Symbols_t::SEMICOL:
			return ";";
		case Symbols_t::SCAN:
			return "?";
		case Symbols_t::NEGATION:
			return "!";
		}
	case Lex_kind_t::BAD_SYMBOL:
		{
			std::string ret;
			ret.push_back(data_);
			return ret;
		}
	case Lex_kind_t::BINOP:
		switch (data_)
		{
		case BinOp_t::ADD:
			return "+";
		case BinOp_t::SUB:
			return "-";
		case BinOp_t::MULT:
			return "*";
		case BinOp_t::DIV:
			return "/";
		}
		
	case Lex_kind_t::COMPOP:
		switch (data_)
		{
		case CompOp_t::LESS:
			return "less";
		case CompOp_t::GREATER:
			return "greater";
		case CompOp_t::LESorEQ:
			return "lesOReq";
		case CompOp_t::GRorEQ:
			return "grOReq";
		case CompOp_t::EQUAL:
			return "==";
		case CompOp_t::NOT_EQUAL:
			return "!=";
		}
	}
	return nullptr;
}


#endif

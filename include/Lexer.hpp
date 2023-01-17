#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctype.h>
#include <algorithm>
#include <fstream>
#include <typeinfo>

std::unordered_map<std::string, int>VARS;
std::vector<std::string> vars;

typedef enum Lex_kind_t {
	BRACE,
	VAR,
	STMT,
	VALUE,
	SCOPE,
	UNOP,
	BINOP,
	COMPOP,
	SYMBOL,

} Lex_kind_t;


class Lex_t
{
	Lex_kind_t kind_;
	int data_;

public:
	Lex_t(Lex_kind_t kind, int data) : kind_(kind), data_(data){};
	virtual ~Lex_t() = default;
	std::string name() const;
	std::string short_name() const;
	Lex_kind_t get_kind() const { return kind_; };
	int get_data() const { return data_; };
	virtual Lex_t* get_var() const { return nullptr; };
	virtual int calculate();
};


enum BinOp_t { ADD, SUB, MULT, DIV };

enum Brace_t { LBRACE, RBRACE };

enum Scope_t { LSCOPE, RSCOPE };

enum CompOp_t { LESS, GREATER, LESorEQ, GRorEQ, EQUAL, NOT_EQUAL };

enum Statements_t { ASSIGN, IF, WHILE, PRINT, INC, DEC };

enum Symbols_t { SEMICOL, SCAN };


void push_binop(std::vector<Lex_t*> &lex_array, BinOp_t binop)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BINOP, binop));
}

void push_unop(std::vector<Lex_t*> &lex_array, Statements_t unop)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::UNOP, unop));
}

void push_brace(std::vector<Lex_t*> &lex_array, Brace_t brace)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BRACE, brace));
}

void push_scope(std::vector<Lex_t*> &lex_array, Scope_t scope)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::SCOPE, scope));
}

void push_stmt(std::vector<Lex_t*> &lex_array, Statements_t kw)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::STMT, kw));
}

void push_var(std::vector<Lex_t*> &lex_array, int num_var)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::VAR, num_var));
}

void push_value(std::vector<Lex_t*> &lex_array, int value)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::VALUE, value));
}

void push_compop(std::vector<Lex_t*> &lex_array, CompOp_t compop)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::COMPOP, compop));
}

void push_symbol(std::vector<Lex_t*> &lex_array, Symbols_t symbol)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::SYMBOL, symbol));
}


std::vector<Lex_t*> lex_string(std::vector<std::string> &vars)
{
	static int num_var = 0;
	char elem, prev = '\0';
	std::string word;
	std::vector <Lex_t*> lex_array;

	std::cin >> std::noskipws;
	std::cin >> elem;
	
	while (elem != '.')
	{
		switch (elem)
		{
		case ';':
			push_symbol(lex_array, SEMICOL);
			break;
		case '=':
			switch(prev)
			{
			case '=':
				lex_array.pop_back();
				push_compop(lex_array, EQUAL);
				break;
			case '<':
				lex_array.pop_back();
				push_compop(lex_array, LESorEQ);
				break;
			case '>':
				lex_array.pop_back();
				push_compop(lex_array, GRorEQ);
				break;
			default:
				push_stmt(lex_array, ASSIGN);
			}
			break;
		case '<':
			push_compop(lex_array, LESS);
			break;
		case '>':
			push_compop(lex_array, GREATER);
			break;
		case '!':
			std::cin >> elem;
			if (elem != '=')
			{
				throw std::logic_error("Syntax error");
			}
			push_compop(lex_array, NOT_EQUAL);
			break;
		case '+':
		{
			if (prev == '+')
			{
				lex_array.pop_back();
				push_unop(lex_array, INC);
			}
			else
			{
				push_binop(lex_array, ADD);
			}

			break;
		}
		case '-':
			if (prev == '-')
			{
				lex_array.pop_back();
				push_unop(lex_array, DEC);
				break;
			}
			else
			{
				push_binop(lex_array, SUB);
			}

			break;
		case '*':
			if(prev == '/')
			{
				lex_array.pop_back();
				while (elem != '/' || prev != '*')
				{
					prev = elem;
					std::cin >> elem;
				}
			}
			else
			{
				push_binop(lex_array, MULT);
			}
			break;
		case '/':
			if(prev == '/')
			{
				lex_array.pop_back();
				while (elem != '\n')
				{
					std::cin >> elem;
				}
			}
			else
			{
				push_binop(lex_array, DIV);
			}
			break;
		case '(':
			push_brace(lex_array, LBRACE);
			break;
		case ')':
			push_brace(lex_array, RBRACE);
			break;
		case '{':
			push_scope(lex_array, LSCOPE);
			break;
		case '}':
			push_scope(lex_array, RSCOPE);
			break;
		case '?':
			push_symbol(lex_array, SCAN);
			break;
		}

		if (std::isdigit(elem))
		{
			while (std::isdigit(elem))
			{
				word += elem;
				prev = elem;
				std::cin >> elem;
			}

			push_value(lex_array, std::stoi(word));
			word.clear();
			continue;
		}
		
		if (std::isalpha(elem) || elem == '_')
		{
			while (std::isalpha(elem) || std::isdigit(elem) || elem == '_')
			{
				word += elem;
				prev = elem;
				std::cin >> elem;
			}

			if (word == "if")
			{
				push_stmt(lex_array, IF);
			}
			else if (word == "while")
			{
				push_stmt(lex_array, WHILE);
			}
			else if (word == "print")
			{
				push_stmt(lex_array, PRINT);
			}
			else
			{
				std::vector<std::string>::iterator itr = std::find(vars.begin(), vars.end(), word);
				if (itr == vars.end())
				{
					push_var(lex_array, num_var);
					vars.push_back(word);
					num_var++;					
				}
				else
				{
					push_var(lex_array, std::distance(vars.begin(), itr));
				}

			}

			word.clear();
			continue;
		}

	    prev = elem;
	    
	    std::cin >> elem;
	}

	return lex_array;
}


int Lex_t::calculate()
{
	return data_;
}


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
		return static_cast<std::string>("VALUE:") + static_cast<std::string>(std::to_string(data_));
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
		case Symbols_t::SCAN:
			type = "?";
			break;
		case Symbols_t::SEMICOL:
			type = ";";
			break;
		}
		return static_cast<std::string>("SYMBOL:") + type;
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
		case Symbols_t::SEMICOL:
		return ";";
		case Symbols_t::SCAN:
		return "?";
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

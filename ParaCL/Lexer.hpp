#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctype.h>
#include <algorithm>


typedef enum Lex_kind_t {
	BRACE,
	DECL,
	KEYWD,
	VALUE,
	SCOPE,
	UNOP,
	BINOP,
	COMPOP,

} Lex_kind_t;


class Lex_t
{
	Lex_kind_t kind_;
	int data_;

public:
	Lex_t(Lex_kind_t kind, int data) : kind_(kind), data_(data){};
	std::string name();
	Lex_kind_t get_kind() { return kind_; };
	int get_data() { return data_; };
	
};

enum UnOp_t { INC, DEC };

enum BinOp_t { ADD, SUB, MULT, DIV };

enum Brace { LBRACE, RBRACE };

enum Scope { LSCOPE, RSCOPE };

enum CompOp_t { LESS, GREATER, LESorEQ, GRorEQ, EQUAL };

enum Keywords { IF, WHILE, PRINT, SCAN };


void push_binop(std::vector<Lex_t*> &lex_array, BinOp_t binop)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BINOP, binop));
}

void push_unop(std::vector<Lex_t*> &lex_array, UnOp_t unop)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::UNOP, unop));
}

void push_brace(std::vector<Lex_t*> &lex_array, Brace brace)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::BRACE, brace));
}

void push_scope(std::vector<Lex_t*> &lex_array, Scope scope)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::SCOPE, scope));
}

void push_keyword(std::vector<Lex_t*> &lex_array, Keywords kw)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::KEYWD, kw));
}

void push_decl(std::vector<Lex_t*> &lex_array, int num_var)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::DECL, num_var));
}

void push_value(std::vector<Lex_t*> &lex_array, int value)
{
	lex_array.push_back(new Lex_t(Lex_kind_t::VALUE, value));
}


std::vector<Lex_t*> lex_string(std::vector<std::string> &vars)
{
	static int num_var = 0;
	char elem, prev = '\0';
	std::string word;
	std::vector <Lex_t*> lex_array;
	//Lex_t *prev_lex = nullptr;

	std::cin >> std::noskipws;
	std::cin >> elem;
	
	while (elem != ';')
	{
		switch (elem)
		{
		case '+':
			if (prev == '+')
			{	
				lex_array.pop_back();
				push_unop(lex_array, DEC);
				break;
			}

			push_binop(lex_array, ADD);
			break;
		case '-':
			push_binop(lex_array, SUB);
			break;
		case '*':
			push_binop(lex_array, MULT);
			break;
		case '/':
			push_binop(lex_array, DIV);
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
			push_keyword(lex_array, SCAN);
			break;
		}

		if (std::isdigit(elem))
		{
			while (std::isdigit(elem))
			{
				word += elem;
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
				std::cin >> elem;
			}

			if (word == "if")
			{
				push_keyword(lex_array, IF);
			}
			else if (word == "while")
			{
				push_keyword(lex_array, WHILE);
			}
			else if (word == "print")
			{
				push_keyword(lex_array, PRINT);
			}
			else
			{
				std::vector<std::string>::iterator itr = std::find(vars.begin(), vars.end(), word);
				if (itr == vars.end())
				{
					push_decl(lex_array, num_var);
					vars.push_back(word);
					num_var++;					
				}
				else
				{
					push_decl(lex_array, std::distance(vars.begin(), itr));
				}

			}

			word.clear();
			continue;
		}
		

		//if (!std::isspace(elem) || std::cin.fail())
	    //{
	     //   throw std::logic_error("Invalid input");
	    //}

	    prev = elem;
	    
	    std::cin >> elem;
	
	}

	return lex_array;
}


std::string Lex_t::name()
{
	switch (kind_)
	{
	case Lex_kind_t::BRACE:
		return static_cast<std::string>("BRACE:") + ((data_ == 0) ? "LBRACE" : "RBRACE");
	case Lex_kind_t::UNOP:
		return static_cast<std::string>("UNOP:") + ((data_ == 0) ? "INC" : "DEC");
	case Lex_kind_t::SCOPE:
		return static_cast<std::string>("SCOPE:") + ((data_ == 0) ? "LSCOPE" : "RSCOPE");
	case Lex_kind_t::VALUE:
		return static_cast<std::string>("VALUE:") + static_cast<std::string>(std::to_string(data_));
	case Lex_kind_t::KEYWD:
		if (data_ == 0 || data_ == 1)
		{
			return static_cast<std::string>("KEYWD:") + ((data_ == 0) ? "if" : "while");
		}
		return static_cast<std::string>("KEYWD:") + ((data_ == 2) ? "print" : "scan");
	case Lex_kind_t::BINOP:
		if (data_ == 0 || data_ == 1)
		{
			return static_cast<std::string>("BINOP:") + ((data_ == 0) ? "ADD" : "SUB");
		}
		return static_cast<std::string>("BINOP:") + ((data_ == 2) ? "MULT" : "DIV");
	case Lex_kind_t::COMPOP:
		if (data_ == 4)
		{
			return "COMPOP:EQUAL";
		}
		if (data_ == 0 || data_ == 1)
		{
			return static_cast<std::string>("COMPOP:") + ((data_ == 0) ? "LESS" : "GREATER");
		}
		return static_cast<std::string>("COMPOP:") + ((data_ == 2) ? "LESorEQ" : "GRorEQ");
	case Lex_kind_t::DECL:
		return static_cast<std::string>("DECL:");
	}
	return nullptr;
}




#endif

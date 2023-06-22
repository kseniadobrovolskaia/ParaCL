#ifndef LEXER_H
#define LEXER_H


#include <algorithm>
#include <typeinfo>
#include <cstdlib>
#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <vector>
#include <signal.h>

#include "AST_creator.hpp"


/**
 * @brief throw_exception - function to display error messages
 * 
 * @param mess       - error message
 * 		  error_elem - index token with error
 * 		  lex_array_ - specified only when you need to set a static lex_array in this function
 */
void throw_exception(std::string mess, int error_elem, std::vector<std::shared_ptr<Lex_t>> lex_array_ = std::vector<std::shared_ptr<Lex_t>>());


//--------------------------------------------LEX_KIND--------------------------------------------------------


enum Lex_kind_t {
	BRACE,
	VAR,
	FUNCTION,
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

enum Statements_t { ASSIGN, IF, WHILE, PRINT, INC, DEC, ARITHMETIC, FUNC, RETURN };

enum Symbols_t { SEMICOL, SCAN, ELSE, NEGATION, COLON, COMMA };


//--------------------------------------------LEX_CLASS-------------------------------------------------------


/**
 * @brief class Lex_t - token as part of an Lex_array_t
 */
class Lex_t
{
	Lex_kind_t kind_;
	int data_;              ///for example, Scope_t::LSCOPE
	int num_str_;			///line number with this token
	int num_;				///index this token in lex_array

	static std::vector<std::string> vars_;					///variables appeared in lexical analysis
	static std::vector<std::string> funcs_;					///function names appeared in lexical analysis

public:
	Lex_t() = default;
	
	Lex_t(Lex_kind_t kind, int data, int num_str, int num) : 
	kind_(kind), data_(data), num_str_(num_str), num_(num) {};

	Lex_t(const Lex_t &rhs) : 
	kind_(rhs.kind_), data_(rhs.data_), num_str_(rhs.num_str_), num_(rhs.num_){};
	
	virtual ~Lex_t() = default;

	std::string name() const;
	std::string short_name() const;
	Lex_kind_t  get_kind() const { return kind_; };
	int         get_str()  const { return num_str_; };
	int         get_data() const { return data_; };
	int         get_num()  const { return num_; };

	static std::vector<std::string>& vars_table()  { return vars_; }
	static std::vector<std::string>& funcs_table() { return funcs_; }

	virtual llvm::Value *codegen()                                               { return nullptr; }
	virtual int          calculate(std::istream &istr, std::ostream &ostr) const { return data_; };
};


#endif

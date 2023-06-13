#ifndef LEXER_H
#define LEXER_H


#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>
#include <signal.h>
#include <unordered_map>
#include <memory>
#include <map>


#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

extern bool EoF;

extern std::vector<std::string> vars;					//variables appeared in lexical analysis
extern std::vector<std::string> funcs;

extern std::unique_ptr<llvm::Module> TheModule;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::map<std::string, llvm::Value*> NamedValues;

enum Move { INCREMENT, GET_CURRENT, USE_CURRENT, RESET };
int token_counter(Move move);					//to move in lex_array during parsing

void throw_exception(std::string mess, int error_elem);


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



//-----------------------------------------GLOBAL_LEX_ARRAY---------------------------------------------------


class Lex_t;
extern std::vector<std::shared_ptr<Lex_t>> lex_array;


//--------------------------------------------LEX_CLASS-------------------------------------------------------


class Lex_t
{
	Lex_kind_t kind_;
	int data_;              //for example, Scope_t::LSCOPE
	int num_str_;			//line number with this token
	int num_;				//index this token in lex_array

public:
	Lex_t() = default;
	Lex_t(Lex_kind_t kind, int data, int num) : kind_(kind), data_(data), num_str_(num){ num_ = lex_array.size(); };
	Lex_t(const Lex_t &rhs) : kind_(rhs.kind_), data_(rhs.data_), num_str_(rhs.num_str_), num_(rhs.num_){};
	virtual ~Lex_t() = default;

	std::string name() const;
	std::string short_name() const;
	Lex_kind_t get_kind() const { return kind_; };
	int get_str() const { return num_str_; };
	int get_data() const { return data_; };
	int get_num() const { return num_; };
	virtual llvm::Value *codegen() { return nullptr; }
	virtual int calculate(std::istream &istr, std::ostream &ostr) const { return data_; };
};


//---------------------------------------------LEX_STRING---------------------------------------------------


extern std::vector<std::shared_ptr<Lex_t>> lex_string(std::istream &istr);


#endif

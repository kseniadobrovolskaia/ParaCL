#ifndef AST_CREATOR_H
#define AST_CREATOR_H


#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>


#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"



class Lex_t;
class Value;
class Variable;
class Negation;
class Assign_node;
class Scope;
class Function;
class UnOp;
class CompOp;
class BinOp;
class Declaration;
class If;
class While;
class Print;
class Return;
class Arithmetic;
class Statement;
class Scope_table;
class Lex_array_t;


/**
 * @brief class AST_creator - class for lexical analysis, parsing, program execution and codegeneration.
 */
class AST_creator {

protected:

	bool RETURN_COMMAND_;

	std::shared_ptr<Lex_t>       AST_;
	std::shared_ptr<Lex_array_t> Tokens_;

	//Global function names that are visible in all scopes
	std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS_;
	std::shared_ptr<Scope_table>                                CURR_SCOPE_;


public:

	AST_creator();

	void lexical_analysis(std::istream &istr);
	void parsing();
	void codegen(const std::string &file_name);
	void run_program(std::istream & istr, std::ostream & ostr);

	bool                         is_return()  const { return RETURN_COMMAND_; }
	bool                        *set_return()       { return &RETURN_COMMAND_;}
	std::shared_ptr<Lex_t>       get_AST()    const { return AST_; };
	std::shared_ptr<Lex_array_t> get_Tokens() const { return Tokens_; };
	std::shared_ptr<Scope_table> get_CURR_SCOPE() const { return CURR_SCOPE_; }
	void                         set_CURR_SCOPE(std::shared_ptr<Scope_table> scope) { CURR_SCOPE_ = scope; }
	int                         &get_var(const std::string &name, int num_lexem);
	void                         init_var(const std::string &name);
	void                         add_func(const std::string &name, llvm::Function *func);
	void                         init_func(const std::string &name, std::shared_ptr<Statement> body);

	
	void print_tokens() const;
	void print_AST() const;
};


#endif//AST_CREATOR_H

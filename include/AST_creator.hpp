#ifndef AST_CREATOR_H
#define AST_CREATOR_H


#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>


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


class Lex_t;
class Statement;
class Scope_table;
class Lex_array_t;


/**
 * @brief class AST_creator - class for lexical analysis, parsing and program execution
 */
class AST_creator {

	std::shared_ptr<Lex_t>       AST_;
	std::shared_ptr<Lex_array_t> Tokens_;

	std::shared_ptr<Scope_table> CURR_SCOPE_;

	//Global function names that are visible in all scopes
	std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS_;


public:

	AST_creator();

	void lexical_analysis(std::istream &istr);
	void parsing();
	void codegen();
	void run_program(std::istream & istr, std::ostream & ostr);

	std::shared_ptr<Lex_t>       get_AST()        const { return AST_; };
	std::shared_ptr<Lex_array_t> get_Tokens()     const { return Tokens_; };
	std::shared_ptr<Scope_table> get_curr_scope() const { return CURR_SCOPE_; };
	
	void print_tokens();
	void print_AST();

	/// For class "Lex_t" in the methods "calculate" to know the current situation of the program
	static bool RETURN_COMMAND;
	static int  IN_FUNCTION;

	static std::shared_ptr<Scope_table>                                CURR_SCOPE;
	static std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS;

	/// To build LLVM IR
	static std::shared_ptr<llvm::LLVMContext>  TheContext;
	static std::shared_ptr<llvm::Module>       TheModule;
	static std::shared_ptr<llvm::IRBuilder<>>  Builder;
	static std::map<std::string, llvm::Value*> NamedValues;

};


#endif

#ifndef CODEGEN_CREATOR_H
#define CODEGEN_CREATOR_H


#include "AST_creator.hpp"



/**
 * @brief class Codegen_creator - class-visitor of token classes, for their codegeneration.
 * 			
 */
class Codegen_creator : public  AST_creator {

	std::shared_ptr<llvm::LLVMContext> TheContext_ = std::make_shared<llvm::LLVMContext>();
	std::shared_ptr<llvm::Module>      TheModule_  = std::make_shared<llvm::Module>("ParaCL", *TheContext_);
	std::shared_ptr<llvm::IRBuilder<>> Builder_    = std::make_shared<llvm::IRBuilder<>>(*TheContext_);

public:

	Codegen_creator(AST_creator &creator) : AST_creator(creator) {};

	llvm::Value *codegen_value(Value &value);
	llvm::Value *codegen_variable(Variable &variable);
	llvm::Value *codegen_negation(Negation &negation);
	llvm::Value *codegen_assign(Assign_node &assign_node);
	llvm::Value *codegen_scope(Scope &scope);
	llvm::Value *codegen_call(Function &function);
	llvm::Value *codegen_unop(UnOp &unop);
	llvm::Value *codegen_binop(BinOp &binop);
	llvm::Value *codegen_compop(CompOp &compop);

	llvm::Value *codegen_decl(Declaration &declaration);
	llvm::Value *codegen_if(If &if_class);
	llvm::Value *codegen_while(While &while_class);
	llvm::Value *codegen_print(Print &print);
	llvm::Value *codegen_return(Return &ret);
	llvm::Value *codegen_arith(Arithmetic &arith);

	llvm::Function   *codegen_func_decl(Declaration &declaration);
	llvm::Function   *codegen_func_arith(Arithmetic &arith);
	llvm::AllocaInst *alloca_var(const std::string &name, int num_lexem);

	///Print LLVM IR in file "file_name"
	void dump(const std::string &file_name) const
	{
		std::error_code EC;
	  	llvm::raw_fd_ostream file(file_name, EC);
	  	TheModule_->print(file, nullptr);
	}
};


#endif//CODEGEN_CREATOR_H

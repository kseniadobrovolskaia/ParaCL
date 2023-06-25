#include "Parser_stmts.hpp"



AST_creator::AST_creator()
{
}


void AST_creator::lexical_analysis(std::istream &istr)
{
	Tokens_ = std::make_shared<Lex_array_t>(istr);
}


void AST_creator::run_program(std::istream & istr, std::ostream & ostr)
{
	RETURN_COMMAND = 0;
	CURR_SCOPE     = std::make_shared<Scope_table>();
	FUNCTIONS      = FUNCTIONS_;

	AST_->calculate(istr, ostr);
}


void AST_creator::parsing()
{
	IN_FUNCTION = 0;

	/// That there is nothing from the previous parsing
	CURR_SCOPE = std::make_shared<Scope_table>();
	FUNCTIONS.clear();

	AST_ = parse_scope(Tokens_);

	FUNCTIONS_  = FUNCTIONS;
}


void AST_creator::codegen(const std::string &file_name)
{
	RETURN_COMMAND = 0;
	CURR_SCOPE     = std::make_shared<Scope_table>();
	FUNCTIONS      = FUNCTIONS_;	

	if (!AST_)
	{
		return;
	}

	Arithmetic Anonim(AST_);
	Anonim.codegen_func();

	///Print LLVM IR in file "file_name"
	std::error_code EC;
  	llvm::raw_fd_ostream OS(file_name, EC);
  	AST_creator::TheModule->print(llvm::errs(), nullptr);
}


void AST_creator::print_tokens()
{ 
	std::cout << "Program tokens :" << std::endl;

	Tokens_->print();
}


void AST_creator::print_AST() 
{
	std::cout << "Program statements :" << std::endl;
	
	const std::vector<std::shared_ptr<Statement>> &stmts = std::static_pointer_cast<Scope>(AST_)->get_lhs();

	for (auto &&elem : stmts)
	{
		std::cout << elem->name() << "\n";
	}
}

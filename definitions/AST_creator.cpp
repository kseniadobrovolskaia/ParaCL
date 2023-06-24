#include "Parser_stmts.hpp"



AST_creator::AST_creator()
{
}


void AST_creator::lexical_analysis(std::istream &istr)
{
	/// That there is nothing from the previous lexical analysis
	CURR_SCOPE = std::make_shared<Scope_table>();
	FUNCTIONS.clear();

	Tokens_ = std::make_shared<Lex_array_t>(istr);

	/// We write the result of the analysis to the local members of the class object
	CURR_SCOPE_ = CURR_SCOPE;
	FUNCTIONS_  = FUNCTIONS;
}


void AST_creator::run_program(std::istream & istr, std::ostream & ostr)
{
	RETURN_COMMAND = 0;
	IN_FUNCTION    = 0;
	CURR_SCOPE     = CURR_SCOPE_;
	FUNCTIONS      = FUNCTIONS_;

	AST_->calculate(istr, ostr);

	CURR_SCOPE_ = CURR_SCOPE;
	FUNCTIONS_  = FUNCTIONS;
}


void AST_creator::parsing()
{
	CURR_SCOPE     = CURR_SCOPE_;
	FUNCTIONS      = FUNCTIONS_;

	AST_ = parse_scope(Tokens_);

	CURR_SCOPE_ = CURR_SCOPE;
	FUNCTIONS_  = FUNCTIONS;
}


void AST_creator::codegen()
{
	CURR_SCOPE = std::make_shared<Scope_table>();

	if (!AST_)
	{
		return;
	}

	Arithmetic Anonim(AST_);
	auto *func = Anonim.codegen_func();
	
	std::cout << "\n\n\n";
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
	
	const std::vector<std::shared_ptr<Statement>> &stmts = (static_cast<Scope*>(AST_.get()))->get_lhs();

	for (auto &&elem : stmts)
	{
		std::cout << elem->name() << "\n";
	}
}

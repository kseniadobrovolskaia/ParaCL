#include "AST_creator.hpp"
#include "Parser_stmts.hpp"



AST_creator::AST_creator()
{ 
	CURR_SCOPE = std::make_shared<Scope_table>();
}


void AST_creator::lexical_analysis(std::istream &istr)
{
	Tokens_ = std::make_shared<Lex_array_t>(istr);
}


void AST_creator::run_program(std::istream & istr, std::ostream & ostr)
{
	AST_->calculate(istr, ostr);
}


void AST_creator::parsing()
{
	AST_ = parse_scope(Tokens_);
}


void AST_creator::print_tokens()
{ 
	Tokens_->print();
}


void AST_creator::print_AST() 
{
	std::cout << "Утверждения программы :" << std::endl;

	
	const std::vector<std::shared_ptr<Statement>> &stmts = (static_cast<Scope*>(AST_.get()))->get_lhs();

	for (auto &&elem : stmts)
	{
		std::cout << elem->name() << "\n";
	}
}

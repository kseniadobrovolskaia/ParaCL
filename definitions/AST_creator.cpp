#include "Parser.hpp"



AST_creator::AST_creator()
{
}


void AST_creator::lexical_analysis(std::istream &istr)
{
	Tokens_ = std::make_shared<Lex_array_t>(istr);
}


void AST_creator::parsing()
{
	Parser parser(Tokens_);

	AST_       = parser.parse_scope();
	FUNCTIONS_ = parser.get_global_funcs();
}


void AST_creator::run_program(std::istream & istr, std::ostream & ostr)
{
	RETURN_COMMAND_ = 0;
	CURR_SCOPE_     = std::make_shared<Main_Scope_table>(FUNCTIONS_);

	AST_->calculate(istr, ostr, *this);
}


void AST_creator::codegen(const std::string &file_name)
{
	RETURN_COMMAND_ = 0;
	CURR_SCOPE_     = std::make_shared<Main_Scope_table>(FUNCTIONS_);

	Codegen_creator cod_creator(*this);

	if (!AST_)
	{
		return;
	}

	Arithmetic Anonim(AST_);
	Anonim.codegen_func(cod_creator);

	cod_creator.dump(file_name);
}


void AST_creator::print_tokens() const
{ 
	std::cout << "Program tokens :" << std::endl;

	Tokens_->print();
}


void AST_creator::print_AST() const
{
	std::cout << "Program statements :" << std::endl;
	
	const std::vector<std::shared_ptr<Statement>> &stmts = std::static_pointer_cast<Scope>(AST_)->get_lhs();

	for (auto &&elem : stmts)
	{
		std::cout << elem->name() << "\n";
	}
}


int &AST_creator::get_var(const std::string &name, int num_lexem)
{ 
	return CURR_SCOPE_->get_var(name, num_lexem);
}

void AST_creator::init_var(const std::string &name)
{ 
	CURR_SCOPE_->init_var(name);
}

void AST_creator::add_func(const std::string &name, llvm::Function *func)
{ 
	CURR_SCOPE_->add_func(name, func);
}

void AST_creator::init_func(const std::string &name, std::shared_ptr<Statement> body)
{ 
	CURR_SCOPE_->init_func(name, body);
}

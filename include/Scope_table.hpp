#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H


#include "Lex_array.hpp"


//--------------------------------------------SCOPE_TABLE_CLASS-------------------------------------------------------



/**
 * @brief class Scope_table - to create a scope that defines the lifetimes of all variables and functions
 * 
 */
class Scope_table
{
	///Scope which contains this scope
	std::shared_ptr<Scope_table> higher_scope_;

	///Variables declared in this scope
	std::unordered_map<std::string, int> vars_;

	///Functions declared in this scope
	std::unordered_map<std::string, std::shared_ptr<Statement>> funcs_;

public:
	Scope_table(std::shared_ptr<Scope_table> higher_scope) : higher_scope_(higher_scope) {};
	Scope_table() : higher_scope_(nullptr) {};

	void init_var(const std::string &name);
	void init_func(const std::string &name, std::shared_ptr<Statement> body);

	///in this or higher_scope_
	int is_var_exist(const std::string &name) const;
	int is_func_exist(const std::string &name) const;

	int                         &get_var(const std::string &name, int num_lexem);
	std::shared_ptr<Statement>   get_func_decl(const std::string &name, int num_lexem);
	std::shared_ptr<Scope_table> get_high_scope() const { return higher_scope_; };
	void                         print_scope() const;

	void clean_var_table() noexcept;
	void clean_func_table() noexcept;
};


#endif

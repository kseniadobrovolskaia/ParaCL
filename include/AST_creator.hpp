#ifndef AST_CREATOR_H
#define AST_CREATOR_H

#include <memory>
#include <unordered_map>
#include <iostream>


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
	void run_program(std::istream & istr, std::ostream & ostr);


	void print_tokens();
	void print_AST();

	/// For class "Lex_t" in the methods "calculate" to know the current situation of the program
	static bool RETURN_COMMAND;
	static int  IN_FUNCTION;

	static std::shared_ptr<Scope_table> CURR_SCOPE;
	static std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS;
};


#endif

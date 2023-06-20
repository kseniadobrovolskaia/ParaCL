#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H

#include "Stmt_methods.hpp"


std::shared_ptr<Statement> parse_declaration(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_arithmetic(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Statement> parse_return(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Statement> parse_while(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Statement> parse_print(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Statement> parse_if(std::shared_ptr<Lex_array_t> lex_array);
std::shared_ptr<Lex_t> parse_scope(std::shared_ptr<Lex_array_t> lex_array, bool reset = 0);


#endif

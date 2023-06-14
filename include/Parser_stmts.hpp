#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H

#include "Stmt_methods.hpp"

extern bool MAIN;

std::shared_ptr<Statement> parse_declaration();
std::shared_ptr<Lex_t> parse_arithmetic();
std::shared_ptr<Statement> parse_return();
std::shared_ptr<Statement> parse_while();
std::shared_ptr<Statement> parse_print();
std::shared_ptr<Statement> parse_if();
std::shared_ptr<Lex_t> parse_scope();


//--------------------------------------------RUN_PROGRAM--------------------------------------------------


void run_program(std::shared_ptr<Lex_t> prog, std::istream & istr, std::ostream & ostr);


#endif

#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H

#include "Stmt_methods.hpp"

extern bool MAIN;


int is_plus_minus(Lex_t &node);
int is_mul_div(Lex_t &node);
int is_brace(Lex_t &node);
int is_compop(Lex_t &node);
int is_scope(Lex_t &node);
int is_unop(Lex_t &node);

bool is_negation(Lex_t &node);
bool is_semicol(Lex_t &node);
bool is_binop(Lex_t &node);
bool is_assign(Lex_t &node);
bool is_return(Lex_t &node);
bool is_comma(Lex_t &node);
bool is_colon(Lex_t &node);
bool is_scan(Lex_t &node);
bool is_else(Lex_t &node);

std::shared_ptr<Statement> parse_declaration(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_arithmetic(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Statement> parse_return(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Statement> parse_while(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Statement> parse_print(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Statement> parse_if(std::vector<std::shared_ptr<Lex_t>> &lex_array);
std::shared_ptr<Lex_t> parse_scope(std::vector<std::shared_ptr<Lex_t>> &lex_array);


//--------------------------------------------RUN_PROGRAM--------------------------------------------------


void run_program(std::shared_ptr<Lex_t> prog, std::istream & istr, std::ostream & ostr);


#endif

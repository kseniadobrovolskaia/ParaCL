#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H

#include "Stmt_methods.hpp"



class Parser {

    int IN_FUNCTION = 0;

    const std::shared_ptr<Lex_array_t> &lex_array_;
 
    std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS;

public:

    Parser(std::shared_ptr<Lex_array_t> lex_array) : lex_array_(lex_array) { parse_scope(1); };

    std::shared_ptr<Statement> parse_declaration();
    std::shared_ptr<Statement> parse_return();
    std::shared_ptr<Statement> parse_while();
    std::shared_ptr<Statement> parse_print();
    std::shared_ptr<Statement> parse_if();

    std::shared_ptr<Lex_t> parse_function_call();
    std::shared_ptr<Lex_t> parse_arithmetic();
    std::shared_ptr<Lex_t> parse_negation();
    std::shared_ptr<Lex_t> parse_negative();
    std::shared_ptr<Lex_t> parse_unary();
    std::shared_ptr<Lex_t> parse_scope(bool reset = 0);
    std::shared_ptr<Lex_t> parse_asgn();
    std::shared_ptr<Lex_t> parse_bool();
    std::shared_ptr<Lex_t> parse_E();
    std::shared_ptr<Lex_t> parse_M();
    std::shared_ptr<Lex_t> parse_T();

    std::unordered_map<std::string, std::shared_ptr<Statement>> get_global_funcs() const { return FUNCTIONS; };

};


#endif//PARSER_STMTS_H

#ifndef PARSER_STMTS_H
#define PARSER_STMTS_H

#include "Stmt_methods.hpp"


/**
 * @brief class Parser - parses an array of tokens and returns the AST root, start parsing with the method "parse_scope".
 */
class Parser {

    ///Nesting level
    int IN_FUNCTION = 0;

    ///Tokens arrat for parsing
    std::shared_ptr<Lex_array_t> lex_array_;
 
    ///Global functions
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

    /**
     * @brief parse_scope - function to parse scopes
     * 
     * @param reset       - the function contains a static variable MAIN 
     *                      which says that we are parsing the main.
     *                      When this parameter is set to 1, the MAIN is again set to 1
     */
    std::shared_ptr<Lex_t> parse_scope(bool reset = 0);
    std::shared_ptr<Lex_t> parse_asgn();
    std::shared_ptr<Lex_t> parse_bool();
    std::shared_ptr<Lex_t> parse_E();
    std::shared_ptr<Lex_t> parse_M();
    std::shared_ptr<Lex_t> parse_T();

    std::unordered_map<std::string, std::shared_ptr<Statement>> get_global_funcs() const { return FUNCTIONS; };

};


#endif//PARSER_STMTS_H

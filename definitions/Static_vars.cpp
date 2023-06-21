#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Lex_array.hpp"




std::vector<std::string> Lex_t::vars_;
std::vector<std::string> Lex_t::funcs_;

bool Lex_array_t::EoF_;

bool AST_creator::RETURN_COMMAND;
int  AST_creator::IN_FUNCTION;

std::shared_ptr<Scope_table> AST_creator::CURR_SCOPE;
std::unordered_map<std::string, std::shared_ptr<Statement>> AST_creator::FUNCTIONS;

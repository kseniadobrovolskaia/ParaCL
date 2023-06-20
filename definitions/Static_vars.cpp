#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Lex_array.hpp"
#include "AST_creator.hpp"




std::vector<std::string> Lex_t::vars_;					//variables appeared in lexical analysis
std::vector<std::string> Lex_t::funcs_;

bool Lex_array_t::EoF_ = 0;

bool AST_creator::RETURN_COMMAND = 0;
int AST_creator::IN_FUNCTION = 0;

std::shared_ptr<Scope_table> AST_creator::CURR_SCOPE;
std::unordered_map<std::string, std::shared_ptr<Statement>> AST_creator::FUNCTIONS;

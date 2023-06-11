#include <vector>
#include <string>
#include <memory>
#include "Scope_table.hpp"
#include "Lex_methods.hpp"

#include <unordered_map>

bool EoF = 0;
bool MAIN = 1;

std::vector<std::string> vars;
std::vector<std::string> funcs;

class Lex_t;
std::vector<std::shared_ptr<Lex_t>> lex_array;


std::shared_ptr<Scope_table> CURR_SCOPE;
bool RETURN_COMMAND = 0;
int IN_FUNCTION = 0;

class Statement;
std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS;



#include <vector>
#include <string>
#include <memory>
#include <unordered_map>


bool EoF = 0;
bool MAIN = 1;
bool RETURN_COMMAND = 0;
int IN_FUNCTION = 0;

std::vector<std::string> vars;
std::vector<std::string> funcs;


class Scope_table;
std::shared_ptr<Scope_table> CURR_SCOPE;

class Lex_array_t;
std::shared_ptr<Lex_array_t> lex_array;

class Statement;
std::unordered_map<std::string, std::shared_ptr<Statement>> FUNCTIONS;



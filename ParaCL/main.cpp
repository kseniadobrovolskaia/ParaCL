
#include "Sintax_graph.hpp"
#include "Parser_stmts.hpp"
#include "Parser_arith.hpp"

void print_lexems(std::vector<Lex_t*> &lexems);
void print_VARS();
void print_prog_elems(std::vector<Statement*> prog);
void build_sintax_graph(std::vector<Statement*> prog);


int main(int argc, char const *argv[])
{
	try
	{
		std::vector<Lex_t*> lexems = lex_string(vars);
		program_size = lexems.size();

		std::vector<Statement*> prog = parse_program(lexems);
		build_sintax_graph(prog);
		print_VARS();

		system ("dot sintax_tree.txt -Tpng -o sintax_tree.png\n"
				"shotwell sintax_tree.png");
	
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


void print_lexems(std::vector<Lex_t*> &lexems)
{
	for (auto elem = lexems.begin(); elem < lexems.end(); ++elem)
	{
		std::cout << (*elem)->name();
		std::cout << " ";
	}
	std::cout << std::endl;
}


void print_VARS()
{
	std::cout << "Значения глобальных переменных: " << std::endl;

	for (auto const &pair : VARS)
	{
		std::cout << pair.first << " = " << pair.second << std::endl;
	}
}


void print_prog_elems(std::vector<Statement*> prog)
{
	std::cout << "Элементы программы :" << std::endl;
	for (auto elem : prog)
	{
		std::cout << elem->name() << std::endl;
	}
}


void build_sintax_graph(std::vector<Statement*> prog)
{
	std::ofstream file_tree;

	file_tree.open("sintax_tree.txt");

	if (!(file_tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}

	file_tree << "digraph G{\n           node_0[label = \"Program\", style=\"filled\", shape=\"record\", fillcolor = \"purple\"];";

	create_scope_nodes(prog, file_tree);

	file_tree << "}";
	file_tree.close();	
}



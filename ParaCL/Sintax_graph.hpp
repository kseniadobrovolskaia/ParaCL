#ifndef SINTAX_GRAPH_H
#define SINTAX_GRAPH_H

#include "Parser.hpp"

//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
//1. разбор арифметический выражений (в результате исполнения вернет инт)(не мод среду)
//2.разбор логических выражений      (в результате исполнения вернет 0 или 1)(не мод среду)
//3.разбор утверждений(присваивание, ++, --, if, while, ?, print) (assign_stmt(), if_stmt(), while_stmt())(не возвр рез)
//4.Собрать все вместе(программа==список утверждений)

void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree);

void build_sintax_graph(Lex_t *sintax_tree)
{
	std::ofstream tree;

	tree.open("sintax_tree.txt");

	if (!(tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}
	
	tree << "digraph G{ ";

	create_node_in_sintax_tree_file(sintax_tree, tree);

	tree << "}";
	tree.close();	

}


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree)
{
	static int i = 0;
	static int elemold = 0;
	int elem = i;
	std::vector<std::string> colors = {"red", "yellow", "blue", "brown", "purple", "violet", "pink", "green", "orange"};

	if (!(file_tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}

  	
	file_tree << "\n           node_" << i << "[label = \"" << sintax_tree->short_name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"" << colors[i % 9] << "\"];";

  	i++;
  	elemold = elem;

  	if (sintax_tree->get_kind() != Lex_kind_t::VALUE)
	{
		create_node_in_sintax_tree_file(static_cast<BinOp *>(sintax_tree)->get_lhs(), file_tree);
		file_tree << "\n           node_" << elem << "  -> node_" << elemold << ";\n";
		elemold = elem;
		create_node_in_sintax_tree_file(static_cast<BinOp *>(sintax_tree)->get_rhs(), file_tree);
		file_tree << "\n           node_" << elem << "  -> node_" << elemold << ";\n";
		elemold = elem;
	}
}


#endif

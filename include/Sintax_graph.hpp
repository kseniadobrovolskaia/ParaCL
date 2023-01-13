#ifndef SINTAX_GRAPH_H
#define SINTAX_GRAPH_H

#include "Parser_stmts.hpp"


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree, int *num_node);

#if 0
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
#endif
#if 1
void build_sintax_graph(std::vector<Statement*> prog)
{
	std::ofstream tree;

	tree.open("sintax_tree.txt");

	if (!(tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}
	
	tree << "digraph G{\n           node_0[label = \"Prog\", style=\"filled\", shape=\"record\", fillcolor = \"pink\"];";

	int prev_num_node = 1, prev_stmt = 0, num_node = 1, size_prog = prog.size();

	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		
		tree << "\n           node_" << num_node << "[label = \"stmt\", style=\"filled\", shape=\"record\", fillcolor = \"blue\"];";
		tree << "\n           node_" << prev_stmt << "  -> node_" << num_node << ";\n";
		prev_stmt = num_node;
		num_node++;
		tree << "\n           node_" << num_node << "[label = \"" << prog[prog_elem]->name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"purple\"];";
		tree << "\n           node_" << num_node - 1 << "  -> node_" << num_node << ";\n";
		num_node++;
		
		create_node_in_sintax_tree_file(prog[prog_elem]->get_rhs(), tree, &num_node);

	}

	tree << "}";
	tree.close();	
}

#endif


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree, int *num_node)
{
	static int i = 1;
	static int elemold = 0;
	int elem = i;
	i = *num_node;
	std::vector<std::string> colors = {"red", "yellow", "blue", "brown", "purple", "violet", "pink", "green", "orange"};

	if (!(file_tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}

	file_tree << "\n           node_" << i << "[label = \"" << sintax_tree->short_name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"" << colors[i % 9] << "\"];";

  	i++;
  	elemold = elem;

  	if ((sintax_tree->get_kind() != Lex_kind_t::VALUE) && (sintax_tree->get_kind() != Lex_kind_t::VAR))
	{
		create_node_in_sintax_tree_file(static_cast<BinOp *>(sintax_tree)->get_lhs(), file_tree, num_node);
		file_tree << "\n           node_" << elem << "  -> node_" << elemold << ";\n";
		elemold = elem;
		create_node_in_sintax_tree_file(static_cast<BinOp *>(sintax_tree)->get_rhs(), file_tree, num_node);
		file_tree << "\n           node_" << elem << "  -> node_" << elemold << ";\n";
		elemold = elem;
	}

	*num_node = i;
}


#endif

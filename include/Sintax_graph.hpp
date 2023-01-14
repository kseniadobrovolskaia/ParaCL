#ifndef SINTAX_GRAPH_H
#define SINTAX_GRAPH_H

#include "Parser_stmts.hpp"


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree, int *num_node);


void build_sintax_graph(std::vector<Statement*> prog)
{
	std::ofstream tree;

	tree.open("sintax_tree.txt");

	if (!(tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}
	
	tree << "digraph G{\n           node_0[label = \"Prog\", style=\"filled\", shape=\"record\", fillcolor = \"purple\"];";

	int prev_num_node = 1, prev_stmt = 1, num_node = 1, size_prog = prog.size();

	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		
		tree << "\n           node_" << num_node << "[label = \"stmt\", style=\"filled\", shape=\"record\", fillcolor = \"violet\"];";
		tree << "\n           node_" << prev_stmt - 1 << "  -> node_" << num_node << ";\n";
		
		prev_stmt = num_node;
		num_node++;
		tree << "\n           node_" << num_node << "[label = \"" << prog[prog_elem]->name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"pink\"];";
		tree << "\n           node_" << num_node - 1 << "  -> node_" << num_node << ";\n";
		
		prev_stmt = num_node;
		num_node++;
		prev_num_node = num_node;
		
		create_node_in_sintax_tree_file(prog[prog_elem]->get_lhs(), tree, &num_node);
		tree << "\n           node_" << prev_stmt << "  -> node_" << prev_num_node << ";\n";
		prev_num_node = num_node;
		create_node_in_sintax_tree_file(prog[prog_elem]->get_rhs(), tree, &num_node);
		tree << "\n           node_" << prev_stmt << "  -> node_" << prev_num_node << ";\n";
	}

	tree << "}";
	tree.close();	
}


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree, int *num_node)
{
	static int i = 1;
	static int elemold = 0;
	int elem = i;
	i = *num_node;
	std::vector<std::string> colors = {"azure", "beige", "coral", "snow", "purple", "violet", "pink"};

	if (!(file_tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}

	std::string colour;

	switch (sintax_tree->get_kind())
	{
	case Lex_kind_t::VAR:
		colour = "azure";
		break;
	case Lex_kind_t::VALUE:
		colour = "azure";
		break;
	case Lex_kind_t::BINOP:
		colour = "pink";
		break;
	case Lex_kind_t::COMPOP:
		colour = "pink";
		break;
	}

	file_tree << "\n           node_" << i << "[label = \"" << sintax_tree->short_name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"" << colour << "\"];";

  	i++;
  	*num_node = i;
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

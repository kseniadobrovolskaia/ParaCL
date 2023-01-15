#ifndef SINTAX_GRAPH_H
#define SINTAX_GRAPH_H

#include "Parser_stmts.hpp"


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree, int *num_node);


void create_prog_nodes(std::vector<Statement*> prog, std::ofstream &tree)
{
	static int num_node = 1, main_prog = 1;
	int prev_num_node = 1, prev_stmt = num_node - 1, stmt = 0, stmt_type = 0, size_prog = prog.size();


	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		if (main_prog == 1)
		{
			main_prog = 0;
			tree << "digraph G{\n           node_0[label = \"Prog\", style=\"filled\", shape=\"record\", fillcolor = \"purple\"];";
		}

		tree << "\n           node_" << num_node << "[label = \"stmt\", style=\"filled\", shape=\"record\", fillcolor = \"violet\"];";
		stmt = num_node;
		tree << "\n           node_" << prev_stmt << "  -> node_" << stmt << ";\n";
		prev_stmt = stmt;
		
		num_node++;
		stmt_type = num_node;
		tree << "\n           node_" << num_node << "[label = \"" << prog[prog_elem]->name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"pink\"];";
		tree << "\n           node_" << stmt << "  -> node_" << stmt_type << ";\n";
		
		num_node++;
		
		create_node_in_sintax_tree_file(prog[prog_elem]->get_lhs(), tree, &num_node);
		tree << "\n           node_" << stmt_type << "  -> node_" << stmt_type + 1 << ";\n";
		prev_num_node = num_node;

		if (prog[prog_elem]->get_kind() == Keywords_t::ASSIGN)
		{
			create_node_in_sintax_tree_file(prog[prog_elem]->get_rhs(), tree, &num_node);
			tree << "\n           node_" << stmt_type << "  -> node_" << prev_num_node << ";\n";
		}
		else if ((prog[prog_elem]->get_kind() == Keywords_t::IF) || (prog[prog_elem]->get_kind() == Keywords_t::WHILE))
		{
			tree << "\n           node_" << num_node << "[label = \"scope\", style=\"filled\", shape=\"record\", fillcolor = \"snow\"];";
			num_node++;
			create_prog_nodes(static_cast<Scope*>(prog[prog_elem]->get_rhs())->get_lhs(), tree);
			tree << "\n           node_" << stmt_type << "  -> node_" << prev_num_node << ";\n";
		}
	}
}


void create_node_in_sintax_tree_file(Lex_t *sintax_tree, std::ofstream &file_tree, int *num_node)
{
	static int i = 1;
	static int elemold = 0;
	int elem = *num_node;
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
	case Lex_kind_t::KEYWD:
		colour = "pink";
		break;
	}

	file_tree << "\n           node_" << i << "[label = \"" << sintax_tree->short_name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"" << colour << "\"];";

  	i++;
  	*num_node = i;
  	elemold = elem;

  	if ((sintax_tree->get_kind() != Lex_kind_t::VALUE) && (sintax_tree->get_kind() != Lex_kind_t::VAR) && (sintax_tree->get_kind() != Lex_kind_t::KEYWD))
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

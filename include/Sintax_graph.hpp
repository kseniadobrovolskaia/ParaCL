#ifndef SINTAX_GRAPH_H
#define SINTAX_GRAPH_H

#include "Parser_stmts.hpp"


void build_sintax_graph(std::vector<Statement*> prog);

void create_statement_nodes(Lex_t *curr_node, std::ofstream &file_tree, int *num_node);
void create_scope_nodes(std::vector<Statement*> prog, std::ofstream &tree);

int is_if_long_form(Statement *stmt);
int is_if_while(Statement *stmt);
int is_assign(Statement *stmt);
int is_unop(Statement *stmt);



//-----------------------------------------------------BUILD_SINTAX_GRAPH------------------------------------------------------------------------------------------


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


//-------------------------------------------------------NODE_CREATORS------------------------------------------------------------------------------------------


void create_scope_nodes(std::vector<Statement*> prog, std::ofstream &file_tree)
{
	static int num_node = 1;
	int prev_num_node, prev_stmt = num_node - 1, stmt, stmt_type, size_prog = prog.size();

	if (!(file_tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}

	for (int prog_elem = 0; prog_elem < size_prog; prog_elem++)
	{
		file_tree << "\n           node_" << num_node << "[label = \"stmt\", style=\"filled\", shape=\"record\", fillcolor = \"violet\"];";
		stmt = num_node;
		file_tree << "\n           node_" << prev_stmt << "  -> node_" << stmt << ";\n";
		prev_stmt = stmt;
		
		num_node++;

		if (is_unop(prog[prog_elem]) || is_assign(prog[prog_elem]))
		{
			create_statement_nodes(prog[prog_elem]->get_lhs(), file_tree, &num_node);
			file_tree << "\n           node_" << stmt << "  -> node_" << stmt + 1 << ";\n";
		}
		else
		{
			stmt_type = num_node;

			file_tree << "\n           node_" << num_node << "[label = \"" << prog[prog_elem]->name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"pink\"];";
			file_tree << "\n           node_" << stmt << "  -> node_" << stmt_type << ";\n";
			
			num_node++;

			create_statement_nodes(prog[prog_elem]->get_lhs(), file_tree, &num_node);
			file_tree << "\n           node_" << stmt_type << "  -> node_" << stmt_type + 1 << ";\n";

			prev_num_node = num_node;

			if (is_if_while(prog[prog_elem]))
			{
				file_tree << "\n           node_" << num_node << "[label = \"scope\", style=\"filled\", shape=\"record\", fillcolor = \"snow\"];";
				num_node++;
				create_scope_nodes(static_cast<Scope*>(static_cast<If*>(prog[prog_elem])->get_rhs())->get_lhs(), file_tree);
				file_tree << "\n           node_" << stmt_type << "  -> node_" << prev_num_node << ";\n";
				if (is_if_long_form(prog[prog_elem]))
				{
					prev_num_node = num_node;
					file_tree << "\n           node_" << num_node << "[label = \"else\", style=\"filled\", shape=\"record\", fillcolor = \"pink\"];";
					num_node++;
					file_tree << "\n           node_" << num_node - 1 << "  -> node_" << num_node << ";\n";
			
					file_tree << "\n           node_" << num_node << "[label = \"scope\", style=\"filled\", shape=\"record\", fillcolor = \"snow\"];";
					num_node++;
					create_scope_nodes(static_cast<Scope*>(static_cast<If*>(prog[prog_elem])->get_else())->get_lhs(), file_tree);
					file_tree << "\n           node_" << stmt_type << "  -> node_" << prev_num_node << ";\n";
				}
			}
		}
	}
}


void create_statement_nodes(Lex_t *curr_node, std::ofstream &file_tree, int *num_node)
{
	static int prev_elem = 0;
	int curr_elem = *num_node;
	
	if (!(file_tree.is_open()))
	{
	  std::cerr << "File \"sintax_tree.txt\" did not open" << std::endl;
	  exit(EXIT_FAILURE);
	}

	std::string colour;

	switch (curr_node->get_kind())
	{
	case Lex_kind_t::VAR:
		colour = "lightcyan2";
		break;
	case Lex_kind_t::VALUE:
		colour = "azure";
		break;
	case Lex_kind_t::BINOP:
	case Lex_kind_t::COMPOP:
		colour = "powderblue";
		break;
	case Lex_kind_t::STMT:
	case Lex_kind_t::SYMBOL:
	case Lex_kind_t::UNOP:
		colour = "pink";
		break;
	default:
		throw std::logic_error("Brace or scope are not a nodes");
	}

	file_tree << "\n           node_" << *num_node << "[label = \"" << curr_node->short_name() << "\", style=\"filled\", shape=\"record\", fillcolor = \"" << colour << "\"];";

	prev_elem = curr_elem;
	if (is_unop(curr_node) >= 0)
	{
		(*num_node)++;
		create_statement_nodes(curr_node->get_var(), file_tree, num_node);
		file_tree << "\n           node_" << curr_elem << "  -> node_" << prev_elem << ";\n";
		prev_elem = curr_elem;
	}

  	(*num_node)++;
  	prev_elem = curr_elem;

  	if (is_binop(curr_node) || is_assign(curr_node))
	{
		create_statement_nodes(static_cast<BinOp *>(curr_node)->get_lhs(), file_tree, num_node);
		file_tree << "\n           node_" << curr_elem << "  -> node_" << prev_elem << ";\n";
		prev_elem = curr_elem;
		create_statement_nodes(static_cast<BinOp *>(curr_node)->get_rhs(), file_tree, num_node);
		file_tree << "\n           node_" << curr_elem << "  -> node_" << prev_elem << ";\n";
		prev_elem = curr_elem;
	}
}


//---------------------------------------------------------ISERS------------------------------------------------------------------------------------------


int is_if_while(Statement *stmt)
{
	if (stmt->get_kind() == Statements_t::IF)
	{
		return 1;
	}
	if (stmt->get_kind() == Statements_t::WHILE)
	{
		return 1;
	}
	return 0;
}


int is_assign(Statement *stmt)
{
	return stmt->get_kind() == Statements_t::ASSIGN;
}


int is_unop(Statement *stmt)
{
	if ((stmt->get_kind() == Statements_t::INC) ||
	(stmt->get_kind() == Statements_t::DEC))
	{
		return 1;
	}
	return 0;
}


int is_if_long_form(Statement *stmt)
{
	if (stmt->get_kind() != Statements_t::IF)
	{
		return 0;
	}
	if (static_cast<If*>(stmt)->get_else())
	{
		return 1;
	}
	return 0;
}

#endif

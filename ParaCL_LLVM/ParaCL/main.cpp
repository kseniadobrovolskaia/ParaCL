#include "Parser_stmts.hpp"



void print_lex_array(std::vector<std::shared_ptr<Lex_t>> &lex_array);
void print_prog_elems(std::vector<std::shared_ptr<Statement>> prog);
void build_sintax_graph(std::vector<std::shared_ptr<Statement>> prog);

//`llvm-config --cxxflags --ldflags --system-libs --libs core`

static void HandleArithmetic(std::shared_ptr<Statement> elem)
{
	Lex_t &node = elem->get_lhs();
	auto *func = node.codegen();
    
    std::cout << "Read top-level expression:\n";
    func->print(llvm::errs());
    std::cout << "\n";
}


int main(int argc, char const *argv[])
{
	try
	{
		CURR_SCOPE = std::make_shared<Scope_table>();

		if (argc > 1)
		{
			std::ifstream data;

			data.open(argv[1]);
			if (!(data.is_open()))
			{
				std::cerr << "File \"" << argv[1] << "\" did not open" << std::endl;
				exit(EXIT_FAILURE);
			}

			lex_array = lex_string(data);
		}
		else
		{
			lex_array = lex_string(std::cin);
		}

		std::shared_ptr<Lex_t> prog = parse_scope(lex_array);//AST

		const std::vector<std::shared_ptr<Statement>> stmts = static_cast<Scope*>(prog.get())->get_lhs();

		//--------------------------------------------------------------------
		
		TheContext = std::make_unique<llvm::LLVMContext>();
		TheModule = std::make_unique<llvm::Module>("ParaCL", *TheContext);
		Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

		for (auto &elem : stmts)
		{
			if (dynamic_cast<Arithmetic*>(elem.get()))
			{
				HandleArithmetic(elem);
			}
			else
			{
				std::cout << "Read definition:\n";
				auto *func = elem->codegen();
	    		func->print(llvm::errs());
	    		std::cout << "\n";
			}
		}
		
		std::cout << "\n\n\n";
		TheModule->print(llvm::errs(), nullptr);

		//run_program(prog, std::cin, std::cout);

		//system ("dot sintax_tree.txt -Tpng -o sintax_tree.png\n"
				// "shotwell sintax_tree.png");
	
	}
	catch(std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}


//--------------------------------------------DEBUG--------------------------------------------------------


void print_lex_array(std::vector<std::shared_ptr<Lex_t>> &lex_array)
{
	for (auto &&elem : lex_array)
	{
		std::cout << elem->name() << " ";
	}
	std::cout << "\n";
}


void print_prog_elems(std::vector<std::shared_ptr<Statement>> prog)
{
	std::cout << "Утверждения программы :" << std::endl;
	for (auto &&elem : prog)
	{
		std::cout << elem->name() << "\n";
	}
}

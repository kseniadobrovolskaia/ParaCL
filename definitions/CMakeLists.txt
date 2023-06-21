cmake_minimum_required(VERSION 3.26.4)

project(definitions)

add_compile_options(-Wall -Wextra -std=c++20)

include_directories(.././include)			

set(SOURCE_LIB  AST_creator.cpp
				Lex_array_t.cpp
				Lex_methods.cpp
				Parser_arith.cpp
				Parser_stmts.cpp
				Scope_table.cpp
				Sintax_graph.cpp
				Static_vars.cpp
				Stmt_methods.cpp
				Throw_exception.cpp
	)


add_library(ParaCL STATIC ${SOURCE_LIB})


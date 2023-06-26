cmake_minimum_required(VERSION 3.14)

project(definitions)

find_package(LLVM REQUIRED)
if(NOT LLVM_FOUND)
	message(SEND_ERROR "Failed to find LLVM13")
	return()
else()
	include_directories(${LLVM_INCLUDE_DIR})
endif()


add_compile_options(-Wall -std=c++2a -fexceptions)

include_directories(.././include)			

set(SOURCE_LIB  AST_creator.cpp
				Codegen_creator.cpp
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




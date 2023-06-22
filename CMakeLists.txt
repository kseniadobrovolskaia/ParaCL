cmake_minimum_required(VERSION 3.14)

project(ParaCL)

find_package(LLVM REQUIRED)
if(NOT LLVM_FOUND)
	message(SEND_ERROR "Failed to find LLVM13")
	return()
else()
	include_directories(${LLVM_INCLUDE_DIR})
endif()

add_compile_options(-Wall -std=c++20 -fexceptions)

set(SOURCE_EXE ./ParaCL/main.cpp)			

include_directories(  include
					${LLVM_INCLUDE_DIR})			

add_executable(Interpreter ${SOURCE_EXE})

add_subdirectory(definitions)				
add_subdirectory(Tests)

target_link_libraries(Interpreter ParaCL LLVM)	

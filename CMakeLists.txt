cmake_minimum_required(VERSION 3.26.4)

project(ParaCL)

add_compile_options(-Wall -std=c++20)

set(SOURCE_EXE ./ParaCL/main.cpp)			

include_directories(include)			

add_executable(Interpreter ${SOURCE_EXE})

add_subdirectory(definitions)				
add_subdirectory(Tests)

target_link_libraries(Interpreter ParaCL)	

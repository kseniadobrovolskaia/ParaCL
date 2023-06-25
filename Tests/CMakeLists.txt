cmake_minimum_required(VERSION 3.14)
project(tests)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


find_package(LLVM REQUIRED)
if(NOT LLVM_FOUND)
  message(SEND_ERROR "Failed to find LLVM13")
  return()
else()
  include_directories(${LLVM_INCLUDE_DIR})
endif()

add_compile_options(-Wall -std=c++2a -fexceptions)


enable_testing()


include_directories(. ././include
                    ${LLVM_INCLUDE_DIR}) 

add_executable(
  ParaCL_test
  
  Run_tests.cpp
  Test.cpp
)
target_link_libraries(
  ParaCL_test
  ParaCL
  LLVM
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ParaCL_test)


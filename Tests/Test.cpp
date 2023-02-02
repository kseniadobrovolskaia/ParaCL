#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>


::testing::AssertionResult IsEqual(std::string name_results, std::string name_answer)
{
  std::string command = "diff " + name_results + " " + name_answer + " > is_equal.txt";

  system (command.c_str());

  std::ifstream is_equal;

  is_equal.open("is_equal.txt");

  if (!(is_equal.is_open()))
  {
    std::cerr << "File \"is_equal.txt\" did not open" << std::endl;
    exit(EXIT_FAILURE);
  }

  command.clear();

  is_equal >> command;

  if (command.size() != 0)
  {
    return ::testing::AssertionFailure() << "Files are not equal";
  }
  else
  {
    return ::testing::AssertionSuccess();
  }
}


//------------------------------PARA_CL_TESTS------------------------------


TEST(Test_ParaCL, Test1)
{
  std::string name_results = "../1_test_results.txt";
  std::string name_answer = "../1_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test2)
{
  std::string name_results = "../2_test_results.txt";
  std::string name_answer = "../2_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test3)
{
  std::string name_results = "../3_test_results.txt";
  std::string name_answer = "../3_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test4)
{
  std::string name_results = "../4_test_results.txt";
  std::string name_answer = "../4_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test5)
{
  std::string name_results = "../5_test_results.txt";
  std::string name_answer = "../5_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}


//------------------------------INPUT_TESTS------------------------------


TEST(Test_Input, Test1)
{
  std::string name_results = "../1_input_results.txt";
  std::string name_answer = "../1_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test2)
{
  std::string name_results = "../2_input_results.txt";
  std::string name_answer = "../2_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test3)
{
  std::string name_results = "../3_input_results.txt";
  std::string name_answer = "../3_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test4)
{
  std::string name_results = "../4_input_results.txt";
  std::string name_answer = "../4_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test5)
{
  std::string name_results = "../5_input_results.txt";
  std::string name_answer = "../5_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}



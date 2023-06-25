#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include "Run_tests.hpp"


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


//------------------------------RUN_PARA_CL--------------------------------



TEST(RunTests, Test0)
{
  Run_tests(); // Run ParaCL and fill files "results"
}



//------------------------------PARA_CL_TESTS------------------------------


static std::string NameResults(int num_test)
{
  std::string str1 = "../../Tests/tests/results/";
  std::string str2 = "_test_results.txt";
  return  str1 + std::to_string(num_test) + str2;
}


static std::string NameAnswers(int num_test)
{
  std::string str1 = "../../Tests/tests/answers/";
  std::string str2 = "_test_answer.txt";
  return  str1 + std::to_string(num_test) + str2;
}


TEST(Test_ParaCL, Test1)
{
  EXPECT_TRUE(IsEqual(NameResults(1), NameAnswers(1)));
}

TEST(Test_ParaCL, Test2)
{
  EXPECT_TRUE(IsEqual(NameResults(2), NameAnswers(2)));
}

TEST(Test_ParaCL, Test3)
{
  EXPECT_TRUE(IsEqual(NameResults(3), NameAnswers(3)));
}

TEST(Test_ParaCL, Test4)
{
  EXPECT_TRUE(IsEqual(NameResults(4), NameAnswers(4)));
}

TEST(Test_ParaCL, Test5)
{
  EXPECT_TRUE(IsEqual(NameResults(5), NameAnswers(5)));
}

TEST(Test_ParaCL, Test6)
{
  EXPECT_TRUE(IsEqual(NameResults(6), NameAnswers(6)));
}

TEST(Test_ParaCL, Test7)
{
  EXPECT_TRUE(IsEqual(NameResults(7), NameAnswers(7)));
}

TEST(Test_ParaCL, Test8)
{
  EXPECT_TRUE(IsEqual(NameResults(8), NameAnswers(8)));
}

TEST(Test_ParaCL, Test9)
{
  EXPECT_TRUE(IsEqual(NameResults(9), NameAnswers(9)));
}

TEST(Test_ParaCL, Test10)
{
  EXPECT_TRUE(IsEqual(NameResults(10), NameAnswers(10)));
}

TEST(Test_ParaCL, Test11)
{
  EXPECT_TRUE(IsEqual(NameResults(11), NameAnswers(11)));
}

TEST(Test_ParaCL, Test12)
{
  EXPECT_TRUE(IsEqual(NameResults(12), NameAnswers(12)));
}

TEST(Test_ParaCL, Test13)
{
  EXPECT_TRUE(IsEqual(NameResults(13), NameAnswers(13)));
}

TEST(Test_ParaCL, Test14)
{
  EXPECT_TRUE(IsEqual(NameResults(14), NameAnswers(14)));
}

TEST(Test_ParaCL, Test15)
{
  EXPECT_TRUE(IsEqual(NameResults(15), NameAnswers(15)));
}


//------------------------------INPUT_TESTS------------------------------


static std::string NameInputResults(int num_test)
{
  std::string str1 = "../../Tests/input_tests/results/";
  std::string str2 = "_input_results.txt";
  return  str1 + std::to_string(num_test) + str2;
}


static std::string NameInputAnswers(int num_test)
{
  std::string str1 = "../../Tests/input_tests/answers/";
  std::string str2 = "_input_answer.txt";
  return  str1 + std::to_string(num_test) + str2;
}


TEST(Test_Input, Test1)
{
  EXPECT_TRUE(IsEqual(NameInputResults(1), NameInputAnswers(1)));
}

TEST(Test_Input, Test2)
{
  EXPECT_TRUE(IsEqual(NameInputResults(2), NameInputAnswers(2)));
}

TEST(Test_Input, Test3)
{
  EXPECT_TRUE(IsEqual(NameInputResults(3), NameInputAnswers(3)));
}

TEST(Test_Input, Test4)
{
  EXPECT_TRUE(IsEqual(NameInputResults(4), NameInputAnswers(4)));
}

TEST(Test_Input, Test5)
{
  EXPECT_TRUE(IsEqual(NameInputResults(5), NameInputAnswers(5)));
}

TEST(Test_Input, Test6)
{
  EXPECT_TRUE(IsEqual(NameInputResults(6), NameInputAnswers(6)));
}

TEST(Test_Input, Test7)
{
  EXPECT_TRUE(IsEqual(NameInputResults(7), NameInputAnswers(7)));
}

TEST(Test_Input, Test8)
{
  EXPECT_TRUE(IsEqual(NameInputResults(8), NameInputAnswers(8)));
}

TEST(Test_Input, Test9)
{
  EXPECT_TRUE(IsEqual(NameInputResults(9), NameInputAnswers(9)));
}

TEST(Test_Input, Test10)
{
  EXPECT_TRUE(IsEqual(NameInputResults(10), NameInputAnswers(10)));
}

TEST(Test_Input, Test11)
{
  EXPECT_TRUE(IsEqual(NameInputResults(11), NameInputAnswers(11)));
}

TEST(Test_Input, Test12)
{
  EXPECT_TRUE(IsEqual(NameInputResults(12), NameInputAnswers(12)));
}

TEST(Test_Input, Test13)
{
  EXPECT_TRUE(IsEqual(NameInputResults(13), NameInputAnswers(13)));
}

TEST(Test_Input, Test14)
{
  EXPECT_TRUE(IsEqual(NameInputResults(14), NameInputAnswers(14)));
}

TEST(Test_Input, Test15)
{
  EXPECT_TRUE(IsEqual(NameInputResults(15), NameInputAnswers(15)));
}

TEST(Test_Input, Test16)
{
  EXPECT_TRUE(IsEqual(NameInputResults(16), NameInputAnswers(16)));
}

TEST(Test_Input, Test17)
{
  EXPECT_TRUE(IsEqual(NameInputResults(17), NameInputAnswers(17)));
}

TEST(Test_Input, Test18)
{
  EXPECT_TRUE(IsEqual(NameInputResults(18), NameInputAnswers(18)));
}

TEST(Test_Input, Test19)
{
  EXPECT_TRUE(IsEqual(NameInputResults(19), NameInputAnswers(19)));
}

TEST(Test_Input, Test20)
{
  EXPECT_TRUE(IsEqual(NameInputResults(20), NameInputAnswers(20)));
}

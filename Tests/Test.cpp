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
  std::string name_results = "../tests/results/1_test_results.txt";
  std::string name_answer = "../tests/answers/1_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test2)
{
  std::string name_results = "../tests/results/2_test_results.txt";
  std::string name_answer = "../tests/answers/2_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test3)
{
  std::string name_results = "../tests/results/3_test_results.txt";
  std::string name_answer = "../tests/answers/3_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test4)
{
  std::string name_results = "../tests/results/4_test_results.txt";
  std::string name_answer = "../tests/answers/4_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test5)
{
  std::string name_results = "../tests/results/5_test_results.txt";
  std::string name_answer = "../tests/answers/5_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test6)
{
  std::string name_results = "../tests/results/6_test_results.txt";
  std::string name_answer = "../tests/answers/6_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test7)
{
  std::string name_results = "../tests/results/7_test_results.txt";
  std::string name_answer = "../tests/answers/7_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test8)
{
  std::string name_results = "../tests/results/8_test_results.txt";
  std::string name_answer = "../tests/answers/8_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test9)
{
  std::string name_results = "../tests/results/9_test_results.txt";
  std::string name_answer = "../tests/answers/9_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test10)
{
  std::string name_results = "../tests/results/10_test_results.txt";
  std::string name_answer = "../tests/answers/10_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test11)
{
  std::string name_results = "../tests/results/11_test_results.txt";
  std::string name_answer = "../tests/answers/11_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test12)
{
  std::string name_results = "../tests/results/12_test_results.txt";
  std::string name_answer = "../tests/answers/12_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test13)
{
  std::string name_results = "../tests/results/13_test_results.txt";
  std::string name_answer = "../tests/answers/13_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test14)
{
  std::string name_results = "../tests/results/14_test_results.txt";
  std::string name_answer = "../tests/answers/14_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_ParaCL, Test15)
{
  std::string name_results = "../tests/results/15_test_results.txt";
  std::string name_answer = "../tests/answers/15_test_answer.txt";

  EXPECT_TRUE(IsEqual(name_results, name_answer));
}


//------------------------------INPUT_TESTS------------------------------


TEST(Test_Input, Test1)
{
  std::string name_results = "../input_tests/results/1_input_results.txt";
  std::string name_answer = "../input_tests/answers/1_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test2)
{
  std::string name_results = "../input_tests/results/2_input_results.txt";
  std::string name_answer = "../input_tests/answers/2_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test3)
{
  std::string name_results = "../input_tests/results/3_input_results.txt";
  std::string name_answer = "../input_tests/answers/3_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test4)
{
  std::string name_results = "../input_tests/results/4_input_results.txt";
  std::string name_answer = "../input_tests/answers/4_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test5)
{
  std::string name_results = "../input_tests/results/5_input_results.txt";
  std::string name_answer = "../input_tests/answers/5_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test6)
{
  std::string name_results = "../input_tests/results/6_input_results.txt";
  std::string name_answer = "../input_tests/answers/6_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test7)
{
  std::string name_results = "../input_tests/results/7_input_results.txt";
  std::string name_answer = "../input_tests/answers/7_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test8)
{
  std::string name_results = "../input_tests/results/8_input_results.txt";
  std::string name_answer = "../input_tests/answers/8_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test9)
{
  std::string name_results = "../input_tests/results/9_input_results.txt";
  std::string name_answer = "../input_tests/answers/9_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test10)
{
  std::string name_results = "../input_tests/results/10_input_results.txt";
  std::string name_answer = "../input_tests/answers/10_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test11)
{
  std::string name_results = "../input_tests/results/11_input_results.txt";
  std::string name_answer = "../input_tests/answers/11_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test12)
{
  std::string name_results = "../input_tests/results/12_input_results.txt";
  std::string name_answer = "../input_tests/answers/12_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test13)
{
  std::string name_results = "../input_tests/results/13_input_results.txt";
  std::string name_answer = "../input_tests/answers/13_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test14)
{
  std::string name_results = "../input_tests/results/14_input_results.txt";
  std::string name_answer = "../input_tests/answers/14_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test15)
{
  std::string name_results = "../input_tests/results/15_input_results.txt";
  std::string name_answer = "../input_tests/answers/15_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test16)
{
  std::string name_results = "../input_tests/results/16_input_results.txt";
  std::string name_answer = "../input_tests/answers/16_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test17)
{
  std::string name_results = "../input_tests/results/17_input_results.txt";
  std::string name_answer = "../input_tests/answers/17_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test18)
{
  std::string name_results = "../input_tests/results/18_input_results.txt";
  std::string name_answer = "../input_tests/answers/18_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test19)
{
  std::string name_results = "../input_tests/results/19_input_results.txt";
  std::string name_answer = "../input_tests/answers/19_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}

TEST(Test_Input, Test20)
{
  std::string name_results = "../input_tests/results/20_input_results.txt";
  std::string name_answer = "../input_tests/answers/20_input_answer.txt";
  
  EXPECT_TRUE(IsEqual(name_results, name_answer));
}


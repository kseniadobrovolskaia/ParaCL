#ifndef RUN_TESTS_H
#define RUN_TESTS_H



/**
 * @brief Run_tests -   Starts the execution of ParaCL programs 
 * 						(15 tests for ParaCL, 20 for error handling), 
 * 						the results are written to files in the "results" directory. 
 * 						Then runs gtest to compare the files in the "results" directory with 
 * 						the files in the "answers" directory (contains the correct program output)
 */
void Run_tests();

#endif//RUN_TESTS_H

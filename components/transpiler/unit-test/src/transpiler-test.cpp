//
//  transpiler-test.cpp
//  unit-test
//
//  Created by Gautham Banasandra on 20/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "n1ql.h"
#include "test-utils.hpp"

using TestParam = std::pair<std::string, std::string>;

class TranspilerTest : public testing::TestWithParam<TestParam> {};

std::map<std::string, std::string> tc_name_input = {
  {"SimpleContinue", "./inputs/test_inputs/input_continue.txt"},
  {"NestedContinue", "./inputs/test_inputs/input_continue_nested.txt"},
  
  {"BreakContinue", "./inputs/test_inputs/input_continue_break.txt"},
  
  {"SimpleBreak", "./inputs/test_inputs/input_break.txt"},
  {"ElseBreak", "./inputs/test_inputs/input_break_else.txt"},
  {"NestedBreak", "./inputs/test_inputs/input_break_nested.txt"},
  {"SwitchBreak", "./inputs/test_inputs/input_break_switch.txt"},
  {"NestedForInBreak", "./inputs/test_inputs/input_break_nested_for_in.txt"},
  {"NestedIfBreak", "./inputs/test_inputs/input_break_nested_if.txt"},
  {"NestedForBreak", "./inputs/test_inputs/input_break_nested_for.txt"},
  {"SwitchForBreak", "./inputs/test_inputs/input_break_switch_for.txt"},
  {"Return1", "./inputs/test_inputs/input_return1.txt"},
  {"Return2", "./inputs/test_inputs/input_return2.txt"},
  {"Return3", "./inputs/test_inputs/input_return3.txt"},
  {"Return4", "./inputs/test_inputs/input_return4.txt"},
  {"Return5", "./inputs/test_inputs/input_return5.txt"},
  {"Return6", "./inputs/test_inputs/input_return6.txt"},
  {"Return7", "./inputs/test_inputs/input_return7.txt"},
  {"LabeledBreak1", "./inputs/test_inputs/input_labeled_break1.txt"},
  {"LabeledBreak2", "./inputs/test_inputs/input_labeled_break2.txt"},
  {"LabeledBreak3", "./inputs/test_inputs/input_labeled_break3.txt"},
  {"LabeledBreak4", "./inputs/test_inputs/input_labeled_break4.txt"},
  {"LabeledBreak5", "./inputs/test_inputs/input_labeled_break5.txt"},
  {"LabeledBreak6", "./inputs/test_inputs/input_labeled_break6.txt"},
  {"LabeledBreak7", "./inputs/test_inputs/input_labeled_break7.txt"},
  {"LabeledContinue1", "./inputs/test_inputs/input_labeled_continue1.txt"},
  {"LabeledContinue2", "./inputs/test_inputs/input_labeled_continue2.txt"},
  {"LabeledContinue3", "./inputs/test_inputs/input_labeled_continue3.txt"},
  {"LabeledContinue4", "./inputs/test_inputs/input_labeled_continue4.txt"},
  {"LabeledContinue5", "./inputs/test_inputs/input_labeled_continue5.txt"},
  {"LabeledContinue6", "./inputs/test_inputs/input_labeled_continue6.txt"},
  {"LabeledContinue7", "./inputs/test_inputs/input_labeled_continue7.txt"},
  {"LabeledContinue8", "./inputs/test_inputs/input_labeled_continue8.txt"},
  {"Mixed1", "./inputs/test_inputs/input_mixed1.txt"},
  {"Break3n", "./inputs/test_inputs/input_break_3n.txt"}};

std::map<std::string, std::string> tc_name_expected = {
  {"SimpleContinue", "./inputs/test_expected/expected_continue.txt"},
  {"NestedContinue", "./inputs/test_expected/expected_continue_nested.txt"},
  
  {"BreakContinue", "./inputs/test_expected/expected_continue_break.txt"},
  
  {"SimpleBreak", "./inputs/test_expected/expected_break.txt"},
  {"ElseBreak", "./inputs/test_expected/expected_break_else.txt"},
  {"NestedBreak", "./inputs/test_expected/expected_break_nested.txt"},
  {"SwitchBreak", "./inputs/test_expected/expected_break_switch.txt"},
  {"NestedForInBreak",
    "./inputs/test_expected/expected_break_nested_for_in.txt"},
  {"NestedIfBreak", "./inputs/test_expected/expected_break_nested_if.txt"},
  {"NestedForBreak", "./inputs/test_expected/expected_break_nested_for.txt"},
  {"SwitchForBreak", "./inputs/test_expected/expected_break_switch_for.txt"},
  {"Return1", "./inputs/test_expected/expected_return1.txt"},
  {"Return2", "./inputs/test_expected/expected_return2.txt"},
  {"Return3", "./inputs/test_expected/expected_return3.txt"},
  {"Return4", "./inputs/test_expected/expected_return4.txt"},
  {"Return5", "./inputs/test_expected/expected_return5.txt"},
  {"Return6", "./inputs/test_expected/expected_return6.txt"},
  {"Return7", "./inputs/test_expected/expected_return7.txt"},
  {"LabeledBreak1", "./inputs/test_expected/expected_labeled_break1.txt"},
  {"LabeledBreak2", "./inputs/test_expected/expected_labeled_break2.txt"},
  {"LabeledBreak3", "./inputs/test_expected/expected_labeled_break3.txt"},
  {"LabeledBreak4", "./inputs/test_expected/expected_labeled_break4.txt"},
  {"LabeledBreak5", "./inputs/test_expected/expected_labeled_break5.txt"},
  {"LabeledBreak6", "./inputs/test_expected/expected_labeled_break6.txt"},
  {"LabeledBreak7", "./inputs/test_expected/expected_labeled_break7.txt"},
  {"LabeledContinue1",
    "./inputs/test_expected/expected_labeled_continue1.txt"},
  {"LabeledContinue2",
    "./inputs/test_expected/expected_labeled_continue2.txt"},
  {"LabeledContinue3",
    "./inputs/test_expected/expected_labeled_continue3.txt"},
  {"LabeledContinue4",
    "./inputs/test_expected/expected_labeled_continue4.txt"},
  {"LabeledContinue5",
    "./inputs/test_expected/expected_labeled_continue5.txt"},
  {"LabeledContinue6",
    "./inputs/test_expected/expected_labeled_continue6.txt"},
  {"LabeledContinue7",
    "./inputs/test_expected/expected_labeled_continue7.txt"},
  {"LabeledContinue8",
    "./inputs/test_expected/expected_labeled_continue8.txt"},
  {"Mixed1", "./inputs/test_expected/expected_mixed1.txt"},
  {"Break3n", "./inputs/test_expected/expected_break_3n.txt"}};

TEST_P(TranspilerTest, TranspilationTest) {
  Transpiler transpiler(GetTranspilerSrc());
  std::string input = GetTestData(tc_name_input[GetParam().first]);
  std::string output = transpiler.Transpile(input, "input.js", "input.map.json",
                                            "127.0.0.1", "25000");
  std::string expected_output = GetTestData(tc_name_expected[GetParam().first]);
  
  EXPECT_GT(input.length(), 0);
  EXPECT_GT(expected_output.length(), 0);
  EXPECT_STREQ(transpiler.JsFormat(output).c_str(),
               transpiler.JsFormat(expected_output).c_str());
  std::cout << "             Passed: " << GetParam().first << '\n';
  if (GetParam().second.length() > 0) {
    std::cout << "             Summary: " << GetParam().second << '\n';
  }
}

INSTANTIATE_TEST_CASE_P(BreakStmt, TranspilerTest,
                        testing::Values(TestParam("NestedForInBreak", ""),
                                        TestParam("NestedIfBreak", ""),
                                        TestParam("ElseBreak", ""),
                                        TestParam("SwitchBreak", ""),
                                        TestParam("NestedBreak", ""),
                                        TestParam("SimpleBreak", ""),
                                        TestParam("NestedForBreak", ""),
                                        TestParam("SwitchForBreak", "")));

INSTANTIATE_TEST_CASE_P(ContinueStmt, TranspilerTest,
                        testing::Values(TestParam("SimpleContinue", ""),
                                        TestParam("NestedContinue", "")));

INSTANTIATE_TEST_CASE_P(BreakAndContinue, TranspilerTest,
                        testing::Values(TestParam("BreakContinue", "")));

INSTANTIATE_TEST_CASE_P(
                        ReturnStmt, TranspilerTest,
                        testing::Values(
                                        TestParam("Return1", "Testing for a trivial return scenario"),
                                        TestParam("Return2", "Scenario when return has an argument"),
                                        TestParam("Return3",
                                                  "Returning an anonymous function definition from iterator"),
                                        TestParam("Return4", "Returning from three levels of nested iterator"),
                                        TestParam("Return5", "Returning from two levels of nested iterator"),
                                        TestParam("Return6",
                                                  "Returning an anonymous function from 2 - iterator"),
                                        TestParam("Return7",
                                                  "An anonymous function in between from iterators")));

INSTANTIATE_TEST_CASE_P(
                        LabeledBreakStmt, TranspilerTest,
                        testing::Values(
                                        TestParam("LabeledBreak1", "Trivial labeled break"),
                                        TestParam("LabeledBreak2", "Breaking out of an iterator"),
                                        TestParam("LabeledBreak3",
                                                  "Nested for-of loops but breaking within the outer loop"),
                                        TestParam("LabeledBreak4",
                                                  "This is a case where the break must not be replaced"),
                                        TestParam("LabeledBreak5",
                                                  "Two nested iterators, breaking to the outermost loop"),
                                        TestParam("LabeledBreak6",
                                                  "Three nested iterators, breaking to the outermost loop"),
                                        TestParam("LabeledBreak7",
                                                  "Three nested iterators, breaking to the middle loop"),
                                        TestParam("Break3n", "Three levels of for-loop nesting")));

INSTANTIATE_TEST_CASE_P(
                        LabeledContinueStmt, TranspilerTest,
                        testing::Values(
                                        TestParam("LabeledContinue1", "Trivial labeled continue"),
                                        TestParam("LabeledContinue2", "Continue to a loop outside iterator"),
                                        TestParam("LabeledContinue3",
                                                  "Nested for-of loops but continue within the outer loop"),
                                        TestParam("LabeledContinue4",
                                                  "This is a case where the continue must not be replaced"),
                                        TestParam("LabeledContinue5",
                                                  "Two nested iterators, continue to the outermost loop"),
                                        TestParam("LabeledContinue6",
                                                  "Three nested iterators, continue to the outermost loop"),
                                        TestParam("LabeledContinue7",
                                                  "Three nested iterators, continue to the middle loop"),
                                        TestParam("LabeledContinue8", "Three nested iterators inside a while "
                                                  "loop, continue to while loop")));

INSTANTIATE_TEST_CASE_P(
                        MixedExitCriteria, TranspilerTest,
                        testing::Values(TestParam(
                                                  "Mixed1", "Labeled break and return statements inside iterator")));

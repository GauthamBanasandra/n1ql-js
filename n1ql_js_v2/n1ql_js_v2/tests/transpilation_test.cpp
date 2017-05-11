//
//  transpilation_test.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 21/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "jsify.hpp"
#include "test_utils.hpp"
#include "utils.hpp"
#include "v8_env.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <map>
#include <string>

typedef std::pair<std::string, std::string> TestParam;
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
    {"Return", "./inputs/test_inputs/input_return.txt"},
    {"ReturnArg", "./inputs/test_inputs/input_return_arg.txt"},
    {"ReturnFunc", "./inputs/test_inputs/input_return_func.txt"},
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
    {"Return", "./inputs/test_expected/expected_return.txt"},
    {"ReturnArg", "./inputs/test_expected/expected_return_arg.txt"},
    {"ReturnFunc", "./inputs/test_expected/expected_return_func.txt"},
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
    {"Break3n", "./inputs/test_expected/expected_break_3n.txt"}};

TEST_P(TranspilerTest, TranspilationTest) {
  std::string input = ReadFile(tc_name_input[GetParam().first]);
  std::string output = Transpile(input);
  std::string expected_output = ReadFile(tc_name_expected[GetParam().first]);

  EXPECT_GT(input.length(), 0);
  EXPECT_GT(expected_output.length(), 0);
  EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
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
        TestParam("Return", "Testing for a trivial return scenario"),
        TestParam("ReturnArg", "Scenario when return has an argument"),
        TestParam("ReturnFunc",
                  "Returning an anonymous function definition from iterator")));

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
                  "Three nested iterators, continue to the middle loop")));

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
    {"SwitchForBreak", "./inputs/test_inputs/input_break_switch_for.txt"}};

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
    {"SwitchForBreak", "./inputs/test_expected/expected_break_switch_for.txt"}};

TEST_P(TranspilerTest, TranspilationTest) {
    std::string input = ReadFile(tc_name_input[GetParam().first]);
    std::string output = Transpile(input);
    std::string expected_output = ReadFile(tc_name_expected[GetParam().first]);

//    if (GetParam().first=="ElseBreak") {
//        std::cout << "output" << std::endl << FormatJs(output) << std::endl;
//        std::cout << "expected output" << std::endl << FormatJs(expected_output) << std::endl;
//    }
    
    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    std::cout << "             Passed " << GetParam().first <<std::endl;
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

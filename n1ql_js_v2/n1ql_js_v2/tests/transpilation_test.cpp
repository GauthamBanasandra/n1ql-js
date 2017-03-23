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

TEST(TranspilationTest, SimpleBreak) {
    std::string input = ReadFile("./inputs/test_inputs/input_break.txt");
    std::string output = Transpile(input);
    std::string expected_output = ReadFile("./inputs/test_expected/expected_break.txt");
    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
}

TEST(TranspilationTest, NestedBreak) {
    std::string input = ReadFile("./inputs/test_inputs/input_break_nested.txt");
    std::string output = Transpile(input);
    std::string expected_output = ReadFile("./inputs/test_expected/expected_break_nested.txt");
    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
}

TEST(TranspilationTest, SwitchBreak) {
    std::string input = ReadFile("./inputs/test_inputs/input_break_switch.txt");
    std::string output = Transpile(input);
    std::string expected_output = ReadFile("./inputs/test_expected/expected_break_switch.txt");
    
    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
}

TEST(TranspilationTest, NestedIfBreak) {
    std::string input = ReadFile("./inputs/test_inputs/input_break_nested_if.txt");
    std::string output = Transpile(input);
    std::string expected_output = ReadFile("./inputs/test_expected/expected_break_nested_if.txt");
    
    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
}

TEST(TranspilationTest, SwitchForBreak) {
    std::string input = ReadFile("./inputs/test_inputs/input_break_switch_for.txt");
    std::string output = Transpile(input);
    std::string expected_output = ReadFile("./inputs/test_expected/expected_break_switch_for.txt");
    
    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
}

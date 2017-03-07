//
//  jsify_test.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "jsify.hpp"
#include "test_utils.hpp"
#include "utils.hpp"
#include "v8_env.hpp"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;
string FormatJs(string input);
V8Env v8env;

TEST(JsifyTest, Select) {
    string input = ReadFile("./inputs/test_inputs/input_select.txt");
    string output = parse(input.c_str());
    string expected_output =
        ReadFile("./inputs/test_expected/expected_select.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
}

string FormatJs(string input) {
    string jsFormatSrc = N1qlUtils::ReadFile(N1qlUtils::GetJsFormatPath());
    string formattedJs = v8env.Build(jsFormatSrc, input);

    return formattedJs;
}

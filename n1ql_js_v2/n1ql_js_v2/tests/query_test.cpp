//
//  query_test.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 06/06/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <iostream>
#include "test_utils.hpp"
#include "gtest/gtest.h"

typedef std::pair<std::string, std::string> TestParam;
class QueryTest : public testing::TestWithParam<TestParam> {};

std::map<std::string, std::string> tc_name_input;
std::map<std::string, std::string> tc_name_expected;

TEST_P(QueryTest, QueryStreamingTest) {
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


//
//  jsify_test.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "test_utils.hpp"
#include "utils.hpp"
#include "v8_env.hpp"
#include "gtest/gtest.h"
#include "jsify.hpp"
#include <iostream>

using namespace std;

string FormatJs(string input);
V8Env v8env;


TEST(TrivialTest, Select) {
    string input = ReadFile("./inputs/test_inputs/input_select.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_select.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Create) {
    string input = ReadFile("./inputs/test_inputs/input_create.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_create.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Upsert) {
    string input = ReadFile("./inputs/test_inputs/input_upsert.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_upsert.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Delete) {
    string input = ReadFile("./inputs/test_inputs/input_delete.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_delete.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Update) {
    string input = ReadFile("./inputs/test_inputs/input_update.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_update.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Merge) {
    string input = ReadFile("./inputs/test_inputs/input_merge.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_merge.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Insert) {
    string input = ReadFile("./inputs/test_inputs/input_insert.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_insert.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(TrivialTest, Combined) {
    string input = ReadFile("./inputs/test_inputs/input_combined.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_combined.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(QueryExclusionTest, QueryInComment) {
    string input = ReadFile("./inputs/test_inputs/input_comment.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_comment.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(QueryExclusionTest, QueryInString) {
    string input = ReadFile("./inputs/test_inputs/input_str.txt");
    string output;
    int code = parse(input.c_str(), &output);
    string expected_output =
        ReadFile("./inputs/test_expected/expected_str.txt");

    EXPECT_STREQ(FormatJs(output).c_str(), FormatJs(expected_output).c_str());
    EXPECT_EQ(code, OK);
}

TEST(ErrorTest, SelectAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_select_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_SELECT);
}

TEST(ErrorTest, SelectAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_select_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_SELECT);
}

TEST(ErrorTest, SelectAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_select_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_SELECT);
}

TEST(ErrorTest, CreateAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_create_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_CREATE);
}

TEST(ErrorTest, CreateAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_create_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_CREATE);
}

TEST(ErrorTest, CreateAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_create_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_CREATE);
}

TEST(ErrorTest, UpsertAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_upsert_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_UPSERT);
}

TEST(ErrorTest, UpsertAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_upsert_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_UPSERT);
}

TEST(ErrorTest, UpsertAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_upsert_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_UPSERT);
}

TEST(ErrorTest, DeleteAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_delete_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_DELETE);
}

TEST(ErrorTest, DeleteAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_delete_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_DELETE);
}

TEST(ErrorTest, DeleteAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_delete_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_DELETE);
}

TEST(ErrorTest, UpdateAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_update_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_UPDATE);
}

TEST(ErrorTest, UpdateAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_update_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_UPDATE);
}

TEST(ErrorTest, UpdateAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_update_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_UPDATE);
}

TEST(ErrorTest, MergeAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_merge_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_MERGE);
}

TEST(ErrorTest, MergeAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_merge_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_MERGE);
}

TEST(ErrorTest, MergeAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_merge_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_MERGE);
}

TEST(ErrorTest, InsertAsKeyword) {
    string input = ReadFile("./inputs/test_inputs/input_insert_kw.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_INSERT);
}

TEST(ErrorTest, InsertAsFunction) {
    string input = ReadFile("./inputs/test_inputs/input_insert_func.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_INSERT);
}

TEST(ErrorTest, InsertAsLabel) {
    string input = ReadFile("./inputs/test_inputs/input_insert_lbl.txt");
    string output;
    int code = parse(input.c_str(), &output);

    EXPECT_EQ(code, KWD_INSERT);
}

string FormatJs(string input) {
    string jsFormatSrc = N1qlUtils::ReadFile(N1qlUtils::GetJsFormatPath());
    string formattedJs = v8env.Build(jsFormatSrc, input);

    return formattedJs;
}

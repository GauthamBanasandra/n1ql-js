//
//  test_utils.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "test_utils.hpp"
#include "utils.hpp"
#include "v8_env.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

void MapTestData(string);

map<string, string> test_data;
string transpiler_js_src;
V8Env v8env;

string ReadFile(string filename) {
    if (test_data.size() == 0) {
        system("python /Users/gautham/projects/file_modifier/input_combine.py /Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/inputs/ > /Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/inputs/jsify_inputs.txt");
        MapTestData(N1qlUtils::GetTestInputPath());
    }

    //    ifstream file(filename);
    //    string source((istreambuf_iterator<char>(file)),
    //                  istreambuf_iterator<char>());

    return test_data[filename];
}

void MapTestData(string test_data_path) {
    ifstream test_data_file(test_data_path);
    string delimiter =
        "*********************************************************"
        "*******************************************";
    string line;
    string key;

    while (getline(test_data_file, key)) {
        string value;

        do {
            if (line != "") {
                value += line + "\n";
            }

            getline(test_data_file, line);
        } while (line != delimiter);

        test_data[key] = value;
        line = "";
    }
}

string Transpile(string source) {
    if (transpiler_js_src.length() == 0) {
        transpiler_js_src = N1qlUtils::ReadFile("/Users/gautham/projects/github/n1ql-js/n1ql_query_builder/transpiler.js");
        transpiler_js_src += N1qlUtils::ReadFile("/Users/gautham/gosrc/src/github.com/couchbase/eventing.checkin/v8_consumer/third_party/estools.js");
    }

    string transpiled_js =
        v8env.Build(transpiler_js_src, source, EXEC_TRANSPILER);
    return transpiled_js;
}

string FormatJs(string input) {
    string jsFormatSrc = N1qlUtils::ReadFile(N1qlUtils::GetJsFormatPath());
    string formattedJs = v8env.Build(jsFormatSrc, input, EXEC_JS_FORMAT);

    return formattedJs;
}

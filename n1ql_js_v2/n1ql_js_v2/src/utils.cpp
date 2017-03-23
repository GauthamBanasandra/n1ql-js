//
//  utils.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "utils.hpp"
#include <fstream>

using namespace v8;
using namespace std;

// Returns the path to transpiler.js
string N1qlUtils::GetTranspilerJsPath() {
    return "/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/src/transpiler.js";
}

// Returns the path to js_format.js (Does not exist currently - required for unit tests).
string N1qlUtils::GetJsFormatPath() {
    return "/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/src/"
           "js_format.js";
}

string N1qlUtils::GetStartupData() { return "./n1ql_js"; }

string N1qlUtils::GetTestInputPath() {
    return "/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/"
           "inputs/inputs/jsify_inputs.txt";
}

string N1qlUtils::GetEsToolsPath() {
    return "/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/src/estools.js";
}

// Reads a file from the given path and returns the content.
string N1qlUtils::ReadFile(string file_path) {
    ifstream file(file_path);
    string source((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());

    return source;
}

string N1qlUtils::GetArgAsString(const FunctionCallbackInfo<Value> &args) {
    HandleScope scope(args.GetIsolate());
    Local<Value> arg = args[0];
    String::Utf8Value arg_string(arg);

    return *arg_string;
}

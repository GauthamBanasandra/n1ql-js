//
//  main.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 06/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "jsify.hpp"
#include "utils.hpp"
#include "v8_env.hpp"
#include "query_engine.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include "n1ql_js.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
//    ifstream file("/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/n1ql_js_inputs/input_select.txt");
//    string source((istreambuf_iterator<char>(file)),
//                  istreambuf_iterator<char>());
//    const char *input = source.c_str();
//    string plain_js;
//    parse(input, &plain_js);
//    
//    string transpilerJsSrc=N1qlUtils::ReadFile(N1qlUtils::GetTranspilerJsPath());
//    V8Env v8Env;
//    string transpiled_js=v8Env.Build(transpilerJsSrc, plain_js, EXEC_TRANSPILER);
//    cout << transpiled_js << endl;
//    

//    string source=N1qlUtils::ReadFile("/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/n1ql_js_inputs/input_select.txt");
//    V8Env v8Env;
//    string result=v8Env.ExecJs(source);
//    cout << result << endl;

    string source = N1qlUtils::ReadFile("/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/n1ql_js_inputs/input_select.txt");
    string transpiled_js = TranspileAndExec(source);
    
    cout << transpiled_js << endl;
    return 0;
}

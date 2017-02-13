//
// Created by Gautham Banasandra on 10/02/17.
//
#include <iostream>
#include "n1ql_js.h"
#include "jsify/jsify.h"
#include "v8/V8Env.h"
#include "utils/Utils.h"

using namespace std;

void ExecJs(string user_code)
{
    cout << "user input" << endl << user_code << endl << endl << endl << endl;
    string plain_js = parse(user_code.c_str());
    cout << "after lex and yacc" << endl << plain_js << endl << endl << endl << endl;

    const string &query_builder_src = N1qlUtils::ReadFile(N1qlUtils::GetQueryBuilderPath());

    V8Env v8;
    plain_js = v8.Build(query_builder_src, plain_js);
    cout << "after esprima" << endl << plain_js << endl << endl << endl << endl;
    string result = v8.ExecJs(plain_js);
    cout << "after executing" << endl << result << endl << endl << endl << endl;
}
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
    string plain_js = parse(user_code.c_str());
    const string &query_builder_src = N1qlUtils::ReadFile(N1qlUtils::GetQueryBuilderPath());

    V8Env v8;
    plain_js = v8.Build(query_builder_src, plain_js);
    cout << plain_js << endl << endl;

    // Uncomment the following lines to execute the js generated above.
//    string result = v8.ExecJs(plain_js);

//    cout << "after executing" << endl << result << endl;
}
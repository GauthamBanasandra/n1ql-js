//
// Created by Gautham Banasandra on 10/02/17.
//
#include <iostream>
#include "n1ql_js.h"
#include "jsify/jsify.h"
#include "v8/V8Env.h"
#include "v8/QueryBuilder.h"
#include "utils/Utils.h"

using namespace std;

void ExecJs(string source_code)
{
    string plain_js = parse(source_code.c_str());

    /*V8Env v8;
    v8.ExecJs("./n1ql_js", plain_js);*/

    const string &query_builder_src = N1qlUtils::ReadFile(N1qlUtils::GetQueryBuilderPath());
    QueryBuilder builder;
    plain_js = builder.Build(query_builder_src, plain_js);

    cout << "n1ql_js.cpp\tcode " << plain_js << endl;
}
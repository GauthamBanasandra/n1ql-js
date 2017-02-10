//
// Created by Gautham Banasandra on 10/02/17.
//
#include <iostream>
#include "n1ql_js.h"
#include "jsify/jsify.h"
#include "v8/V8Env.h"

using namespace std;

void ExecJs(string source_code)
{
    string plain_js = parse(source_code.c_str());

    V8Env v8;
    v8.ExecJs("./n1ql_js", plain_js);

    cout << "code " << plain_js << endl;
}
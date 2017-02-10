//
// Created by Gautham Banasandra on 10/02/17.
//
#include <iostream>
#include "n1ql_js.h"
#include "jsify/jsify.h"

using namespace std;

void exec_js(string source_code)
{
    string plain_js = parse(source_code.c_str());

    cout << plain_js << endl;
}
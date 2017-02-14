//
// Created by Gautham Banasandra on 10/02/17.
//
#include <fstream>

#include "Utils.h"

using namespace v8;
using namespace std;

string N1qlUtils::GetQueryBuilderPath()
{
    return "query_exec/query_builder.js";
}

string N1qlUtils::GetStartupData()
{
    return "./n1ql_js";
}

string N1qlUtils::ReadFile(string file_path)
{
    ifstream file(file_path);
    string source((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    return source;
}

string N1qlUtils::GetArgAsString(const FunctionCallbackInfo<Value> &args)
{
    HandleScope scope(args.GetIsolate());
    Local<Value> arg = args[0];
    String::Utf8Value arg_string(arg);

    return *arg_string;
}

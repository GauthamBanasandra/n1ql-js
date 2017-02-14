//
// Created by Gautham Banasandra on 10/02/17.
//

#ifndef N1QL_JS_UTILS_H
#define N1QL_JS_UTILS_H


#include <iostream>
#include "/Users/gautham/.cbdepscache/include/v8.h"


class N1qlUtils
{
public:
    static std::string GetStartupData();

    static std::string GetQueryBuilderPath();

    static std::string ReadFile(std::string file_path);

    static std::string GetArgAsString(const v8::FunctionCallbackInfo<v8::Value> &);
};

#endif //N1QL_JS_UTILS_H

//
// Created by Gautham Banasandra on 10/02/17.
//

#ifndef N1QL_JS_UTILS_H
#define N1QL_JS_UTILS_H


#include <iostream>


class N1qlUtils
{
public:
    static std::string GetStartupData();
    static std::string GetQueryBuilderPath();
    static std::string ReadFile(std::string file_path);
};

#endif //N1QL_JS_UTILS_H

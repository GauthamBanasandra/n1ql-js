//
// Created by Gautham Banasandra on 10/02/17.
//

#ifndef N1QL_JS_V8ENV_H
#define N1QL_JS_V8ENV_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "ArrayBufferAllocator.h"
#include "/Users/gautham/.cbdepscache/include/libplatform/libplatform.h"
#include "/Users/gautham/.cbdepscache/include/v8.h"


class V8Env
{
private:
    static void LogFunction(const FunctionCallbackInfo<Value> &);
    static void IterFunction(const FunctionCallbackInfo<Value> &);

public:
    void ExecJs(std::string, std::string);
};


#endif //N1QL_JS_V8ENV_H

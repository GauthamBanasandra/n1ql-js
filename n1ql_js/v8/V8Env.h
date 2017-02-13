//
// Created by Gautham Banasandra on 10/02/17.
//

#ifndef N1QL_JS_V8ENV_H
#define N1QL_JS_V8ENV_H

#include "ArrayBufferAllocator.h"
#include "/Users/gautham/.cbdepscache/include/libplatform/libplatform.h"
#include "/Users/gautham/.cbdepscache/include/v8.h"


class V8Env
{
private:
    Isolate *isolate;
    Platform *platform;

    static void LogFunction(const FunctionCallbackInfo<Value> &);

    static void IterFunction(const FunctionCallbackInfo<Value> &);

public:
    V8Env();

    std::string ExecJs(std::string);

    std::string Build(std::string, std::string);

    Isolate *getIsolate() const;

    virtual ~V8Env();
};


#endif //N1QL_JS_V8ENV_H

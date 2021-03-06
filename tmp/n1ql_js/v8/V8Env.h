//
// Created by Gautham Banasandra on 10/02/17.
//

#ifndef N1QL_JS_V8ENV_H
#define N1QL_JS_V8ENV_H

#include "ArrayBufferAllocator.h"
#include <include/libplatform/libplatform.h>
#include <include/v8.h>


class V8Env
{
private:
    Isolate *isolate;
    Platform *platform;

    static void LogFunction(const FunctionCallbackInfo<Value> &);

    static void IterFunction(const FunctionCallbackInfo<Value> &);

    static void StopIterFunction(const FunctionCallbackInfo<Value> &);

    static void ExecQueryFunction(const FunctionCallbackInfo<Value> &);

    static void N1qlQueryConstructor(const FunctionCallbackInfo<Value> &);

public:
    V8Env();

    std::string ExecJs(std::string);

    std::string Build(std::string, std::string);

    Isolate *getIsolate() const;

    virtual ~V8Env();
};


#endif //N1QL_JS_V8ENV_H

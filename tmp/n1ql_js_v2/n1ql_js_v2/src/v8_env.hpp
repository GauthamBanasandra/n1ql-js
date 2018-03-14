//
//  v8_env.hpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef v8_env_hpp
#define v8_env_hpp

#include "../include/array_buf.h"
#include "include/libplatform/libplatform.h"
#include "include/v8.h"
enum builder_mode { EXEC_JS_FORMAT, EXEC_TRANSPILER };

class V8Env {
  private:
    Isolate *isolate;
    Platform *platform;

    static void LogFunction(const FunctionCallbackInfo<Value> &);
    
    static void SendMailFunction(const FunctionCallbackInfo<Value> &);

    static void IterFunction(const FunctionCallbackInfo<Value> &);

    static void StopIterFunction(const FunctionCallbackInfo<Value> &);

    static void ExecQueryFunction(const FunctionCallbackInfo<Value> &);

    static void N1qlQueryConstructor(const FunctionCallbackInfo<Value> &);

  public:
    V8Env();

    std::string ExecJs(std::string);

    std::string Build(std::string, std::string, int);

    Isolate *getIsolate() const;

    virtual ~V8Env();
};

#endif /* v8_env_hpp */

//
//  transpiler.cpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 24/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "n1ql.hpp"
#include <include/v8.h>

// Accepts transpiler.js source and user code as parameters and performs
// transpilation.
std::string Transpile(std::string js_src, std::string user_code, int mode) {
  std::string utf8Result;
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    
    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);
    v8::Context::Scope context_scope(context);
    
    v8::Local<v8::String> source =
    v8::String::NewFromUtf8(isolate, js_src.c_str());
    
    v8::Local<v8::Script> script =
    v8::Script::Compile(context, source).ToLocalChecked();
    
    script->Run(context).ToLocalChecked();
    v8::Local<v8::String> function_name;
    switch (mode) {
      case EXEC_JS_FORMAT:
        // Format the given JavaScript code.
        function_name = v8::String::NewFromUtf8(isolate, "jsFormat");
        break;
      case EXEC_TRANSPILER:
        // Perform transpilation.
        function_name = v8::String::NewFromUtf8(isolate, "transpile");
      default:
        break;
    }
    
    v8::Local<v8::Value> function_def = context->Global()->Get(function_name);
    v8::Local<v8::Function> function_ref =
    v8::Local<v8::Function>::Cast(function_def);
    
    // Input source for transpilation.
    v8::Local<v8::Value> args[1];
    args[0] = v8::String::NewFromUtf8(isolate, user_code.c_str());
    
    v8::Local<v8::Value> function_result =
    function_ref->Call(function_ref, 1, args);
    
    v8::String::Utf8Value utf8(function_result);
    utf8Result = *utf8;
  }
  
  return utf8Result;
}

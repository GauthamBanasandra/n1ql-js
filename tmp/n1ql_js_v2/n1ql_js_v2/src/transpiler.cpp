//
//  q_builder.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "v8_env.hpp"

// Accepts the builder code and user code as parameters and transforms
// constructs into N1QL calls.
std::string V8Env::Build(std::string js_src, std::string user_code, int mode) {
    std::string utf8Result;

    v8::Isolate *isolate = getIsolate();
    {
        v8::Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        
        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, js_src.c_str());

        // Compile the source code.
        v8::Local<v8::Script> script =
            v8::Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
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
        }
        
        v8::Local<v8::Value> function_def = context->Global()->Get(function_name);
        v8::Local<v8::Function> function_ref = v8::Local<v8::Function>::Cast(function_def);

        // Input on which the query builder should act on.
        v8::Local<v8::Value> args[1];
        args[0] = v8::String::NewFromUtf8(isolate, user_code.c_str());

        v8::Local<v8::Value> function_result = function_ref->Call(function_ref, 1, args);

        // Convert the result to an UTF8 string and print it.
        v8::String::Utf8Value utf8(function_result);
        utf8Result = *utf8;
    }

    return utf8Result;
}

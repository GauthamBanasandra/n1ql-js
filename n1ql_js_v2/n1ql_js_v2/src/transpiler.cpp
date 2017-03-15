//
//  q_builder.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "v8_env.hpp"

using namespace std;

// Accepts the builder code and user code as parameters and transforms
// constructs into N1QL calls.
string V8Env::Build(string js_src, string user_code, int mode) {
    string utf8Result;

    Isolate *isolate = getIsolate();
    {
        Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
        
        // Create a new context.
        Local<Context> context = Context::New(isolate, NULL, global);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        Local<String> source =
            String::NewFromUtf8(isolate, js_src.c_str(), NewStringType::kNormal)
                .ToLocalChecked();

        // Compile the source code.
        Local<Script> script =
            Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        script->Run(context).ToLocalChecked();

        Local<String> function_name;
        switch (mode) {
            case EXEC_JS_FORMAT:
                function_name = String::NewFromUtf8(isolate, "jsFormat");
                break;
            case EXEC_TRANSPILER:
                function_name = String::NewFromUtf8(isolate, "transpile");
            default:
                break;
        }
        
        Local<Value> function_def = context->Global()->Get(function_name);
        Local<Function> function_ref = Local<Function>::Cast(function_def);

        // Input on which the query builder should act on.
        Local<Value> args[1];
        args[0] = String::NewFromUtf8(isolate, user_code.c_str(),
                                      NewStringType::kNormal)
                      .ToLocalChecked();

        Local<Value> function_result =
            function_ref->Call(function_ref, 1, args);

        // Convert the result to an UTF8 string and print it.
        String::Utf8Value utf8(function_result);
        utf8Result = *utf8;
    }

    return utf8Result;
}

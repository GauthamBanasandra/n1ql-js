//
// Created by Gautham Banasandra on 13/02/17.
//
#include "V8Env.h"
#include "../utils/Utils.h"

using namespace std;

// This function just prints the arguments to the standard output.
void V8Env::LogFunction(const FunctionCallbackInfo<Value> &args)
{
    string value = N1qlUtils::GetArgAsString(args);

    cout << value << endl;
}

// This function accepts a function 'F' as argument and executes it as part of the parent script.
void V8Env::IterFunction(const FunctionCallbackInfo<Value> &args)
{
    string str_func = N1qlUtils::GetArgAsString(args);

    // Get the current isolate.
    Isolate *isolate = Isolate::GetCurrent();

    // Create the source code from the argument.
    Local<String> source = String::NewFromUtf8(isolate,
                                               str_func.c_str(),
                                               NewStringType::kNormal).ToLocalChecked();

    // Get the current context from the isolate.
    Local<Context> context = isolate->GetCurrentContext();

    // Compile the script.
    Local<Script> script = Script::Compile(context, source).ToLocalChecked();

    // Run
    // TODO:    Storing the result may not be necessary.
    Local<Value> result = script->Run(context).ToLocalChecked();

    // Once the script runs, we can call the function 'F'.
    // Grab the function definition for 'F'.
    Local<Value> function_def = context->Global()->Get(String::NewFromUtf8(isolate,
                                                                           "F",
                                                                           NewStringType::kNormal).ToLocalChecked());
    Local<Function> function = Local<Function>::Cast(function_def);

    // Call the function F.
    Local<Value> function_result = function->Call(function, 0, NULL);

    // TODO:    Storing the result may not be necessary.
    String::Utf8Value function_value(function_result);
}

void V8Env::N1qlFunction(const FunctionCallbackInfo<Value> &args)
{
    string query_string = N1qlUtils::GetArgAsString(args);

    cout << "query string\t" << query_string << endl;

}

/*
 * Executes the given JavaScript code.
 * Params:  js_src - The source code to execute.
 */
string V8Env::ExecJs(string js_src)
{
    string utf8Result;

    Isolate *isolate = getIsolate();
    {
        Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        // Exposing the log() function to Js.
        Local<ObjectTemplate> global_functions = ObjectTemplate::New(isolate);
        auto name_log = String::NewFromUtf8(isolate, "log", NewStringType::kNormal).ToLocalChecked();
        auto callback_log = FunctionTemplate::New(isolate, V8Env::LogFunction);
        global_functions->Set(name_log, callback_log);

        // Exposing the iter() function to Js.
        auto name_iter = String::NewFromUtf8(isolate, "iter", NewStringType::kNormal).ToLocalChecked();
        auto callback_iter = FunctionTemplate::New(isolate, V8Env::IterFunction);
        global_functions->Set(name_iter, callback_iter);

        // Exposing the n1ql() function to Js.
        auto name_n1ql = String::NewFromUtf8(isolate, "n1ql", NewStringType::kNormal).ToLocalChecked();
        auto callback_n1ql = FunctionTemplate::New(isolate, V8Env::N1qlFunction);
        global_functions->Set(name_n1ql, callback_n1ql);

        // Create a new context.
        Local<Context> context = Context::New(isolate, NULL, global_functions);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        Local<String> source = String::NewFromUtf8(isolate,
                                                   js_src.c_str(),
                                                   NewStringType::kNormal).ToLocalChecked();

        // Compile the source code.
        Local<Script> script = Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        Local<Value> result = script->Run(context).ToLocalChecked();

        // Convert the result to an UTF8 string and print it.
        String::Utf8Value utf8(result);

        utf8Result = *utf8;
    }

    return utf8Result;
}

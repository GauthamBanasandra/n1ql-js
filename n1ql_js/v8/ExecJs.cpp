//
// Created by Gautham Banasandra on 13/02/17.
//
#include <vector>
#include "V8Env.h"
#include "../query_exec/QueryEngine.h"
#include "../utils/Utils.h"

using namespace std;

// This function just prints the arguments to the standard output.
void V8Env::LogFunction(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    Local<Object> json = isolate->GetCurrentContext()
            ->Global()
            ->Get(String::NewFromUtf8(isolate, "JSON", NewStringType::kNormal).ToLocalChecked())
            ->ToObject();
    Local<Function> stringify = json
            ->Get(String::NewFromUtf8(isolate, "stringify", NewStringType::kNormal).ToLocalChecked()).As<Function>();

    Local<Value> param = args[0];
    Local<Value> result = stringify->Call(json, 1, &param);

    String::Utf8Value const str_result(result);

    cout << *str_result << endl;
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

/*
 * Constructor for N1qlQuery().
 * Accepts a string as N1QL query and returns an instance of N1qlQuery.
 * Attributes -     query - contains the query as string.
 * Methods -    exec_query() - executes the query.
 */
void V8Env::N1qlQueryConstructor(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    Local<ObjectTemplate> obj = ObjectTemplate::New();

    // Attribute name - query.
    Local<Name> query_name = String::NewFromUtf8(isolate, "query", NewStringType::kNormal).ToLocalChecked();
    Local<Value> empty_string = String::NewFromUtf8(isolate, "", NewStringType::kNormal).ToLocalChecked();

    // Initialize the attribute "name" with empty string.
    obj->Set(query_name, empty_string);

    // Method name - exec_query.
    Local<String> exec_query_name = String::NewFromUtf8(isolate, "exec_query", NewStringType::kNormal).ToLocalChecked();
    obj->Set(exec_query_name, FunctionTemplate::New(isolate, V8Env::ExecQueryFunction));

    // If the arguments to the constructor is non-empty, initialize the instance with query string.
    if (!args[0].IsEmpty() && args[0]->IsString())
        obj->Set(query_name, args[0]);

    // Return the new instance.
    args.GetReturnValue().Set(obj->NewInstance());
}

// Accepts the N1QL query and executes it.
void V8Env::ExecQueryFunction(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

    // Get the "query" attribute of the instance.
    Local<Name> query_name = String::NewFromUtf8(isolate, "query", NewStringType::kNormal).ToLocalChecked();
    Local<Value> query_value = args.This()->Get(query_name);
    String::Utf8Value query_string(query_value);

    // Execute the query.
    QueryEngine qEngine;
    vector<string> rows = qEngine.ExecQuery(*query_string);

    // TODO:    Try to return a proper object here, instead of parsing string to JSON.
    string result = "[";
    for (string row : rows)
        result += row + ",";
    result.erase(result.length() - 1);
    result += "]";

    Local<Value> json_result = JSON::Parse(String::NewFromUtf8(Isolate::GetCurrent(),
                                                               result.c_str(),
                                                               NewStringType::kNormal).ToLocalChecked());
    args.GetReturnValue().Set(json_result);
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

        Local<ObjectTemplate> global_functions = ObjectTemplate::New(isolate);

        // Exposing the log() function to Js.
        auto name_log = String::NewFromUtf8(isolate, "log", NewStringType::kNormal).ToLocalChecked();
        auto callback_log = FunctionTemplate::New(isolate, V8Env::LogFunction);
        global_functions->Set(name_log, callback_log);

        // Exposing the iter() function to Js.
        auto name_iter = String::NewFromUtf8(isolate, "iter", NewStringType::kNormal).ToLocalChecked();
        auto callback_iter = FunctionTemplate::New(isolate, V8Env::IterFunction);
        global_functions->Set(name_iter, callback_iter);

        // Exposing the N1qlQuery() constructor to Js.
        auto name_constructor = String::NewFromUtf8(isolate, "N1qlQuery", NewStringType::kNormal).ToLocalChecked();
        auto callback_constructor = FunctionTemplate::New(isolate, V8Env::N1qlQueryConstructor);
        global_functions->Set(name_constructor, callback_constructor);

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

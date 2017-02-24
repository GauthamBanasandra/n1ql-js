//
// Created by Gautham Banasandra on 13/02/17.
//
#include <vector>
#include "V8Env.h"
#include "../query_exec/QueryEngine.h"
#include "../utils/Utils.h"

using namespace std;
extern bool stop_signal;

// This function just prints the arguments to the standard output.
void V8Env::LogFunction(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = Isolate::GetCurrent();

    if (args[0]->IsString())
        cout << N1qlUtils::GetArgAsString(args) << endl;
    else
    {
        Local<Object> json = isolate->GetCurrentContext()
                ->Global()
                ->Get(String::NewFromUtf8(isolate, "JSON"))
                ->ToObject();
        Local<Function> stringify = json
                ->Get(String::NewFromUtf8(isolate, "stringify")).As<Function>();

        Local<Value> param = args[0];
        Local<Value> result = stringify->Call(json, 1, &param);

        String::Utf8Value const str_result(result);


        cout << *str_result << endl;
    }
}

void V8Env::StopIterFunction(const FunctionCallbackInfo<Value> &args)
{
    stop_signal = true;
}

// This function accepts a function as argument and executes it as part of the parent script.
void V8Env::IterFunction(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = Isolate::GetCurrent();

    // Get the "query" attribute of the instance.
    Local<Name> query_name = String::NewFromUtf8(isolate, "query", NewStringType::kNormal).ToLocalChecked();
    Local<Value> query_value = args.This()->Get(query_name);
    String::Utf8Value query_string(query_value);

    Local<Function> ffunc = Local<Function>::Cast(args[0]);

    QueryEngine qEngine;
    qEngine.ExecQuery(*query_string, ffunc);
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

    // Method name - exec_query.
    Local<String> iter_name = String::NewFromUtf8(isolate, "iter", NewStringType::kNormal).ToLocalChecked();
    obj->Set(iter_name, FunctionTemplate::New(isolate, V8Env::IterFunction));

    // Method name - exec_query.
    Local<String> stop_iter_name = String::NewFromUtf8(isolate, "stop_iter", NewStringType::kNormal).ToLocalChecked();
    obj->Set(stop_iter_name, FunctionTemplate::New(isolate, V8Env::StopIterFunction));

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

    Local<Array> result_array = Array::New(isolate, static_cast<int>(rows.size()));

    for (int i = 0; i < rows.size(); ++i)
    {
        Local<Value> json_row = JSON::Parse(String::NewFromUtf8(isolate, rows[i].c_str()));
        result_array->Set(static_cast<uint32_t>(i), json_row);
    }

    args.GetReturnValue().Set(result_array);
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

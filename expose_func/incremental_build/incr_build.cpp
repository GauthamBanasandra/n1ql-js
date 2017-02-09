//
// Created by Gautham Banasandra on 07/02/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "/Users/gautham/.cbdepscache/include/libplatform/libplatform.h"
#include "/Users/gautham/.cbdepscache/include/v8.h"

using namespace v8;
using namespace std;

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
    virtual void *Allocate(size_t length)
    {
        void *data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void *AllocateUninitialized(size_t length)
    { return malloc(length); }

    virtual void Free(void *data, size_t)
    { free(data); }
};

void LogFunction(const FunctionCallbackInfo<Value> &args)
{
    HandleScope scope(args.GetIsolate());
    Local<Value> arg = args[0];
    String::Utf8Value value(arg);

    cout << *value << endl;
}

void IterFunction(const FunctionCallbackInfo<Value> &args)
{
    HandleScope scope(args.GetIsolate());
    Local<Value> arg = args[0];
    String::Utf8Value value(arg);

    const char *source_code = string(*value).c_str();
    Isolate *isolate = Isolate::GetCurrent();

    Local<String> source = String::NewFromUtf8(isolate,
                                               source_code,
                                               NewStringType::kNormal).ToLocalChecked();
    Local<Context> context = isolate->GetCurrentContext();

    Local<Script> script = Script::Compile(context, source).ToLocalChecked();

    // Crashes if the following lines are uncommented.
    Local<Value> result = script->Run(context).ToLocalChecked();
    String::Utf8Value utf8(result);

    Local<Value> function_def = context->Global()->Get(String::NewFromUtf8(isolate,
                                                                           "F",
                                                                           NewStringType::kNormal).ToLocalChecked());
    Local<Function> function = Local<Function>::Cast(function_def);

    Local<Value> function_result = function->Call(function, 0, NULL);
    String::Utf8Value function_value(function_result);

    cout << "calling F\t" << *function_value << endl;

    cout << "calling iter\t" << *utf8 << endl;
}


int main(int argc, char *argv[])
{
    // Initialize V8.
    V8::InitializeICU();
    V8::InitializeExternalStartupData(argv[0]);
    Platform *platform = platform::CreateDefaultPlatform();
    V8::InitializePlatform(platform);
    V8::Initialize();
    // Create a new Isolate and make it the current one.
    ArrayBufferAllocator allocator;
    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = &allocator;
    Isolate *isolate = Isolate::New(create_params);
    {
        Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);

        Local<ObjectTemplate> global_functions = ObjectTemplate::New(isolate);
        auto name_log = String::NewFromUtf8(isolate, "log", NewStringType::kNormal).ToLocalChecked();
        auto callback_log = FunctionTemplate::New(isolate, LogFunction);
        global_functions->Set(name_log, callback_log);

        auto name_iter = String::NewFromUtf8(isolate, "iter", NewStringType::kNormal).ToLocalChecked();
        auto callback_iter = FunctionTemplate::New(isolate, IterFunction);
        global_functions->Set(name_iter, callback_iter);

        // Create a new context.
        Local<Context> context = Context::New(isolate, NULL, global_functions);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

        // Read the input from file.
        ifstream file_name(argv[1]);
        string src((istreambuf_iterator<char>(file_name)),
                   istreambuf_iterator<char>());

        const char *source_code = src.c_str();

        // isolate->GetCurrentContext();

        // Create a string containing the JavaScript source code.
        Local<String> source = String::NewFromUtf8(isolate,
                                                   source_code,
                                                   NewStringType::kNormal).ToLocalChecked();

        // Compile the source code.
        Local<Script> script = Script::Compile(context, source).ToLocalChecked();

        // Run the script to get the result.
        Local<Value> result = script->Run(context).ToLocalChecked();

        // Convert the result to an UTF8 string and print it.
        String::Utf8Value utf8(result);

        cout << "calling script\t" << *utf8 << endl;
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete platform;
    return 0;
}
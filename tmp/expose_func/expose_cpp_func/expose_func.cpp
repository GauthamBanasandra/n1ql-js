//
// Created by Gautham Banasandra on 01/02/17.
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

        // Exposing the log() function to Js.
        Local<ObjectTemplate> global_functions = ObjectTemplate::New(isolate);
        auto function_name = String::NewFromUtf8(isolate, "log", NewStringType::kNormal).ToLocalChecked();
        auto function_callback = FunctionTemplate::New(isolate, LogFunction);

        // Attaching LogFunction as callback to log() method.
        global_functions->Set(function_name, function_callback);

        // Create a new context.
        Local<Context> context = Context::New(isolate, NULL, global_functions);

        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);

        // Read the input from file.
        ifstream file_name(argv[1]);
        string src((istreambuf_iterator<char>(file_name)),
                   istreambuf_iterator<char>());

        const char *source_code = src.c_str();

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

        cout << *utf8 << endl;
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete platform;
    return 0;
}
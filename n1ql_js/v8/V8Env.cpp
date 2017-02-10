//
// Created by Gautham Banasandra on 10/02/17.
//

#include "V8Env.h"

using namespace std;

void V8Env::LogFunction(const FunctionCallbackInfo<Value> &args)
{
    HandleScope scope(args.GetIsolate());
    Local<Value> arg = args[0];
    String::Utf8Value value(arg);

    cout << *value << endl;
}

void V8Env::IterFunction(const FunctionCallbackInfo<Value> &args)
{
    HandleScope scope(args.GetIsolate());
    Local<Value> arg = args[0];
    String::Utf8Value str_func(arg);

    // Get the current isolate.
    Isolate *isolate = Isolate::GetCurrent();

    // Create the source code from the argument.
    Local<String> source = String::NewFromUtf8(isolate,
                                               string(*str_func).c_str(),
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
 * Executes the given JavaScript code.
 * Params:  startup_data - name of the final executable.
 *          js_src - The source code to execute.
 */
void V8Env::ExecJs(string startup_data, string js_src)
{
    // Initialize V8.
    V8::InitializeICU();
    V8::InitializeExternalStartupData(startup_data.c_str());
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
        auto name_log = String::NewFromUtf8(isolate, "log", NewStringType::kNormal).ToLocalChecked();
        auto callback_log = FunctionTemplate::New(isolate, V8Env::LogFunction);
        global_functions->Set(name_log, callback_log);

        // Exposing the iter() function to Js.
        auto name_iter = String::NewFromUtf8(isolate, "iter", NewStringType::kNormal).ToLocalChecked();
        auto callback_iter = FunctionTemplate::New(isolate, V8Env::IterFunction);
        global_functions->Set(name_iter, callback_iter);

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
//        String::Utf8Value utf8(result);
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete platform;
}

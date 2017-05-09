//
//  main.cpp
//  v8_perf
//
//  Created by Gautham Banasandra on 08/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

// Data size. Ensure that this size and the size in Js file match.
#define SIZE 10000000
// Js source file to execute.
#define SOURCE "/Users/gautham/projects/github/n1ql-js/performance/v8_perf/v8_perf/new_code.js"

using namespace v8;

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
  virtual void *Allocate(size_t length) {
    void *data = AllocateUninitialized(length);
    return data == NULL ? data : memset(data, 0, length);
  }
  virtual void *AllocateUninitialized(size_t length) { return malloc(length); }
  virtual void Free(void *data, size_t) { free(data); }
};

std::string ReadFile(std::string file_path) {
  std::ifstream file(file_path);
  std::string source((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
  return source;
}

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Function> ffunc = v8::Local<v8::Function>::Cast(args[0]);
  
  v8::Local<v8::Value> callback_args[1];
  for (int i = 0; i < SIZE; ++i) {
    callback_args[0] = v8::Number::New(isolate, 0);
    ffunc->Call(ffunc, 1, callback_args);
  }
}

const char *ToCString(const v8::String::Utf8Value &value) {
  return *value ? *value : "<std::string conversion failed>";
}

const char *ToJson(v8::Isolate *isolate, v8::Handle<v8::Value> object) {
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Object> global = context->Global();

  v8::Local<v8::Object> JSON =
      global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
  v8::Local<v8::Function> JSON_stringify = v8::Local<v8::Function>::Cast(
      JSON->Get(v8::String::NewFromUtf8(isolate, "stringify")));

  v8::Local<v8::Value> result;
  v8::Local<v8::Value> args[1];
  args[0] = {object};
  result = JSON_stringify->Call(context->Global(), 1, args);
  v8::String::Utf8Value str(result->ToString());
  return ToCString(str);
}

void Print(const v8::FunctionCallbackInfo<v8::Value> &args) {
  for (int i = 0; i < args.Length(); i++) {
    String::Utf8Value utf(args[i]);
    std::cout << *utf << ' ';
  }
  std::cout << '\n';
}

void Time(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto now = std::chrono::high_resolution_clock::now();
  auto ms = std::chrono::time_point_cast<std::chrono::microseconds>(now)
                .time_since_epoch()
                .count();

  auto number =
      v8::Number::New(v8::Isolate::GetCurrent(), static_cast<double>(ms));

  args.GetReturnValue().Set(number);
}

int main(int argc, char *argv[]) {
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

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

    global->Set(v8::String::NewFromUtf8(isolate, "log"),
                v8::FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "iter"),
                v8::FunctionTemplate::New(isolate, IterFunction));
    global->Set(v8::String::NewFromUtf8(isolate, "getTime"),
                v8::FunctionTemplate::New(isolate, Time));

    // Create a new context.
    Local<Context> context = Context::New(isolate, NULL, global);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    std::string src = ReadFile(SOURCE);
    // Create a string containing the JavaScript source code.
    Local<String> source =
        String::NewFromUtf8(isolate, src.c_str(), NewStringType::kNormal)
            .ToLocalChecked();

    // Compile the source code.
    Local<Script> script = Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    Local<Value> result = script->Run(context).ToLocalChecked();
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  delete platform;
  return 0;
}

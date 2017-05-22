//
//  main.cpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 19/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <fstream>
#include <include/libplatform/libplatform.h>
#include <include/v8.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n1ql.hpp"
#define BUILTIN                                                                \
  "/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/builtin.js"
#define SOURCE "/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/input.js"

using namespace v8;

N1QL *n1ql_handle;

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

void Print(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::HandleScope handle_scope(isolate);
  if (args[0]->IsString()) {
    v8::String::Utf8Value str(args[0]);
    std::cout << *str << '\n';
  } else {
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> global = context->Global();

    v8::Local<v8::Object> JSON =
        global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
    v8::Local<v8::Function> JSON_stringify = v8::Local<v8::Function>::Cast(
        JSON->Get(v8::String::NewFromUtf8(isolate, "stringify")));

    v8::Local<v8::Value> result;
    v8::Local<v8::Value> json_args[1];
    json_args[0] = args[0];
    result = JSON_stringify->Call(context->Global(), 1, &json_args[0]);
    v8::String::Utf8Value str(result->ToString());

    std::cout << *str << '\n';
  }
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

    n1ql_handle = new N1QL("couchbase://127.0.0.1:8091/beer-sample");

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "log"),
                v8::FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "iter"),
                v8::FunctionTemplate::New(isolate, IterFunction));
    global->Set(v8::String::NewFromUtf8(isolate, "stopIter"),
                v8::FunctionTemplate::New(isolate, StopIterFunction));
    global->Set(v8::String::NewFromUtf8(isolate, "execQuery"),
                v8::FunctionTemplate::New(isolate, ExecQueryFunction));

    // Create a new context.
    Local<Context> context = Context::New(isolate, NULL, global);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    std::string src = ReadFile(SOURCE);
    src += ReadFile(BUILTIN);

    // Create a string containing the JavaScript source code.
    Local<String> source =
        String::NewFromUtf8(isolate, src.c_str(), NewStringType::kNormal)
            .ToLocalChecked();

    // Compile the source code.
    Local<Script> script = Script::Compile(context, source).ToLocalChecked();

    // Run the script to get the result.
    Local<Value> result = script->Run(context).ToLocalChecked();

    // Convert the result to an UTF8 string and print it.
    String::Utf8Value utf8(result);
    printf("%s\n", *utf8);
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  delete platform;
  return 0;
}
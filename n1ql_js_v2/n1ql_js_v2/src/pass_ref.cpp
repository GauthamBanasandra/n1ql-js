//
//  pass_ref.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 17/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <fstream>
#include <include/libplatform/libplatform.h>
#include <include/v8.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define SOURCE "/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/pass_ref.js"

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

void Print(const v8::FunctionCallbackInfo<v8::Value> &args) {
  for (int i = 0; i < args.Length(); i++) {
    String::Utf8Value utf(args[i]);
    std::cout << *utf << ' ';
  }
  std::cout << '\n';
}

void TimerFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Local<v8::Value> params = args[0];
  if (params->IsFunction()) {
    v8::Local<v8::Function> function = params.As<v8::Function>();
    
    v8::String::Utf8Value func_name(function->GetName());
    if (func_name.length()) {
      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Local<v8::Function> timer_func_ref = context->Global()->Get(function->GetName()).As<v8::Function>();
      std::cout << "is reachable:\t" << !timer_func_ref->IsUndefined() << std::endl;
      std::cout << "name:\t" << *func_name << std::endl;
    } else {
      std::cout << "anonymous" << std::endl;
    }
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
    HandleScope handle_scope(isolate);

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "log"),
                v8::FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "createTimer"),
                v8::FunctionTemplate::New(isolate, TimerFunction));
    
    // Create a new context.
    Local<Context> context = Context::New(isolate, NULL, global);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    std::string src = ReadFile(SOURCE);
    
    // Create a string containing the JavaScript source code.
    Local<String> source = String::NewFromUtf8(isolate, src.c_str(),
                                               NewStringType::kNormal)
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

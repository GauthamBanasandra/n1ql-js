//
//  main.cpp
//  parsing_perf
//
//  Created by Gautham Banasandra on 10/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

#define SIZE 1000
#define SOURCE                                                                 \
  "/Users/gautham/projects/github/n1ql-js/performance/v8_perf/v8_perf/"        \
  "parse_json.js"

using namespace v8;

v8::Local<v8::Function> json_parse;

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

void Parse(const v8::FunctionCallbackInfo<v8::Value> &args) {  
  v8::Local<v8::Value> input = args[0];

  v8::Local<v8::Value> result = json_parse->Call(json_parse, 1, &input);
  args.GetReturnValue().Set(result);
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
  V8::InitializeICU();
  V8::InitializeExternalStartupData(argv[0]);
  Platform *platform = platform::CreateDefaultPlatform();
  V8::InitializePlatform(platform);
  V8::Initialize();

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
    global->Set(v8::String::NewFromUtf8(isolate, "json_parse_v8"),
                v8::FunctionTemplate::New(isolate, Parse));
    global->Set(v8::String::NewFromUtf8(isolate, "getTime"),
                v8::FunctionTemplate::New(isolate, Time));

    // Create a new context.
    Local<Context> context = Context::New(isolate, NULL, global);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    v8::Local<v8::Object> json =
        isolate->GetCurrentContext()
            ->Global()
            ->Get(v8::String::NewFromUtf8(isolate, "JSON"))
            ->ToObject();

    json_parse =
        json->Get(v8::String::NewFromUtf8(isolate, "parse")).As<v8::Function>();

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

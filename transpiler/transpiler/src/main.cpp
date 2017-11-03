// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "log.hpp"
#include "n1ql.h"
#include "utils.hpp"

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

#define PROJECT_ROOT "/Users/gautham/projects/github/n1ql-js/transpiler"
#define SOURCE_PATH PROJECT_ROOT "/transpiler/inputs/input1.js"
#define THIRD_PARTY_PATH PROJECT_ROOT "/transpiler/third_party"
#define TRANSPILER_JS_PATH PROJECT_ROOT "/transpiler/src/transpiler.js"
#define BUILTIN_JS_PATH PROJECT_ROOT "/transpiler/src/builtin.js"

inline std::string ReadFile(std::string path) {
  std::string line, content;
  std::ifstream file(path);
  if (file.is_open()) {
    while (getline(file, line)) {
      content += line + "\n";
    }
    
    file.close();
  } else {
    std::cout << "Unable to open file:\t" << path << std::endl;
  }
  
  return content;
}

inline std::string GetTranspilerSrc() {
  std::string transpiler_src;
  transpiler_src += ReadFile(TRANSPILER_JS_PATH) + "\n" +
  ReadFile(THIRD_PARTY_PATH "/esprima.js") + "\n" +
  ReadFile(THIRD_PARTY_PATH "/escodegen.js") + "\n" +
  ReadFile(THIRD_PARTY_PATH "/estraverse.js") + "\n";
  return transpiler_src;
}

void Log(const v8::FunctionCallbackInfo<v8::Value> &args) {
  for (auto i = 0; i < args.Length(); i++) {
    std::cout << JSONStringify(args.GetIsolate(), args[i]) << std::endl;
  }
}

void LogProper(const v8::FunctionCallbackInfo<v8::Value> &args) {
  auto isolate = args.GetIsolate();
  v8::HandleScope handle_scope(isolate);
  
  auto context = isolate->GetCurrentContext();
  auto global = context->Global();
  auto json = global->Get(v8Str(isolate, "JSON"))->ToObject();
  auto stringify =
  v8::Local<v8::Function>::Cast(json->Get(v8Str(isolate, "stringify")));
  v8::Local<v8::Value> json_args[1];
  
  for (auto i = 0; i < args.Length(); ++i) {
    json_args[0] = args[i];
    auto result = stringify->Call(stringify, 1, json_args);
    v8::String::Utf8Value msg(result);
    std::cout << ToCString(msg) << std::endl;
  }
}

int main(int argc, char *argv[]) {
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  v8::Platform *platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
  v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate *isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    
    auto global = v8::ObjectTemplate::New(isolate);
    global->Set(v8Str(isolate, "log"),
                v8::FunctionTemplate::New(isolate, LogProper));
    global->Set(v8Str(isolate, "iter"),
                v8::FunctionTemplate::New(isolate, IterFunction));
    global->Set(v8Str(isolate, "stopIter"),
                v8::FunctionTemplate::New(isolate, StopIterFunction));
    global->Set(v8Str(isolate, "execQuery"),
                v8::FunctionTemplate::New(isolate, ExecQueryFunction));
    global->Set(v8Str(isolate, "getReturnValue"),
                v8::FunctionTemplate::New(isolate, GetReturnValueFunction));
    auto context = v8::Context::New(isolate, nullptr, global);
    v8::Context::Scope context_scope(context);
    
    ConnectionPool *conn_pool = new ConnectionPool(
                                                   15, "127.0.0.1:12000", "default", "eventing", "asdasd");
    auto n1ql_handle = new N1QL(conn_pool, isolate);
    isolate->SetData(3, n1ql_handle);
    
    std::string transpiler_src = GetTranspilerSrc();
    std::string js_src = ReadFile(SOURCE_PATH);
    Transpiler transpiler(transpiler_src);
    std::string transpiled_src = transpiler.Transpile(
                                                      js_src, "input1.js", "input1.map.json", "127.0.0.1", "9090");
    std::string script_to_execute = transpiled_src + ReadFile(BUILTIN_JS_PATH);
    
    auto source = v8Str(isolate, script_to_execute.c_str());
    v8::Local<v8::Script> script =
    v8::Script::Compile(context, source).ToLocalChecked();
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    v8::String::Utf8Value utf8(result);
    printf("%s\n", *utf8);
    
    delete conn_pool;
    delete n1ql_handle;
  }
  
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete platform;
  delete create_params.array_buffer_allocator;
  return 0;
}

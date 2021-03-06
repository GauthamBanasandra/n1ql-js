// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <list>
#include <stdlib.h>
#include <string>

#include "comm.hpp"
#include "inspector_agent.h"
#include "js_exception.hpp"
#include "libplatform/libplatform.h"
#include "log.hpp"
#include "n1ql.h"
#include "utils.hpp"
#include "v8.h"

#define PROJECT_ROOT                                                           \
"/Users/gautham/projects/github/n1ql-js/components/transpiler"
#define SOURCE_PATH PROJECT_ROOT "/transpiler/inputs/input10.js"
#define THIRD_PARTY_PATH PROJECT_ROOT "/transpiler/third_party"
#define TRANSPILER_JS_PATH PROJECT_ROOT "/transpiler/src/transpiler.js"
#define BUILTIN_JS_PATH PROJECT_ROOT "/transpiler/src/builtin.js"

bool noRedact;

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

std::string GetScriptToExecute(std::string &n1ql_js_src) {
  auto isolate = v8::Isolate::GetCurrent();
  auto transpiler_src = GetTranspilerSrc();
  Transpiler transpiler(isolate, transpiler_src);
  CompilationInfo info = transpiler.Compile(std::move(n1ql_js_src));
  Transpiler::LogCompilationInfo(info);
  if (!info.compile_success) {
    throw "Compilation failed";
  }
  
  std::cout << n1ql_js_src << std::endl;
  auto jsify_info = Jsify(n1ql_js_src);
  if (jsify_info.code != kOK) {
    std::cout << "Jsify failed with code: " << jsify_info.code << std::endl;
  }
  
  std::cout << "After JSIFY:" << std::endl;
  std::cout << jsify_info.handler_code << std::endl;
  
  auto transpiled_src =
  transpiler.Transpile(jsify_info.handler_code, "input1.js",
                       "input1.map.json", "127.0.0.1", "9000");
  
  std::cout << "After transpilation:" << std::endl;
  std::cout << transpiled_src << std::endl;
  return transpiled_src + ReadFile(BUILTIN_JS_PATH);
}

std::string CompileHandler(std::string handler) {
  auto isolate = v8::Isolate::GetCurrent();
  auto transpiler = UnwrapData(isolate)->transpiler;
  auto info = transpiler->Compile(handler);
  v8::HandleScope handle_scoe(isolate);
  
  auto info_obj = v8::Object::New(isolate);
  info_obj->Set(v8Str(isolate, "language"), v8Str(isolate, info.language));
  info_obj->Set(v8Str(isolate, "compileSuccess"),
                v8::Boolean::New(isolate, info.compile_success));
  info_obj->Set(v8Str(isolate, "index"), v8::Int32::New(isolate, info.index));
  info_obj->Set(v8Str(isolate, "lineNumber"),
                v8::Int32::New(isolate, info.line_no));
  info_obj->Set(v8Str(isolate, "columnNumber"),
                v8::Int32::New(isolate, info.col_no));
  info_obj->Set(v8Str(isolate, "description"),
                v8Str(isolate, info.description));
  
  return JSONStringify(isolate, info_obj);
}

void DebugExecute(v8::Isolate *isolate, const v8::Local<v8::Context> &context, const std::string &source) {
  v8::HandleScope handle_scope(isolate);
  v8::TryCatch try_catch(isolate);
  
  auto origin_v8str = v8Str(isolate, SOURCE_PATH);
  v8::ScriptOrigin origin(origin_v8str);
  auto source_v8str = v8Str(isolate, source);
  auto script = v8::Script::Compile(source_v8str, &origin);
  script->Run();
  
  auto global = context->Global();
  auto func_ref = global->Get(v8Str(isolate, "OnUpdate"));
  auto func = func_ref.As<v8::Function>();
  func->Call(v8::Null(isolate), 0, nullptr);
}

int main(int argc, char *argv[]) {
  curl_global_init(CURL_GLOBAL_ALL);
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
    auto origin_v8_str = v8Str(isolate, SOURCE_PATH);
    v8::ScriptOrigin origin(origin_v8_str);
    
    try {
      struct Data data;
      JsException js_exception(isolate);
      data.js_exception = &js_exception;
      data.comm = new Communicator("127.0.0.1", "9300");
      data.transpiler = new Transpiler(isolate, GetTranspilerSrc());
      isolate->SetData(DATA_SLOT, &data);
      
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
      
      auto conn_pool =
      new ConnectionPool(isolate, 6, "127.0.0.1:12000", "default");
      data.n1ql_handle = new N1QL(conn_pool, isolate);
      
      auto js_src = ReadFile(SOURCE_PATH);
      auto script_to_execute = GetScriptToExecute(js_src);
      
      auto source = v8Str(isolate, script_to_execute.c_str());
      auto script = v8::Script::Compile(context, source, &origin).ToLocalChecked();
      auto result = script->Run(context).ToLocalChecked();
      v8::String::Utf8Value utf8(result);
      printf("%s\n", *utf8);

      auto agent = new inspector::Agent("127.0.0.1", "/tmp/frontend.url", 9140, [](const std::string &url) {});
      agent->Start(isolate, platform, SOURCE_PATH);
      agent->PauseOnNextJavascriptStatement("Break on start");
      
      DebugExecute(isolate, context, script_to_execute);
      
      agent->Stop();
      delete conn_pool;
      delete data.n1ql_handle;
      delete data.comm;
      delete agent;
    } catch (const char *e) {
      std::cout << e << '\n';
    }
  }
  
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete platform;
  delete create_params.array_buffer_allocator;
  
  curl_global_cleanup();
  return 0;
}

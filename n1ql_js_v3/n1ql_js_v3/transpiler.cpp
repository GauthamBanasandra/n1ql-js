//
//  transpiler.cpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 24/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "n1ql.hpp"
#include <include/v8.h>

Transpiler::Transpiler(std::string transpiler_src) {
  isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

  context = v8::Context::New(isolate, NULL, global);
  v8::Context::Scope context_scope(context);
  auto source = v8::String::NewFromUtf8(isolate, transpiler_src.c_str());
  auto script = v8::Script::Compile(context, source).ToLocalChecked();
  script->Run(context).ToLocalChecked();

  this->context = handle_scope.Escape(context);
}

v8::Local<v8::Value> Transpiler::ExecTranspiler(std::string function, v8::Local<v8::Value> args[], int args_len) {
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(context);
  auto function_name = v8::String::NewFromUtf8(isolate, function.c_str());
  auto function_def = context->Global()->Get(function_name);
  auto function_ref = v8::Local<v8::Function>::Cast(function_def);
  auto result = function_ref->Call(function_ref, args_len, args);

  return handle_scope.Escape(result);
}

std::string Transpiler::Transpile(std::string handler_code, std::string src_filename) {
  v8::Local<v8::Value> args[2];
  args[0] = v8::String::NewFromUtf8(isolate, handler_code.c_str());
  args[1] = v8::String::NewFromUtf8(isolate, src_filename.c_str());
  auto result = ExecTranspiler("transpile", args, 2);
  v8::String::Utf8Value utf8result(result);

  return *utf8result;
}

std::string Transpiler::JsFormat(std::string handler_code) {
  v8::Local<v8::Value> args[1];
  args[0] = v8::String::NewFromUtf8(isolate, handler_code.c_str());
  auto result = ExecTranspiler("jsFormat", args, 1);
  v8::String::Utf8Value utf8result(result);

  return *utf8result;
}

std::string Transpiler::GetSourceMap(std::string handler_code, std::string src_filename) {
  v8::Local<v8::Value> args[2];
  args[0] = v8::String::NewFromUtf8(isolate, handler_code.c_str());
  args[1] = v8::String::NewFromUtf8(isolate, src_filename.c_str());
  auto result = ExecTranspiler("getSourceMap", args, 2);
  v8::String::Utf8Value utf8result(result);
  
  return *utf8result;
}

bool Transpiler::IsTimerCalled(std::string handler_code) {
  v8::Local<v8::Value> args[1];
  args[0] = v8::String::NewFromUtf8(isolate, handler_code.c_str());
  auto result = ExecTranspiler("isTimerCalled", args, 1);
  auto bool_result = v8::Local<v8::Boolean>::Cast(result);

  return ToCBool(bool_result);
}

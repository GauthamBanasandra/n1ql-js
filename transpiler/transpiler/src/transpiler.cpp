// Copyright (c) 2017 Couchbase, Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an "AS IS"
// BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied. See the License for the specific language governing
// permissions and limitations under the License.

#include <iostream>
#include <v8.h>

#include "n1ql.h"
#include "utils.hpp"

Transpiler::Transpiler(const std::string &transpiler_src) {
  isolate = v8::Isolate::GetCurrent();
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
  
  context = v8::Context::New(isolate, NULL, global);
  v8::Context::Scope context_scope(context);
  auto source = v8Str(isolate, transpiler_src.c_str());
  auto script = v8::Script::Compile(context, source).ToLocalChecked();
  
  script->Run(context).ToLocalChecked();
  
  this->context = handle_scope.Escape(context);
}

v8::Local<v8::Value> Transpiler::ExecTranspiler(const std::string &function,
                                                v8::Local<v8::Value> args[],
                                                const int &args_len) {
  v8::EscapableHandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(context);
  auto function_name = v8Str(isolate, function.c_str());
  auto function_def = context->Global()->Get(function_name);
  auto function_ref = v8::Local<v8::Function>::Cast(function_def);
  auto result = function_ref->Call(function_ref, args_len, args);
  
  return handle_scope.Escape(result);
}

CompilationInfo Transpiler::Compile(const std::string &n1ql_js_src) {
  std::string js_src;
  Pos last_pos;
  std::list<InsertedCharsInfo> n1ql_pos;
  auto code = CommentN1QL(n1ql_js_src.c_str(), &js_src, &n1ql_pos, &last_pos);
  if (code != kOK) {
    std::cout << "CommentN1QL failed with code: " << code << std::endl;
  }
  
  CompilationInfo info;
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> args[1];
  args[0] = v8Str(isolate, js_src.c_str());
  auto result = ExecTranspiler("compile", args, 1);
  if (!result.IsEmpty()) {
    auto res_obj = result->ToObject();
    auto language = res_obj->Get(v8Str(isolate, "language"))->ToString();
    auto compile_status =
    res_obj->Get(v8Str(isolate, "compileSuccess"))->ToBoolean();
    auto index = res_obj->Get(v8Str(isolate, "index"))->ToInteger();
    auto line_no = res_obj->Get(v8Str(isolate, "lineNumber"))->ToInteger();
    auto col_no = res_obj->Get(v8Str(isolate, "columnNumber"))->ToInteger();
    auto description = res_obj->Get(v8Str(isolate, "description"))->ToString();
    
    info.compile_success = compile_status->Value();
    v8::String::Utf8Value lang_str(language);
    info.language = *lang_str;
    if (!info.compile_success) {
      v8::String::Utf8Value desc_str(description);
      info.description = *desc_str;
      info.index = index->Value();
      info.line_no = line_no->Value();
      info.col_no = col_no->Value();
      
      RectifyCompilationInfo(info, n1ql_pos);
      return info;
    }
  } else {
    throw "Result of ExecTranspiler is empty";
  }
  
  return info;
}

void Transpiler::RectifyCompilationInfo(CompilationInfo &info,
                                        const std::list<InsertedCharsInfo> &n1ql_pos) {
  for (const auto &pos : n1ql_pos) {
    info.index -= pos.type_len;
    if (pos.line_no == info.line_no) {
      info.col_no -= pos.type_len;
    }
  }
}

std::string Transpiler::Transpile(const std::string &handler_code,
                                  const std::string &src_filename,
                                  const std::string &src_map_name,
                                  const std::string &host_addr,
                                  const std::string &eventing_port) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> args[2];
  args[0] = v8Str(isolate, handler_code.c_str());
  args[1] = v8Str(isolate, src_filename.c_str());
  auto result = ExecTranspiler("transpile", args, 2);
  v8::String::Utf8Value utf8result(result);
  
  std::string src_transpiled = *utf8result;
  src_transpiled += "\n//# sourceMappingURL=http://" + host_addr + ":" +
  eventing_port + "/debugging/" + src_map_name;
  
  return src_transpiled;
}

std::string Transpiler::JsFormat(const std::string &handler_code) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> args[1];
  args[0] = v8Str(isolate, handler_code.c_str());
  auto result = ExecTranspiler("jsFormat", args, 1);
  v8::String::Utf8Value utf8result(result);
  
  return *utf8result;
}

std::string Transpiler::GetSourceMap(const std::string &handler_code,
                                     const std::string &src_filename) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> args[2];
  args[0] = v8Str(isolate, handler_code.c_str());
  args[1] = v8Str(isolate, src_filename.c_str());
  auto result = ExecTranspiler("getSourceMap", args, 2);
  v8::String::Utf8Value utf8result(result);
  
  return *utf8result;
}

bool Transpiler::IsTimerCalled(const std::string &handler_code) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> args[1];
  args[0] = v8Str(isolate, handler_code.c_str());
  auto result = ExecTranspiler("isTimerCalled", args, 1);
  auto bool_result = v8::Local<v8::Boolean>::Cast(result);
  
  return ToCBool(bool_result);
}

void Transpiler::LogCompilationInfo(const CompilationInfo &info) {
  if (info.compile_success) {
    std::cout << "Language: " << info.language << " Compilation successful"
    << '\n';
  } else {
    std::cout << "Language: " << info.language
    << " Syntax error Index: " << info.index
    << " Line number: " << info.line_no
    << " Column number: " << info.col_no
    << " Description: " << info.description << '\n';
  }
}

//
//  js_exception.cpp
//  transpiler
//
//  Created by Gautham Banasandra on 03/11/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "js_exception.hpp"
#include "utils.hpp"

JsException::JsException(v8::Isolate *isolate) : isolate(isolate) {
  code_str = "code";
  name_str = "name";
  desc_str = "desc";
  
  code.Reset(isolate, v8Str(isolate, code_str));
  desc.Reset(isolate, v8Str(isolate, desc_str));
  name.Reset(isolate, v8Str(isolate, name_str));
}

JsException::JsException(JsException &&exc_obj) {
  CopyMembers(std::move(exc_obj));
}

JsException &JsException::operator=(JsException &&exc_obj) {
  // This check prevents copying when the object is assigned to itself.
  if (this != &exc_obj) {
    CopyMembers(std::move(exc_obj));
  }
  
  return *this;
}

// Extracts the error name: LCB_HTTP_ERROR (0x0D) -> LCB_HTTP_ERROR
std::string JsException::ExtractErrorName(const std::string &error) {
  auto space_pos = error.find(" ");
  if (space_pos == std::string::npos) {
    return error;
  } else {
    return error.substr(0, space_pos);
  }
}

// Copies members from one object to another when an assignment is made.
void JsException::CopyMembers(JsException &&exc_obj) {
  std::swap(isolate, exc_obj.isolate);
  std::swap(code_str, exc_obj.code_str);
  std::swap(desc_str, exc_obj.desc_str);
  std::swap(name_str, exc_obj.name_str);
  
  code.Reset(isolate, v8Str(isolate, code_str));
  desc.Reset(isolate, v8Str(isolate, desc_str));
  name.Reset(isolate, v8Str(isolate, name_str));
}

// Extracts the error message, composes an exception object and throws.
void JsException::Throw(lcb_t instance, lcb_error_t error) {
  v8::HandleScope handle_scope(isolate);
  
  auto code_name = code.Get(isolate);
  auto desc_name = desc.Get(isolate);
  auto name_name = name.Get(isolate);
  
  auto code_value = v8::Number::New(isolate, lcb_get_errtype(error));
  auto name_value = v8Str(isolate, ExtractErrorName(lcb_strerror_short(error)).c_str());
  auto desc_value = v8Str(isolate, lcb_strerror(instance, error));
  
  auto exception = v8::Object::New(isolate);
  exception->Set(code_name, code_value);
  exception->Set(desc_name, desc_value);
  exception->Set(name_name, name_value);
  
  isolate->ThrowException(exception);
}

// Extracts the error messages and aggregates, composes an exception object and
// throws.
void JsException::Throw(lcb_t instance, lcb_error_t error,
                        std::vector<std::string> error_msgs) {
  v8::HandleScope handle_scope(isolate);
  
  auto code_name = code.Get(isolate);
  auto desc_name = desc.Get(isolate);
  auto name_name = name.Get(isolate);
  
  auto code_value = v8::Number::New(isolate, lcb_get_errtype(error));
  auto name_value = v8Str(isolate, ExtractErrorName(lcb_strerror_short(error)).c_str());
  
  auto desc_arr =
  v8::Array::New(isolate, static_cast<int>(error_msgs.size() + 1));
  for (std::string::size_type i = 0; i < error_msgs.size(); ++i) {
    auto desc = v8Str(isolate, error_msgs[i].c_str());
    desc_arr->Set(static_cast<uint32_t>(i), desc);
  }
  
  auto desc_value = v8Str(isolate, lcb_strerror(instance, error));
  desc_arr->Set(static_cast<uint32_t>(error_msgs.size()), desc_value);
  
  auto exception = v8::Object::New(isolate);
  exception->Set(code_name, code_value);
  exception->Set(desc_name, desc_arr);
  exception->Set(name_name, name_value);
  
  isolate->ThrowException(exception);
}

// Throws an exception with just the message (typically used for those errors
// where code and name of exception isn't available)
void JsException::Throw(const std::string &message) {
  v8::HandleScope handle_scope(isolate);
  
  auto desc_name = desc.Get(isolate);
  auto exception = v8::Object::New(isolate);
  exception->Set(desc_name, v8Str(isolate, message));
  
  isolate->ThrowException(exception);
}

JsException::~JsException() {
  code.Reset();
  desc.Reset();
}


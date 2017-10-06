//
//  utils.cpp
//  transpiler
//
//  Created by Gautham Banasandra on 06/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <unistd.h>
#include <vector>

#include <libplatform/libplatform.h>
#include <v8.h>

#include <libcouchbase/api3.h>
#include <libcouchbase/couchbase.h>

#include "log.hpp"
#include "utils.hpp"

// Extracts a C string from a V8 Utf8Value.
const char *ToCString(const v8::String::Utf8Value &value) {
  return *value ? *value : "<std::string conversion failed>";
}

bool ToCBool(const v8::Local<v8::Boolean> &value) {
  if (value.IsEmpty()) {
    LOG(logError) << "Failed to convert to bool" << '\n';
  }
  
  return value->Value();
}

v8::Local<v8::String> v8Str(v8::Isolate *isolate, const char *str) {
  return v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal).ToLocalChecked();
}

const char *JSONStringify(v8::Isolate *isolate, v8::Handle<v8::Value> object) {
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Object> global = context->Global();
  
  v8::Local<v8::Object> JSON = global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
  v8::Local<v8::Function> JSON_stringify = v8::Local<v8::Function>::Cast(JSON->Get(v8::String::NewFromUtf8(isolate, "stringify")));

  v8::Local<v8::Value> result;
  v8::Local<v8::Value> args[1];
  args[0] = {object};
  result = JSON_stringify->Call(global, 1, args);
  v8::String::Utf8Value str(result->ToString());
  
  return ToCString(str);
}

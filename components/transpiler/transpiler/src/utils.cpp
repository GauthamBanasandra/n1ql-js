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
#include <regex>

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

v8::Local<v8::String> v8Str(v8::Isolate *isolate, const std::string &str) {
  return v8::String::NewFromUtf8(isolate, str.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
}

std::string JSONStringify(v8::Isolate *isolate, const v8::Local<v8::Value> &object) {
  v8::HandleScope handle_scope(isolate);
  
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Object> global = context->Global();
  
  v8::Local<v8::Object> JSON = global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
  v8::Local<v8::Function> JSON_stringify = v8::Local<v8::Function>::Cast(JSON->Get(v8::String::NewFromUtf8(isolate, "stringify")));

  v8::Local<v8::Value> result;
  v8::Local<v8::Value> args[1];
  args[0] = object;
  result = JSON_stringify->Call(global, 1, args);
  v8::String::Utf8Value utf8_value(result->ToString());
  std::string json_str = *utf8_value;
  return json_str;
}

std::string JoinHostPort(const std::string &host, const std::string &port) {
  static std::regex ipv6re("^[0-9a-fA-F:]*:[0-9a-fA-F:]+$");
  return std::regex_match(host, ipv6re) ? "[" + host + "]:" + port
  : host + ":" + port;
}

const char *GetUsername(void *cookie, const char *host, const char *port,
                        const char *bucket) {
  auto endpoint = JoinHostPort(host, port);
  auto isolate = static_cast<v8::Isolate *>(cookie);
  auto comm = UnwrapData(isolate)->comm;
  auto info = comm->GetCreds(endpoint);
  if (!info.is_valid) {
    LOG(logError) << "Failed to get username for " << host << ":" << port
    << " err: " << info.msg << std::endl;
  }
  
  static const char *username = "";
  if (info.username != username) {
    username = strdup(info.username.c_str());
  }

  return username;
}

const char *GetPassword(void *cookie, const char *host, const char *port,
                        const char *bucket) {
  auto isolate = static_cast<v8::Isolate *>(cookie);
  auto comm = UnwrapData(isolate)->comm;
  auto endpoint = JoinHostPort(host, port);
  auto info = comm->GetCreds(endpoint);
  if (!info.is_valid) {
    LOG(logError) << "Failed to get password for " << host << ":" << port
    << " err: " << info.msg << std::endl;
  }
  
  static const char *password = "";
  if (info.password != password) {
    password = strdup(info.password.c_str());
  }
  
  return password;
}

//
//  utils.hpp
//  transpiler
//
//  Created by Gautham Banasandra on 06/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <v8.h>
#include <unordered_map>

#include "comm.hpp"

#define DATA_SLOT 0

class N1QL;
class JsException;
class Transpiler;
struct Data {
  N1QL *n1ql_handle;
  JsException *js_exception;
  Communicator *comm;
  Transpiler *transpiler;
  
  std::unordered_map<std::string, std::shared_ptr<char *>> username_store;
  std::unordered_map<std::string, std::shared_ptr<char *>> password_store;
};

inline const Data *UnwrapData(v8::Isolate *isolate) {
  return reinterpret_cast<Data *>(isolate->GetData(DATA_SLOT));
}

const char *ToCString(const v8::String::Utf8Value &value);
bool ToCBool(const v8::Local<v8::Boolean> &value);
v8::Local<v8::String> v8Str(v8::Isolate *isolate, const char *str);
v8::Local<v8::String> v8Str(v8::Isolate *isolate, const std::string &str);
std::string JSONStringify(v8::Isolate *isolate, const v8::Local<v8::Value> &object);
const char *GetUsername(void *cookie, const char *host, const char *port,
                        const char *bucket);
const char *GetPassword(void *cookie, const char *host, const char *port,
                        const char *bucket);

#endif /* utils_hpp */

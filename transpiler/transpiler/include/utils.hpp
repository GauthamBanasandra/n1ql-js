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

#define DATA_SLOT 0

class N1QL;
struct Data {
  N1QL *n1ql_handle;
};

inline const Data *UnwrapData(v8::Isolate *isolate) {
  return reinterpret_cast<Data *>(isolate->GetData(DATA_SLOT));
}

const char *ToCString(const v8::String::Utf8Value &value);
bool ToCBool(const v8::Local<v8::Boolean> &value);
v8::Local<v8::String> v8Str(v8::Isolate *isolate, const char *str);
const char *JSONStringify(v8::Isolate *isolate, v8::Handle<v8::Value> object);

#endif /* utils_hpp */

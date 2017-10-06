//
//  utils.hpp
//  transpiler
//
//  Created by Gautham Banasandra on 06/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

const char *ToCString(const v8::String::Utf8Value &value);
bool ToCBool(const v8::Local<v8::Boolean> &value);
v8::Local<v8::String> v8Str(v8::Isolate *isolate, const char *str);
const char *JSONStringify(v8::Isolate *isolate, v8::Handle<v8::Value> object);

#endif /* utils_hpp */

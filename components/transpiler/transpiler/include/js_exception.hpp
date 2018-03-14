//
//  js_exception.hpp
//  transpiler
//
//  Created by Gautham Banasandra on 03/11/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef JS_EXCEPTION_H
#define JS_EXCEPTION_H

#include <libcouchbase/couchbase.h>
#include <string>
#include <v8.h>
#include <vector>

class JsException {
private:
  // Fields of the exception.
  const char *code_str;
  const char *desc_str;
  const char *name_str;
  
  v8::Isolate *isolate;
  v8::Persistent<v8::String> code;
  v8::Persistent<v8::String> name;
  v8::Persistent<v8::String> desc;
  
  std::string ExtractErrorName(const std::string &error);
  void CopyMembers(JsException &&exc_obj);
  
public:
  JsException() {}
  JsException(v8::Isolate *isolate);
  JsException(JsException &&exc_obj);
  
  // Need to overload '=' as we have members of v8::Persistent type.
  JsException &operator=(JsException &&exc_obj);
  
  void Throw(lcb_t instance, lcb_error_t error);
  void Throw(lcb_t instance, lcb_error_t error,
             std::vector<std::string> error_msgs);
  void Throw(const std::string &message);
  
  ~JsException();
};

#endif

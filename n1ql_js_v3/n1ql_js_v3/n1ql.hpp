//
//  n1ql.hpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 19/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef N1QL_H
#define N1QL_H

#include "n1ql.hpp"
#include <include/v8.h>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include <string>
#include <vector>

class N1QL {
private:
  bool init_success = true;
  std::string conn_str;
  lcb_t instance;
  static void RowCallback(lcb_t, int, const lcb_RESPN1QL *);
  static void Error(lcb_t, const char *, lcb_error_t);
  
public:
  N1QL(std::string);
  std::vector<std::string> ExecQuery(std::string);
  void ExecQuery(std::string, v8::Local<v8::Function>);
  ~N1QL(){ lcb_destroy(instance);};
};

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &);

#endif

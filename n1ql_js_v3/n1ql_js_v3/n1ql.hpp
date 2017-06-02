//
//  n1ql.hpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 19/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef N1QL_H
#define N1QL_H

#include <include/v8.h>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include <map>
#include <queue>
#include <string>
#include <vector>

struct IterQueryHandler {
  bool stop_signal;
  v8::Local<v8::Function> callback;
  v8::Local<v8::Value> return_value;
};

struct BlockingQueryHandler {
  std::vector<std::string> rows;
};

struct QueryHandler {
  bool is_callback_set;
  IterQueryHandler *iter_handler;
  BlockingQueryHandler *block_handler;
};

class InstanceQueue {
private:
  lcb_t current_inst;
  std::queue<lcb_t> instances;

public:
  InstanceQueue(int, int);
  void Push(lcb_t);
  lcb_t Pop();
  lcb_t GetCurrentInstance() { return current_inst; }
  ~InstanceQueue();
};

class N1QL {
private:
  bool init_success = true;
  std::string conn_str;
  std::queue<lcb_t> inst_queue;
  template <typename> static void RowCallback(lcb_t, int, const lcb_RESPN1QL *);
  static void Error(lcb_t, const char *, lcb_error_t);

public:
  N1QL(std::string, int);
  void ExecQuery(std::string);
  ~N1QL();
};

enum builder_mode { EXEC_JS_FORMAT, EXEC_TRANSPILER };
std::string Transpile(std::string, std::string, int);

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &);

#endif

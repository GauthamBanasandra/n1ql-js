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
#include <stack>
#include <string>
#include <vector>

// Data type for managing iterators.
struct IterQueryHandler {
  v8::Local<v8::Function> callback;
  v8::Local<v8::Value> return_value;
};

struct BlockingQueryHandler {
  std::vector<std::string> rows;
};

union QueryHandler {
  IterQueryHandler *iter_handler;
  BlockingQueryHandler *block_handler;
};

// Pool of lcb instances and routines for pool management.
class InstancePool {
private:
  // Instance that was recently popped.
  lcb_t current_inst;
  std::queue<lcb_t> instances;

public:
  InstancePool(int, int, std::string, bool &);
  void Restore(lcb_t instance) { instances.push(instance); }
  lcb_t Acquire();
  lcb_t GetCurrentInstance() { return current_inst; }
  static void Error(lcb_t, const char *, lcb_error_t);
  ~InstancePool();
};

class N1QL {
private:
  bool init_success = true;
  template <typename>
  static void RowCallback(lcb_t, int, const lcb_RESPN1QL *);

public:
  InstancePool inst_pool;
  std::stack<QueryHandler> qhandler_stack;
  N1QL(std::string conn_str, int init_size)
      : inst_pool(init_size, 15, conn_str, init_success) {}
  template <typename>
  void ExecQuery(std::string);
  ~N1QL() {}
};

enum builder_mode { EXEC_JS_FORMAT, EXEC_TRANSPILER };
std::string Transpile(std::string, std::string, int);

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &);

#endif

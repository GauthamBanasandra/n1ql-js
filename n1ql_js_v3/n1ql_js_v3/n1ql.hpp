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
  std::string metadata;
  v8::Local<v8::Function> callback;
  v8::Local<v8::Value> return_value;
};

struct BlockingQueryHandler {
  std::string metadata;
  std::vector<std::string> rows;
};

struct QueryHandler {
  int obj_hash;
  std::string query;
  lcb_t instance = NULL;
  v8::Isolate *isolate = nullptr;
  IterQueryHandler *iter_handler = nullptr;
  BlockingQueryHandler *block_handler = nullptr;
};

// Pool of lcb instances and routines for pool management.
class ConnectionPool {
private:
  bool init_success = true;
  const int capacity;
  int inst_count;
  int inst_incr;
  std::string conn_str;
  std::string rbac_pass;
  std::queue<lcb_t> instances;
  void AddResource(int size);

public:
  ConnectionPool(int init_size, int inst_incr, int capacity,
                 std::string cb_kv_endpoint, std::string cb_source_bucket,
                 std::string rbac_user, std::string rbac_pass);
  bool GetInitStatus() { return init_success; }
  void Restore(lcb_t instance) { instances.push(instance); }
  lcb_t GetResource();
  static void Error(lcb_t instance, const char *msg, lcb_error_t err);
  ~ConnectionPool();
};

// Data structure for maintaining the operations.
// Each stack element is hashed, providing a two-way access.
class HashedStack {
  std::stack<QueryHandler> qstack;
  std::map<int, QueryHandler *> qmap;

public:
  HashedStack() {}
  void Push(QueryHandler &q_handler);
  void Pop();
  QueryHandler Top() { return qstack.top(); }
  QueryHandler *Get(int obj_hash) { return qmap[obj_hash]; }
  ~HashedStack() {}
};

class N1QL {
private:
  ConnectionPool inst_pool;
  // Callback for each row.
  template <typename>
  static void RowCallback(lcb_t instance, int callback_type,
                          const lcb_RESPN1QL *resp);

public:
  N1QL(ConnectionPool inst_pool) : inst_pool(inst_pool) {}
  HashedStack qhandler_stack;
  // Schedules operations for execution.
  template <typename> void ExecQuery(QueryHandler &q_handler);
  ~N1QL() {}
};

enum builder_mode { EXEC_JS_FORMAT, EXEC_TRANSPILER, EXEC_ITER_DEPTH };
std::string Transpile(std::string js_src, std::string user_code, int mode);

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
template <typename HandlerType, typename ResultType>
void AddQueryMetadata(HandlerType handler, v8::Isolate *isolate,
                      ResultType &result);

#endif

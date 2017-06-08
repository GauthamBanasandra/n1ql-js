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
  const int capacity;
  int inst_count;
  int inst_incr;
  std::string conn_str;
  std::queue<lcb_t> instances;
  void AddResource(int);
  
public:
  ConnectionPool(int, int, std::string);
  void Restore(lcb_t instance) { instances.push(instance); }
  lcb_t GetResource();
  static void Error(lcb_t, const char *, lcb_error_t);
  ~ConnectionPool();
};

// Data structure for maintaining the operations.
// Each stack element is hashed, providing a two-way access.
class HashedStack {
  std::stack<QueryHandler> qstack;
  std::map<int, QueryHandler *> qmap;

public:
  HashedStack() {}
  void Push(QueryHandler &);
  void Pop();
  QueryHandler Top() { return qstack.top(); }
  QueryHandler *Get(int obj_hash) { return qmap[obj_hash]; }
  ~HashedStack() {}
};

class N1QL {
private:
  ConnectionPool inst_pool;
  // Callback for each row.
  template <typename> static void RowCallback(lcb_t, int, const lcb_RESPN1QL *);

public:
  N1QL(ConnectionPool _inst_pool) : inst_pool(_inst_pool) {}
  HashedStack qhandler_stack;
  // Schedules operations for execution.
  template <typename> void ExecQuery(QueryHandler &);
  ~N1QL() {}
};

enum builder_mode { EXEC_JS_FORMAT, EXEC_TRANSPILER, EXEC_ITER_DEPTH };
std::string Transpile(std::string, std::string, int);

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &);
template <typename HandlerType, typename ResultType>
void AddQueryMetadata(HandlerType, v8::Isolate *, ResultType &);

#endif

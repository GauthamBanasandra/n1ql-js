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

struct QueryHandler {
  int obj_hash;
  std::string query;
  v8::Isolate *isolate = nullptr;
  lcb_t instance = NULL;
  IterQueryHandler *iter_handler = nullptr;
  BlockingQueryHandler *block_handler = nullptr;
};

// Pool of lcb instances and routines for pool management.
class ConnectionPool {
private:
  // Instance that was recently popped.
  std::queue<lcb_t> instances;
  
public:
  ConnectionPool(int, int, std::string, bool &);
  void Restore(lcb_t instance) { instances.push(instance); }
  lcb_t GetResource();
  static void Error(lcb_t, const char *, lcb_error_t);
  ~ConnectionPool();
};

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
  bool init_success = true;
  ConnectionPool inst_pool;
  template <typename> static void RowCallback(lcb_t, int, const lcb_RESPN1QL *);
  
public:
  N1QL(ConnectionPool _inst_pool) : inst_pool(_inst_pool) {}
  HashedStack qhandler_stack;
  template <typename> void ExecQuery(QueryHandler &);
  ~N1QL() {}
};

enum builder_mode { EXEC_JS_FORMAT, EXEC_TRANSPILER };
std::string Transpile(std::string, std::string, int);

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &);

#endif

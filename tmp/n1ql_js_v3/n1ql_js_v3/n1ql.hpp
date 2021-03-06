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

enum op_code {OK, KWD_ALTER, KWD_BUILD, KWD_CREATE, KWD_DELETE, KWD_DROP, KWD_EXECUTE, KWD_EXPLAIN, KWD_GRANT, KWD_INFER, KWD_INSERT, KWD_MERGE, KWD_PREPARE, KWD_RENAME, KWD_REVOKE, KWD_SELECT, KWD_UPDATE, KWD_UPSERT};
enum lex_op_code{JSIFY, UNILINE_N1QL};
int Jsify(const char* input, std::string *output);
int UniLineN1ql(const char *input, std::string *output);


int Jsify(const char *, std::string *);
int UniLineN1QL(const char *input, std::string *output);

// Data type for managing iterators.
struct IterQueryHandler {
  std::string metadata;
  v8::Local<v8::Function> callback;
};

struct BlockingQueryHandler {
  std::string metadata;
  std::vector<std::string> rows;
};

struct QueryHandler {
  std::string index_hash;
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
  std::string conn_str;
  std::string rbac_pass;
  std::queue<lcb_t> instances;
  void AddResource();

public:
  ConnectionPool(int capacity, std::string cb_kv_endpoint,
                 std::string cb_source_bucket, std::string rbac_user,
                 std::string rbac_pass);
  void Restore(lcb_t instance) { instances.push(instance); }
  lcb_t GetResource();
  static void Error(lcb_t instance, const char *msg, lcb_error_t err);
  ~ConnectionPool();
};

// Data structure for maintaining the operations.
// Each stack element is hashed, providing a two-way access.
class HashedStack {
  std::stack<QueryHandler> qstack;
  std::map<std::string, QueryHandler *> qmap;

public:
  HashedStack() {}
  void Push(QueryHandler &q_handler);
  void Pop();
  QueryHandler Top() { return qstack.top(); }
  QueryHandler *Get(std::string index_hash) { return qmap[index_hash]; }
  int Size() { return static_cast<int>(qstack.size()); }
  ~HashedStack() {}
};

class N1QL {
private:
  ConnectionPool *inst_pool;
  // Callback for each row.
  template <typename>
  static void RowCallback(lcb_t instance, int callback_type,
                          const lcb_RESPN1QL *resp);

public:
  N1QL(ConnectionPool *inst_pool) : inst_pool(inst_pool) {}
  HashedStack qhandler_stack;

  // Schedules operations for execution.
  template <typename> void ExecQuery(QueryHandler &q_handler);
  ~N1QL() {}
};

class Transpiler {
  v8::Isolate *isolate;
  v8::Local<v8::Context> context;

public:
  Transpiler(std::string transpiler_src);
  v8::Local<v8::Value> ExecTranspiler(std::string function, v8::Local<v8::Value> args[], int args_len);
  std::string Transpile(std::string user_code, std::string filename);
  std::string JsFormat(std::string user_code);
  std::string GetSourceMap(std::string user_code, std::string filename);
  bool IsTimerCalled(std::string user_code);
  ~Transpiler() {}
};

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
void GetReturnValueFunction(const v8::FunctionCallbackInfo<v8::Value> &args);
void SetReturnValue(const v8::FunctionCallbackInfo<v8::Value> &args,
                    v8::Local<v8::Object> return_value);

template <typename HandlerType, typename ResultType>
void AddQueryMetadata(HandlerType handler, v8::Isolate *isolate,
                      ResultType &result);
// Makes obj_hash unique by appending stack index.
std::string AppendStackIndex(int obj_hash);
// Functions to get and set the hidden value of a Js object.
bool HasKey(const v8::FunctionCallbackInfo<v8::Value> &args, std::string key);
std::string SetUniqueHash(const v8::FunctionCallbackInfo<v8::Value> &args);
std::string GetBaseHash(const v8::FunctionCallbackInfo<v8::Value> &args,
                        bool &exists);
void PushScopeStack(const v8::FunctionCallbackInfo<v8::Value> &args,
                    std::string key_hash_str, std::string value_hash_str);
bool PopScopeStack(const v8::FunctionCallbackInfo<v8::Value> &args);
std::string GetUniqueHash(const v8::FunctionCallbackInfo<v8::Value> &args);
const char *ToCString(const v8::String::Utf8Value &value);
bool ToCBool(const v8::Local<v8::Boolean> &value);
template <typename T>
v8::Local<T> ToLocal(const v8::MaybeLocal<T> &handle);


#endif

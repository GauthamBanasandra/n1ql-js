//
//  n1ql.cpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 19/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "n1ql.hpp"
#include <include/v8.h>
#include <iostream>
#include <map>
#include <stack>

extern N1QL *n1ql_handle;

std::stack<std::pair<int, QueryHandler>> qhandler_stack;
std::map<int, QueryHandler *> qhandler_map;

N1QL::N1QL(std::string _conn_str, int inst_count) {
  conn_str = _conn_str;
  
  lcb_create_st options;
  lcb_error_t err;
  memset(&options, 0, sizeof(options));
  options.version = 3;
  options.v.v3.connstr = conn_str.c_str();
  
  for (int i = 0; i < inst_count; ++i) {
    lcb_t instance;
    err = lcb_create(&instance, &options);
    if (err != LCB_SUCCESS) {
      init_success = false;
      Error(instance, "N1QL: unable to create lcb handle", err);
    }
    
    err = lcb_connect(instance);
    if (err != LCB_SUCCESS) {
      init_success = false;
      Error(instance, "N1QL: unable to connect to server", err);
    }
    
    lcb_wait(instance);
    
    err = lcb_get_bootstrap_status(instance);
    if (err != LCB_SUCCESS) {
      init_success = false;
      Error(instance, "N1QL: unable to get bootstrap status", err);
    }
    
    inst_queue.push(instance);
  }
}

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::HandleScope handleScope(isolate);
  
  v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
  v8::Local<v8::Value> query_value = args.This()->Get(query_name);
  v8::String::Utf8Value query_string(query_value);
  
  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);
  
  IterQueryHandler iter_handler;
  iter_handler.stop_signal = false;
  iter_handler.callback = func;
  QueryHandler q_handler;
  q_handler.is_callback_set = true;
  q_handler.iter_handler = &iter_handler;
  
  int obj_hash = args.This()->GetIdentityHash();
  qhandler_map[obj_hash] = &q_handler;
  std::pair<int, QueryHandler> stack_element(obj_hash, q_handler);
  qhandler_stack.push(stack_element);
  
  n1ql_handle->ExecQuery(*query_string);
  
  args.GetReturnValue().Set(iter_handler.return_value);
}

void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::EscapableHandleScope handle_scope(isolate);
  
  v8::Local<v8::Value> arg = args[0];
  
  int obj_hash = args.This()->GetIdentityHash();
  qhandler_map[obj_hash]->iter_handler->stop_signal = true;
  qhandler_map[obj_hash]->iter_handler->return_value = handle_scope.Escape(arg);
}

void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  
  v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
  v8::Local<v8::Value> query_value = args.This()->Get(query_name);
  v8::String::Utf8Value query_string(query_value);
  
  BlockingQueryHandler block_handler;
  QueryHandler q_handler;
  q_handler.is_callback_set = false;
  q_handler.block_handler = &block_handler;
  
  int obj_hash = args.This()->GetIdentityHash();
  qhandler_map[obj_hash] = &q_handler;
  std::pair<int, QueryHandler> stack_element(obj_hash, q_handler);
  qhandler_stack.push(stack_element);
  
  n1ql_handle->ExecQuery(*query_string);
  
  std::vector<std::string> &rows = block_handler.rows;
  v8::Local<v8::Array> result_array =
  v8::Array::New(isolate, static_cast<int>(rows.size()));
  
  // Populate the result array with the rows of the result.
  for (int i = 0; i < rows.size(); ++i) {
    v8::Local<v8::Value> json_row =
    v8::JSON::Parse(v8::String::NewFromUtf8(isolate, rows[i].c_str()));
    result_array->Set(static_cast<uint32_t>(i), json_row);
  }
  
  args.GetReturnValue().Set(result_array);
}

// Callback to execute for each row.
template <>
void N1QL::RowCallback<IterQueryHandler>(lcb_t instance, int callback_type,
                                         const lcb_RESPN1QL *resp) {
  QueryHandler &q_handler = qhandler_stack.top().second;
  // If stop_signal is set, then just breakout.
  if (q_handler.iter_handler->stop_signal) {
    
    lcb_breakout(instance);
    return;
  }
  
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *temp;
    asprintf(&temp, "%.*s\n", (int)resp->nrow, resp->row);
    
    v8::Local<v8::Function> callback = q_handler.iter_handler->callback;
    
    // Execute the function callback passed in JavaScript, if iter() is
    // called.
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Value> args[1];
    args[0] = v8::JSON::Parse(v8::String::NewFromUtf8(isolate, temp));
    
    callback->Call(callback, 1, args);
    
    free(temp);
  } else {
    //    std::cout << "query metadata:" << resp->row << '\n';
  }
}

// Callback to execute for each row.
template <>
void N1QL::RowCallback<BlockingQueryHandler>(lcb_t instance, int callback_type,
                                             const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *temp;
    asprintf(&temp, "%.*s\n", (int)resp->nrow, resp->row);
    
    QueryHandler q_handler = qhandler_stack.top().second;
    // Append the result to the rows vector.
    q_handler.block_handler->rows.push_back(std::string(temp));
    
    free(temp);
  } else {
    //    std::cout << "query metadata:" << resp->row << '\n';
  }
}

lcb_t N1QL::GetInstance() {
  lcb_create_st options;
  lcb_error_t err;
  memset(&options, 0, sizeof(options));
  options.version = 3;
  options.v.v3.connstr = conn_str.c_str();
  
  lcb_t instance;
  err = lcb_create(&instance, &options);
  if (err != LCB_SUCCESS) {
    init_success = false;
    Error(instance, "N1QL: unable to create lcb handle", err);
  }
  
  err = lcb_connect(instance);
  if (err != LCB_SUCCESS) {
    init_success = false;
    Error(instance, "N1QL: unable to connect to server", err);
  }
  
  lcb_wait(instance);
  
  err = lcb_get_bootstrap_status(instance);
  if (err != LCB_SUCCESS) {
    init_success = false;
    Error(instance, "N1QL: unable to get bootstrap status", err);
  }
  
  return instance;
}

void N1QL::ExecQuery(std::string query) {
//  lcb_t &instance = inst_queue.front();
//  inst_queue.pop();

  lcb_t instance = GetInstance();
  
  lcb_error_t err;
  lcb_CMDN1QL cmd = {0};
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS)
    Error(instance, "unable to build query string", err);
  
  lcb_n1p_mkcmd(n1ql_params, &cmd);
  
  cmd.callback = qhandler_stack.top().second.is_callback_set
  ? RowCallback<IterQueryHandler>
  : RowCallback<BlockingQueryHandler>;
  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS)
    Error(instance, "unable to query", err);
  
  lcb_n1p_free(n1ql_params);
  lcb_wait(instance);
  
  auto it = qhandler_map.find(qhandler_stack.top().first);
  qhandler_map.erase(it);
  qhandler_stack.pop();
//  inst_queue.push(instance);
  lcb_destroy(instance);
}

void N1QL::Error(lcb_t instance, const char *msg, lcb_error_t err) {
  std::cout << "error: " << err << " " << lcb_strerror(instance, err) << '\n';
}

N1QL::~N1QL() {
  while (!inst_queue.empty()) {
    lcb_destroy(inst_queue.front());
    inst_queue.pop();
  }
}

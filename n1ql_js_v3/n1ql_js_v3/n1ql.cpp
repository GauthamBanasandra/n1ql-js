//
//  n1ql.cpp
//  n1ql_js_v3
//
//  Created by Gautham Banasandra on 19/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "json.hpp"
#include "n1ql.hpp"
#include <include/v8.h>
#include <iostream>

using json = nlohmann::json;

extern N1QL *n1ql_handle;

InstancePool::InstancePool(int init_size, int capacity, std::string conn_str,
                           bool &init_success) {
  // Initialization of lcb instances pool.
  lcb_create_st options;
  lcb_error_t err;
  memset(&options, 0, sizeof(options));
  options.version = 3;
  options.v.v3.connstr = conn_str.c_str();
  options.v.v3.type = LCB_TYPE_BUCKET;
  options.v.v3.passwd = "asdasd";

  for (int i = 0; i < init_size; ++i) {
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

    instances.push(instance);
  }
}

lcb_t InstancePool::Acquire() {
  lcb_t instance = instances.front();
  current_inst = instance;
  instances.pop();
  return instance;
}

// Schedules an operation for iteration.
void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::HandleScope handleScope(isolate);

  // Query to run.
  v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
  v8::Local<v8::Value> query_value = args.This()->Get(query_name);
  v8::String::Utf8Value query_string(query_value);

  // Callback to execute.
  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);

  IterQueryHandler iter_handler;
  iter_handler.callback = func;
  QueryHandler q_handler;
  q_handler.iter_handler = &iter_handler;

  // Schedule the iteration.
  n1ql_handle->qhandler_stack.push(q_handler);
  n1ql_handle->ExecQuery<IterQueryHandler>(*query_string);

  args.GetReturnValue().Set(iter_handler.return_value);
}

// Stops the iteration that is currently executing.
void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = args.GetIsolate();
  v8::EscapableHandleScope handle_scope(isolate);

  // Set the return value of the iterator.
  v8::Local<v8::Value> arg = args[0];
  IterQueryHandler *iter_handler =
      n1ql_handle->qhandler_stack.top().iter_handler;
  iter_handler->return_value = handle_scope.Escape(arg);

  // Cancel the query and stop the RowCallback.
  lcb_t instance = n1ql_handle->inst_pool.GetCurrentInstance();
  lcb_N1QLHANDLE *handle = (lcb_N1QLHANDLE *)lcb_get_cookie(instance);
  lcb_n1ql_cancel(instance, *handle);
}

// Schedules a blocking query operation.
void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);

  v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
  v8::Local<v8::Value> query_value = args.This()->Get(query_name);
  v8::String::Utf8Value query_string(query_value);

  BlockingQueryHandler block_handler;
  QueryHandler q_handler;
  q_handler.block_handler = &block_handler;

  // Schedule blocking query.
  n1ql_handle->qhandler_stack.push(q_handler);
  n1ql_handle->ExecQuery<BlockingQueryHandler>(*query_string);

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
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *row_str;
    asprintf(&row_str, "%.*s\n", (int)resp->nrow, resp->row);

    QueryHandler &q_handler = n1ql_handle->qhandler_stack.top();
    v8::Local<v8::Function> callback = q_handler.iter_handler->callback;

    // Execute the function callback passed in JavaScript.
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Value> args[1];
    args[0] = v8::JSON::Parse(v8::String::NewFromUtf8(isolate, row_str));

    callback->Call(callback, 1, args);

    free(row_str);
  } else {
//    std::cout << "query metadata:" << resp->row << '\n';
  }
}

// Callback to execute for each row.
template <>
void N1QL::RowCallback<BlockingQueryHandler>(lcb_t instance, int callback_type,
                                             const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *row_str;
    asprintf(&row_str, "%.*s\n", (int)resp->nrow, resp->row);

    QueryHandler &q_handler = n1ql_handle->qhandler_stack.top();
    // Append the result to the rows vector.
    q_handler.block_handler->rows.push_back(std::string(row_str));

    free(row_str);
  } else {
//    std::cout << "query metadata:" << resp->row << '\n';
  }
}

template <typename HandlerType> void N1QL::ExecQuery(std::string query) {
  lcb_t instance = inst_pool.Acquire();

  lcb_error_t err;
  lcb_CMDN1QL cmd = {0};
  lcb_N1QLHANDLE handle = NULL;
  cmd.handle = &handle;
  cmd.callback = RowCallback<HandlerType>;

  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS)
    InstancePool::Error(instance, "unable to build query string", err);

  lcb_n1p_mkcmd(n1ql_params, &cmd);

  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS)
    InstancePool::Error(instance, "unable to query", err);

  lcb_n1p_free(n1ql_params);

  // Set the N1QL handle as cookie for instance - allow for query cancellation.
  lcb_set_cookie(instance, &handle);
  lcb_wait(instance);

  qhandler_stack.pop();

  inst_pool.Restore(instance);
}

void InstancePool::Error(lcb_t instance, const char *msg, lcb_error_t err) {
  std::cout << "error: " << err << " " << lcb_strerror(instance, err) << '\n';
}

InstancePool::~InstancePool() {
  while (instances.empty()) {
    lcb_destroy(instances.front());
    instances.pop();
  }
}

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

extern N1QL *n1ql_handle;
std::vector<std::string> rows;
v8::Local<v8::Function> callback;
bool is_callback_set = false;

N1QL::N1QL(std::string _conn_str) {
  conn_str = _conn_str;

  lcb_create_st options;
  lcb_error_t err;
  memset(&options, 0, sizeof(options));
  options.version = 3;
  options.v.v3.connstr = conn_str.c_str();

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
}

void IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);

  v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
  v8::Local<v8::Value> query_value = args.This()->Get(query_name);
  v8::String::Utf8Value query_string(query_value);

  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[0]);

  // Pass the function to callback and execute the query.
  n1ql_handle->ExecQuery(*query_string, func);
}

void StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Local<v8::Value> arg = args[0];
  v8::String::Utf8Value arg_string(arg);
  std::cout << "stopIter:\t" << *arg_string << std::endl;
}

void ExecQueryFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);

  v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
  v8::Local<v8::Value> query_value = args.This()->Get(query_name);
  v8::String::Utf8Value query_string(query_value);

  std::vector<std::string> rows = n1ql_handle->ExecQuery(*query_string);

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

void N1QL::ExecQuery(std::string query, v8::Local<v8::Function> function) {
  is_callback_set = true;
  callback = function;
  ExecQuery(query);
}

// TODO: Pull out 'rows.clear' and 'return rows;' and turn that to a separate
// function.
std::vector<std::string> N1QL::ExecQuery(std::string query) {
  rows.clear();

  lcb_error_t err;
  lcb_CMDN1QL cmd = {0};
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS)
    Error(instance, "unable to build query string", err);

  lcb_n1p_mkcmd(n1ql_params, &cmd);

  cmd.callback = RowCallback;
  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS)
    Error(instance, "unable to query", err);

  lcb_n1p_free(n1ql_params);
  lcb_wait(instance);

  return rows;
}

// Callback to execute for each row.
void N1QL::RowCallback(lcb_t instance, int callback_type,
                       const lcb_RESPN1QL *resp) {
  // If stop_signal is set, then just breakout.
  //  if (stop_signal) {
  //    lcb_breakout(instance);
  //    return;
  //  }

  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *temp;
    asprintf(&temp, "%.*s\n", (int)resp->nrow, resp->row);
    
    // Execute the function callback passed in JavaScript, if iter() is
    // called.
    if (is_callback_set) {
      v8::Isolate *isolate = v8::Isolate::GetCurrent();
      v8::Local<v8::Object> json =
          isolate->GetCurrentContext()
              ->Global()
              ->Get(v8::String::NewFromUtf8(isolate, "JSON"))
              ->ToObject();
      v8::Local<v8::Function> parse =
          json->Get(v8::String::NewFromUtf8(isolate, "parse"))
              .As<v8::Function>();

      v8::Local<v8::Value> args[1];
      args[0] = v8::String::NewFromUtf8(isolate, temp);
      args[0] = parse->Call(json, 1, &args[0]);

      callback->Call(callback, 1, args);
    } else {
      // If it is not a callback - execQuery() is called, append the
      // result to the rows vector.
      rows.push_back(std::string(temp));
    }

    free(temp);
  } else {
    std::cout << "query metadata:" << resp->row << '\n';
  }
}

void N1QL::Error(lcb_t instance, const char *msg, lcb_error_t err) {
  std::cout << "error: " << err << " " << lcb_strerror(instance, err) << '\n';
}

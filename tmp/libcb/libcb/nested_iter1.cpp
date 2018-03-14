//
//  nested_iter.cpp
//  libcb
//
//  Created by Gautham Banasandra on 17/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>

#include <stdio.h>
#include <string>

void end(lcb_t, const char *, lcb_error_t);
void row_callback(lcb_t, int, const lcb_RESPN1QL *);
void row_callback_inner(lcb_t, int, const lcb_RESPN1QL *);
void make_another_query();

int main() {
  lcb_t instance;
  lcb_create_st options;
  lcb_error_t err;

  memset(&options, 0, sizeof(options));
  options.version = 3;
  options.v.v3.connstr = "couchbase://localhost:8091/beer-sample";

  err = lcb_create(&instance, &options);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to create handle", err);
  }

  err = lcb_connect(instance);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to connect", err);
  }

  lcb_wait(instance);

  std::string query = "SELECT name FROM `beer-sample` limit 10;";

  lcb_CMDN1QL cmd;
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();

  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS) {
    end(instance, "unable to build query string", err);
  }

  lcb_n1p_mkcmd(n1ql_params, &cmd);
  cmd.callback = row_callback;
  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to query", err);
  }

  lcb_n1p_free(n1ql_params);
  lcb_wait(instance);

  lcb_destroy(instance);
  return 0;
}

void end(lcb_t instance, const char *msg, lcb_error_t err) {
  fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err,
          lcb_strerror(instance, err));
  exit(EXIT_FAILURE);
}

void row_callback(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("outer:\t%.*s\n", (int)resp->nrow, resp->row);

    // Making another query here.
    make_another_query();
  } else {
    printf("metadata\t %.*s\n", (int)resp->nrow, resp->row);
  }
}

void row_callback_inner(lcb_t instance, int callback_type,
                        const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("inner:\t%.*s\n", (int)resp->nrow, resp->row);
  }
}

void make_another_query(){
  lcb_t instance_inner;
  lcb_create_st options_inner;
  lcb_error_t err_inner;
  
  memset(&options_inner, 0, sizeof(options_inner));
  options_inner.version = 3;
  options_inner.v.v3.connstr = "couchbase://localhost:8091/beer-sample";
  
  err_inner = lcb_create(&instance_inner, &options_inner);
  if (err_inner != LCB_SUCCESS) {
    end(instance_inner, "unable to create handle", err_inner);
  }
  
  err_inner = lcb_connect(instance_inner);
  if (err_inner != LCB_SUCCESS) {
    end(instance_inner, "unable to connect", err_inner);
  }
  
  lcb_wait(instance_inner);
  
  std::string query = "SELECT city, code FROM `beer-sample` limit 3;";
  
  lcb_CMDN1QL cmd_inner;
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  
  err_inner = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err_inner != LCB_SUCCESS) {
    end(instance_inner, "unable to build query string", err_inner);
  }
  
  lcb_n1p_mkcmd(n1ql_params, &cmd_inner);
  cmd_inner.callback = row_callback_inner;
  err_inner = lcb_n1ql_query(instance_inner, NULL, &cmd_inner);
  if (err_inner != LCB_SUCCESS) {
    end(instance_inner, "unable to query", err_inner);
  }
  
  lcb_n1p_free(n1ql_params);
  lcb_wait(instance_inner);
  
  lcb_destroy(instance_inner);
}

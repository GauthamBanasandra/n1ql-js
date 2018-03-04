//
//  nested_iter.cpp
//  libcb
//
//  Created by Gautham Banasandra on 17/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "libcouchbase/couchbase.h"
#include "libcouchbase/n1ql.h"

#include <stdio.h>
#include <iostream>

void end(lcb_t, const char *, lcb_error_t);
void row_callback(lcb_t, int, const lcb_RESPN1QL *);
void row_callback_inner(lcb_t, int, const lcb_RESPN1QL *);
lcb_t make_query(std::string);

int main() {
  std::string query = "SELECT name FROM `beer-sample` limit 10;";
  
  lcb_t instance = make_query(query);
  
  lcb_wait(instance);
  lcb_destroy(instance);
  
  return 0;
}

lcb_t make_query(std::string query){
  lcb_t instance = NULL;
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

  lcb_CMDN1QL cmd;
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  
  // debug.
//  std::cout << query << std::endl;
  query += "";
  
  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS) {
    end(instance, "unable to build query string", err);
  }
  
  lcb_n1p_mkcmd(n1ql_params, &cmd);
  cmd.callback = row_callback;
  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS){
    end(instance, "unable to query", err);
  }
  
  lcb_n1p_free(n1ql_params);
	
  return instance;
}

void end(lcb_t instance, const char *msg, lcb_error_t err) {
  fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err,
          lcb_strerror(instance, err));
  exit(EXIT_FAILURE);
}

void row_callback(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("outer:\t%.*s\n", (int)resp->nrow, resp->row);
  } else {
    printf("metadata\t %.*s\n", (int)resp->nrow, resp->row);
  }
}

void row_callback_inner(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("inner:\t%.*s\n", (int)resp->nrow, resp->row);
  } else {
    //    printf("metadata\t %.*s\n", (int)resp->nrow, resp->row);
  }
}

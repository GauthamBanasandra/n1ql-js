//
//  nested_iter.cpp
//  libcb
//
//  Created by Gautham Banasandra on 17/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <iostream>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

std::map<std::string, lcb_t> queries;

void end(lcb_t instance, const char *msg, lcb_error_t err) {
  fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err,
          lcb_strerror(instance, err));
  exit(EXIT_FAILURE);
}

void row_callback(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("outer:\t%.*s\n", (int)resp->nrow, resp->row);
    lcb_wait(queries["query2"]);
  } else {
    printf("outer:\tmetadata\t %.*s\n", (int)resp->nrow, resp->row);
  }
}

void row_callback_inner(lcb_t instance, int callback_type,
                        const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("inner:\t%.*s\n", (int)resp->nrow, resp->row);
  } else {
    printf("inner:\tmetadata\t %.*s\n", (int)resp->nrow, resp->row);
  }
}

lcb_t make_query(std::string query, bool inner) {
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

  //  std::cout << query.c_str() << std::endl;
  char *mquery = new char[query.length() + 1];
  strcpy(mquery, query.c_str());
  err = lcb_n1p_setstmtz(n1ql_params, mquery);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to build query string", err);
  }

  lcb_n1p_mkcmd(n1ql_params, &cmd);
  if (inner) {
    cmd.callback = row_callback_inner;
  } else {
    cmd.callback = row_callback;
  }

  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to query", err);
  }

  lcb_n1p_free(n1ql_params);
  delete [] mquery;
  
  return instance;
}

int main() {
  std::string query1 = "SELECT name FROM `beer-sample` limit 10;";
  queries["query1"] = make_query(query1, false);

  std::string query2 = "SELECT name FROM `beer-sample` limit 1;";
  queries["query2"] = make_query(query2, true);

  lcb_wait(queries["query1"]);

  lcb_destroy(queries["query1"]);
  lcb_destroy(queries["query2"]);

  return 0;
}

//
//  pos_param.cpp
//  libcb
//
//  Created by Gautham Banasandra on 04/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <iostream>
#include <string>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>

static void end(lcb_t, const char *, lcb_error_t);

static void row_callback(lcb_t, int, const lcb_RESPN1QL *);

using namespace std;

int row_count = 0;

static void end(lcb_t instance, const char *msg, lcb_error_t err) {
  fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err,
          lcb_strerror(instance, err));
  exit(EXIT_FAILURE);
}

static void row_callback(lcb_t instance, int callback_type,
                         const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    ++row_count;
    if (row_count == 2)
      printf("count is 2\n");
    printf("count=%d\trow\t %.*s\n", row_count, (int)resp->nrow, resp->row);
  } else
    printf("metadata\t %.*s\n", (int)resp->nrow, resp->row);
}

int main() {
  // Couchbase handle instance. Connects to a bucket.
  lcb_t instance = nullptr;
  lcb_create_st options;
  lcb_error_t err;
  
  // Allocate memory for the handle.
  memset(&options, 0, sizeof(options));
  options.version = 3;
  options.v.v3.connstr = "couchbase://127.0.0.1:12000/"
  "default?username=Administrator&select_bucket=true&"
  "detailed_errcodes=1";
  options.v.v3.type = LCB_TYPE_BUCKET;
  options.v.v3.passwd = "asdasd";
  
  // Initialize the handle.
  err = lcb_create(&instance, &options);
  if (err != LCB_SUCCESS)
    end(instance, "unable to create handle", err);
  
  // Initialize the parameters for connection.
  err = lcb_connect(instance);
  if (err != LCB_SUCCESS)
    end(instance, "unable to connect to server", err);
  
  // Block till the connection is established.
  lcb_wait(instance);
  
  // Check if the connection was successful.
  err = lcb_get_bootstrap_status(instance);
  if (err != LCB_SUCCESS)
    end(instance, "unable to get bootstrap status", err);
  
  // Structure for writing the query.
  lcb_CMDN1QL cmd = {0};
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  std::string query =
  "SELECT * FROM $1 WHERE name = \"Rochefort 10\"";
  std::string name = "`beer-sample`";
  
  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS)
    end(instance, "unable to build query string", err);
  
  err = lcb_n1p_posparam(n1ql_params, name.c_str(), name.length());
  if (err != LCB_SUCCESS) {
    end(instance, "unable to set positional parameter", err);
  }
  
  // Build the query.
  lcb_n1p_mkcmd(n1ql_params, &cmd);
  
  // Set the callback to be invoked for fetching each row.
  cmd.callback = row_callback;
  cmd.host = "localhost";
  
  // Make the query.
  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS)
    end(instance, "unable to query", err);
  
  // Reset the query structure for re-use in subsequent queries.
  lcb_n1p_free(n1ql_params);
  
  // Block till the queries finish.
  lcb_wait(instance);
  lcb_destroy(instance);
  
  return 0;
}

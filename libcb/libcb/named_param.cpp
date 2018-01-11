//
//  named_param.cpp
//  libcb
//
//  Created by Gautham Banasandra on 11/01/18.
//  Copyright © 2018 Couchbase. All rights reserved.
//

#include <iostream>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>

static void end(lcb_t, const char *, lcb_error_t);

static void row_callback(lcb_t, int, const lcb_RESPN1QL *);

static void end(lcb_t instance, const char *msg, lcb_error_t err) {
  fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err,
          lcb_strerror(instance, err));
  exit(EXIT_FAILURE);
}

static void row_callback(lcb_t instance, int callback_type,
                         const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    printf("%.*s\n", (int)resp->nrow, resp->row);
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
  options.v.v3.connstr = "http://127.0.0.1:9000/"
  "default?username=eventing&select_bucket=true&"
  "detailed_errcodes=1";
  options.v.v3.type = LCB_TYPE_BUCKET;
  options.v.v3.passwd = "asdasd";
  
  // Initialize the handle.
  err = lcb_create(&instance, &options);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to create handle", err);
  }
  
  // Initialize the parameters for connection.
  err = lcb_connect(instance);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to connect to server", err);
  }
  
  // Block till the connection is established.
  lcb_wait(instance);
  
  // Check if the connection was successful.
  err = lcb_get_bootstrap_status(instance);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to get bootstrap status", err);
  }
  
  // Structure for writing the query.
  lcb_CMDN1QL cmd = {0};
  lcb_N1QLPARAMS *params = lcb_n1p_new();
  err = lcb_n1p_setstmtz(params, "SELECT * FROM `beer-sample` LIMIT $lim;");
  if (err != LCB_SUCCESS) {
    end(instance, "unable to build query string", err);
  }
  
  err = lcb_n1p_namedparamz(params, "$lim", "1");
  if (err != LCB_SUCCESS) {
    end(instance, "unable to set named param", err);
  }
  
  // Build the query.
  lcb_n1p_mkcmd(params, &cmd);
  
  // Set the callback to be invoked for fetching each row.
  cmd.callback = row_callback;
  cmd.host = "localhost";
  
  // Make the query.
  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to query", err);
  }
  
  // Reset the query structure for re-use in subsequent queries.
  lcb_n1p_free(params);
  
  // Block till the queries finish.
  lcb_wait(instance);
  lcb_destroy(instance);
  
  return 0;
}

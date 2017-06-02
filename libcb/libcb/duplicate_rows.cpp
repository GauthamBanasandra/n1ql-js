//
//  duplicate_rows.cpp
//  libcb
//
//  Created by Gautham Banasandra on 30/05/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "json.hpp"
#include "libcouchbase/couchbase.h"
#include "libcouchbase/n1ql.h"
#include <iostream>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

void end(lcb_t, const char *, lcb_error_t);
void row_callback1(lcb_t, int, const lcb_RESPN1QL *);
void row_callback2(lcb_t, int, const lcb_RESPN1QL *);
void query(string);

bool first = true;
lcb_t instance;
lcb_create_st options;
lcb_error_t err;

int main(int argc, const char *argv[]) {
  // LCB related setup.
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

  err = lcb_get_bootstrap_status(instance);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to get bootstrap status", err);
  }

  // First query.
  query("SELECT * FROM `beer-sample` LIMIT 25;");
  lcb_wait(instance);
  cout << endl;

  // A flag to indicate that first query has finished.
  first = false;

  // Second query.
  query("SELECT * FROM `beer-sample` LIMIT 10;");
  lcb_wait(instance);

  lcb_destroy(instance);

  return 0;
}

void query(string query) {
  lcb_CMDN1QL cmd = {0};
  lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
  err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
  if (err != LCB_SUCCESS) {
    end(instance, "unable to build query string", err);
  }

  lcb_n1p_mkcmd(n1ql_params, &cmd);

  if (first) {
    cmd.callback = row_callback1;
  } else {
    cmd.callback = row_callback2;
  }

  err = lcb_n1ql_query(instance, NULL, &cmd);
  if (err != LCB_SUCCESS) {
    end(instance, "unable to query", err);
  }

  lcb_n1p_free(n1ql_params);
}

void end(lcb_t instance, const char *msg, lcb_error_t err) {
  fprintf(stdout, "error\t%s\nerror code\t%X\t%s\n", msg, err,
          lcb_strerror(instance, err));
  exit(EXIT_FAILURE);
}

int row_count = 0;
void row_callback1(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
  ++row_count;
  // In the case of first query, we want to break-out after printing the first
  // row.
  if (first && (row_count > 1)) {
    lcb_breakout(instance);
    return;
  }

  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *row_str;
    asprintf(&row_str, "%.*s", (int)resp->nrow, resp->row);
    auto doc = json::parse(row_str);

    // Prints the prefix 'first' or 'second' accordingly, followed by the row
    // that is received.
    cout << "first query:\t" << doc["beer-sample"]["name"] << endl;
    free(row_str);
  } else {
    printf("%s query:\tmetadata\t %.*s\n", (first ? "first" : "second"),
           (int)resp->nrow, resp->row);
  }
}

void row_callback2(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
  if (!(resp->rflags & LCB_RESP_F_FINAL)) {
    char *row_str;
    asprintf(&row_str, "%.*s", (int)resp->nrow, resp->row);
    auto doc = json::parse(row_str);
    
    // Prints the prefix 'first' or 'second' accordingly, followed by the row
    // that is received.
    cout << (first ? "first" : "second") << " query:\t"
    << doc["beer-sample"]["name"] << endl;
    free(row_str);
  } else {
    printf("%s query:\tmetadata\t %.*s\n", (first ? "first" : "second"),
           (int)resp->nrow, resp->row);
  }
}

//
//  main.cpp
//  libcb
//
//  Created by Gautham Banasandra on 09/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "libcouchbase/couchbase.h"
#include "libcouchbase/n1ql.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void end(lcb_t, const char *, lcb_error_t);
void row_callback(lcb_t, int, const lcb_RESPN1QL *);
void query(string);

int row_count = 0;
lcb_t instance = NULL;
lcb_create_st options;
lcb_error_t err;
lcb_CMDN1QL cmd;

int main(int argc, const char *argv[]) {
    memset(&options, 0, sizeof(options));
    options.version = 3;
    options.v.v3.connstr = "couchbase://localhost";
    
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
    
    cmd = {0};
    
    query("SELECT * FROM `beer-sample` LIMIT 5;");
    
    lcb_destroy(instance);
    
    return 0;
}

void query(string query)
{
    lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
    err = lcb_n1p_setstmtz(n1ql_params, query.c_str());
    if (err != LCB_SUCCESS)
        end(instance, "unable to build query string", err);
    
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
}

void end(lcb_t instance, const char *msg, lcb_error_t err) {
    fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err,
            lcb_strerror(instance, err));
    exit(EXIT_FAILURE);
}

void row_callback(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp) {
    if (!(resp->rflags & LCB_RESP_F_FINAL)) {
        ++row_count;
        printf("count=%d\trow\t %.*s\n", row_count, (int)resp->nrow, resp->row);
    } else
        printf("metadata\t %.*s\n", (int)resp->nrow, resp->row);
}

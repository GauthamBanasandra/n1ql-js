//
// Created by Gautham Banasandra on 03/02/17.
//

#include <iostream>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>

static void end(lcb_t, const char *, lcb_error_t);

static void row_callback(lcb_t, int, const lcb_RESPN1QL *);

using namespace std;

int row_count = 0;

static void end(lcb_t instance, const char *msg, lcb_error_t err)
{
    fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err, lcb_strerror(instance, err));
    exit(EXIT_FAILURE);
}

static void row_callback(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp)
{
    if (!(resp->rflags & LCB_RESP_F_FINAL))
    {
        ++row_count;
        printf("count=%d\trow\t %.*s\n", row_count, (int) resp->nrow, resp->row);
    } else
        printf("metadata\t %.*s\n", (int) resp->nrow, resp->row);
}

int main()
{
    lcb_t instance = NULL;
    lcb_create_st options;
    lcb_error_t err;

    memset(&options, 0, sizeof(options));
    options.version = 3;
    options.v.v3.connstr = "couchbase://localhost";

    err = lcb_create(&instance, &options);
    if (err != LCB_SUCCESS)
        end(instance, "unable to create handle", err);

    err = lcb_connect(instance);
    if (err != LCB_SUCCESS)
        end(instance, "unable to connect to server", err);

    lcb_wait(instance);

    err = lcb_get_bootstrap_status(instance);
    if (err != LCB_SUCCESS)
        end(instance, "unable to get bootstrap status", err);

    lcb_CMDN1QL cmd = {0};
    lcb_N1QLPARAMS *n1ql_params = lcb_n1p_new();
    err = lcb_n1p_setstmtz(n1ql_params, "SELECT * FROM default;");
    if (err != LCB_SUCCESS)
        end(instance, "unable to build query string", err);

    lcb_n1p_mkcmd(n1ql_params, &cmd);
    cmd.callback = row_callback;
    cmd.host = "localhost";
    err = lcb_n1ql_query(instance, NULL, &cmd);
    if (err != LCB_SUCCESS)
        end(instance, "unable to query", err);

    lcb_n1p_free(n1ql_params);
    lcb_wait(instance);
    lcb_destroy(instance);

    return 0;
}
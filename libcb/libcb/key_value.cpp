//
// Created by Gautham Banasandra on 03/02/17.
//

#include <iostream>
#include <libcouchbase/couchbase.h>

static void end(lcb_t, const char *, lcb_error_t);

static void callback(lcb_t, int, const lcb_RESPBASE *);

using namespace std;

static void end(lcb_t instance, const char *msg, lcb_error_t err)
{
    fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err, lcb_strerror(instance, err));
    exit(EXIT_FAILURE);
}

static void callback(lcb_t instance, int callback_type, const lcb_RESPBASE *rb)
{
    cout << "-" << lcb_strcbtype(callback_type) << "-" << endl;

    if (rb->rc == LCB_SUCCESS)
    {
        printf("key %.*s\n", (int) rb->nkey, rb->key);

        if (callback_type == LCB_CALLBACK_GET)
        {
            const lcb_RESPGET *rg = (const lcb_RESPGET *) rb;
            printf("val %.*s\n", (int) rg->nvalue, rg->value);
        }
    } else
        end(instance, lcb_strcbtype(rb->rc), rb->rc);

    (void) instance;
}

int main()
{
    // Couchbase handle instance. Connects to a bucket.
    lcb_t instance = NULL;
    lcb_create_st options;
    lcb_error_t err;

    // Allocate memory for the handle.
    memset(&options, 0, sizeof(options));
    options.version = 3;
    options.v.v3.connstr = "couchbase://localhost";

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

    // Callback to be executed after a get operation.
    lcb_install_callback3(instance, LCB_CALLBACK_GET, callback);

    // Callback to be executed after a store operation.
    lcb_install_callback3(instance, LCB_CALLBACK_STORE, callback);

    // Structure for store operation.
    lcb_CMDSTORE set_cmd = {0};
    LCB_CMD_SET_KEY(&set_cmd, "key", strlen("key"));
    LCB_CMD_SET_VALUE(&set_cmd, "true", strlen("true"));
    set_cmd.operation = LCB_SET;

    // Store the key - value pair.
    err = lcb_store3(instance, NULL, &set_cmd);
    if (err != LCB_SUCCESS)
        end(instance, "unable to schedule storage operation", err);

    cout << "waiting for storage to complete\n" << endl;

    // Block till the store operation is completed.
    lcb_wait(instance);

    // Structure for get operation.
    lcb_CMDGET get_cmd = {0};
    LCB_CMD_SET_KEY(&get_cmd, "key", strlen("key"));

    // Get the key.
    err = lcb_get3(instance, NULL, &get_cmd);
    if (err != LCB_SUCCESS)
        end(instance, "unable to schedule get operation", err);

    cout << "waiting for get to complete\n" << endl;

    // Block till the get operation completes.
    lcb_wait(instance);

    lcb_destroy(instance);

    return 0;
}
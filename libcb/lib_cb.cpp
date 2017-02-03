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

    lcb_install_callback3(instance, LCB_CALLBACK_GET, callback);
    lcb_install_callback3(instance, LCB_CALLBACK_STORE, callback);

    lcb_CMDSTORE set_cmd = {0};
    LCB_CMD_SET_KEY(&set_cmd, "key", strlen("key"));
    LCB_CMD_SET_VALUE(&set_cmd, "true", strlen("true"));
    set_cmd.operation = LCB_SET;

    err = lcb_store3(instance, NULL, &set_cmd);
    if (err != LCB_SUCCESS)
        end(instance, "unable to schedule storage operation", err);

    cout << "waiting for storage to complete\n" << endl;
    lcb_wait(instance);

    lcb_CMDGET get_cmd = {0};
    LCB_CMD_SET_KEY(&get_cmd, "key", strlen("key"));
    err = lcb_get3(instance, NULL, &get_cmd);
    if (err != LCB_SUCCESS)
        end(instance, "unable to schedule get operation", err);

    cout << "waiting for get to complete\n" << endl;
    lcb_wait(instance);

    lcb_destroy(instance);

    return 0;
}
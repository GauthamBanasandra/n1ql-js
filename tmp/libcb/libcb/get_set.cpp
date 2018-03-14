//
//  get_set.cpp
//  libcb
//
//  Created by Gautham Banasandra on 12/06/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <iostream>
#include <libcouchbase/api3.h>
#include <libcouchbase/couchbase.h>

void end(lcb_t, const char *, lcb_error_t);
void get_callback(lcb_t, int, const lcb_RESPBASE *);
void store_callback(lcb_t, int, const lcb_RESPBASE *);

int main() {
  lcb_error_t err;
  lcb_t instance = NULL;
  struct lcb_create_st options = {0};
  lcb_CMDSTORE scmd = {0};
  lcb_CMDGET gcmd = {0};

  options.version = 3;
  options.v.v3.connstr = "couchbase://127.0.0.1:12000/"
                         "default?username=eventing&select_bucket=true&"
                         "detailed_errcodes=1";
  options.v.v3.type = LCB_TYPE_BUCKET;
  options.v.v3.passwd = "asdasd";

  err = lcb_create(&instance, &options);
  if (err != LCB_SUCCESS) {
    end(instance, "Unable to create Couchbase handle", err);
  }

  err = lcb_connect(instance);
  if (err != LCB_SUCCESS) {
    end(instance, "Unable to connect", err);
  }

  lcb_wait(instance);

  err = lcb_get_bootstrap_status(instance);
  if (err != LCB_SUCCESS) {
    end(instance, "Unable to get bootstrap status", err);
  }

  lcb_install_callback3(instance, LCB_CALLBACK_STORE, store_callback);
  lcb_install_callback3(instance, LCB_CALLBACK_GET, get_callback);

  std::string key = "apple";
  LCB_CMD_SET_KEY(&scmd, key.c_str(), key.length());
  std::string value = "fruit";
  LCB_CMD_SET_VALUE(&scmd, value.c_str(), value.length());
  scmd.operation = LCB_SET;

  err = lcb_store3(instance, NULL, &scmd);
  if (err != LCB_SUCCESS) {
    end(instance, "Unable to store", err);
  }

  lcb_wait(instance);

  LCB_CMD_SET_KEY(&gcmd, key.c_str(), key.length());

  err = lcb_get3(instance, NULL, &gcmd);
  if (err != LCB_SUCCESS) {
    end(instance, "Unable to get", err);
  }

  lcb_wait(instance);
  lcb_destroy(instance);

  return 0;
}

void end(lcb_t instance, const char *msg, lcb_error_t err) {
  std::cout << "callback type: " << msg
            << " error: " << lcb_strerror(instance, err);
  exit(1);
}

void get_callback(lcb_t instance, int cbtype, const lcb_RESPBASE *rb) {
  std::cout << "--" << lcb_strcbtype(cbtype) << "--" << std::endl;
  if (rb->rc == LCB_SUCCESS) {
    char *key;
    asprintf(&key, "%.*s", (int)rb->nkey, (const char *)rb->key);

    const lcb_RESPGET *rget = (const lcb_RESPGET *)rb;
    char *value;
    asprintf(&value, "%.*s", (int)rget->nvalue, rget->value);

    std::cout << "key: " << key << std::endl;
    std::cout << "value: " << value << std::endl;
    
    free(value);
    free(key);
  } else {
    end(instance, lcb_strcbtype(rb->rc), rb->rc);
  }
}

void store_callback(lcb_t instance, int cbtype, const lcb_RESPBASE *rb) {
  std::cout << "--" << lcb_strcbtype(cbtype) << "--" << std::endl;
  if (rb->rc == LCB_SUCCESS) {
    char *key;
    asprintf(&key, "%.*s", (int)rb->nkey, (const char *)rb->key);

    std::cout << "key " << key << " stored" << std::endl;
    free(key);
  } else {
    end(instance, lcb_strcbtype(rb->rc), rb->rc);
  }
}

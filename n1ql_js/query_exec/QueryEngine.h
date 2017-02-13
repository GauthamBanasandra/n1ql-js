//
// Created by Gautham Banasandra on 13/02/17.
//

#ifndef N1QL_JS_QUERYENGINE_H
#define N1QL_JS_QUERYENGINE_H

#include <iostream>
#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>

class QueryEngine
{
private:
    lcb_t instance;
    lcb_create_st options;
    lcb_error_t err;
    static void row_callback(lcb_t, int, const lcb_RESPN1QL *);
    static void end(lcb_t, const char *, lcb_error_t);

public:
    QueryEngine();
    void ExecQuery(std::string);
    virtual ~QueryEngine();
};


#endif //N1QL_JS_QUERYENGINE_H

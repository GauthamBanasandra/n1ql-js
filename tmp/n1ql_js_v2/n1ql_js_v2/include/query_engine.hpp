//
//  query_engine.h
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 15/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef query_engine_h
#define query_engine_h

#include <iostream>
#include <vector>
#include "libcouchbase/couchbase.h"
#include "libcouchbase/n1ql.h"
#include "include/v8.h"

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
    
    std::vector<std::string> ExecQuery(std::string);
    
    void ExecQuery(std::string, v8::Local<v8::Function>);
    
    virtual ~QueryEngine();
};


#endif /* query_engine_h */

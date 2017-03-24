//
//  query_engine.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 15/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "query_engine.hpp"

using namespace std;
using namespace v8;

vector<string> rows;
Local<Function> callback;
Local<Value> callback_result;
bool is_callback_set = false;
bool stop_signal = false;

QueryEngine::QueryEngine()
{
    // Couchbase handle instance. Connects to a bucket.
    instance = NULL;
    
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
}

QueryEngine::~QueryEngine()
{
    cout << "calling destructor" << endl;
    lcb_destroy(instance);
}

// Error handler for LCB instance.
void QueryEngine::end(lcb_t instance, const char *msg, lcb_error_t err)
{
    fprintf(stderr, "error\t%s\nerror code\t%X\t%s\n", msg, err, lcb_strerror(instance, err));
    exit(EXIT_FAILURE);
}

void QueryEngine::ExecQuery(std::string query, v8::Local<v8::Function> function)
{
    is_callback_set = true;
    callback = function;
    ExecQuery(query);
}

// TODO: Pull out 'rows.clear' and 'return rows;' and turn that to a separate function.
vector<string> QueryEngine::ExecQuery(std::string query)
{
    rows.clear();
    
    // Structure for writing the query.
    lcb_CMDN1QL cmd = {0};
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
    
    return rows;
}

// Callback to execute for each row.
void QueryEngine::row_callback(lcb_t instance, int callback_type, const lcb_RESPN1QL *resp)
{
    if (stop_signal)
    {
        lcb_breakout(instance);
        return;
    }
    
    if (!(resp->rflags & LCB_RESP_F_FINAL))
    {
        char *temp;
        asprintf(&temp, "%.*s\n", (int) resp->nrow, resp->row);
        
        if (is_callback_set)
        {
            v8::Isolate *isolate = Isolate::GetCurrent();
            v8::Local<v8::Object> json = isolate->GetCurrentContext()
            ->Global()
            ->Get(String::NewFromUtf8(isolate, "JSON"))
            ->ToObject();

            v8::Local<v8::Function> parse = json
            ->Get(v8::String::NewFromUtf8(isolate, "parse")).As<v8::Function>();
            
            v8::Local<v8::Value> args[1];
            args[0] = String::NewFromUtf8(isolate, temp);
            args[0] = parse->Call(json, 1, &args[0]);
            
            callback_result = callback->Call(callback, 1, args);
        } else
            rows.push_back(string(temp));
        
        
        free(temp);
    } else
        printf("metadata\t %.*s\n", (int) resp->nrow, resp->row);
}

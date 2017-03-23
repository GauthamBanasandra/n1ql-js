//
//  exec_js.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "v8_env.hpp"
#include "utils.hpp"
#include "query_engine.hpp"

extern bool stop_signal;

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);







static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];
        
        while((i++ < 3))
            ret += '=';
        
    }
    
    return ret;
    
}
std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    
    return ret;
}













std::string V8Env::ExecJs(std::string js_source) {
    std::string utf8Result;
    
    v8::Isolate *isolate=getIsolate();
    {
        v8::Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);
        
        v8::Local<v8::ObjectTemplate> global_functions = v8::ObjectTemplate::New(isolate);
        
        auto name_log = v8::String::NewFromUtf8(isolate, "log");
        auto callback_log = v8::FunctionTemplate::New(isolate, V8Env::LogFunction);
        global_functions->Set(name_log, callback_log);
        
        auto name_mail = v8::String::NewFromUtf8(isolate, "sendMail");
        auto callback_mail = v8::FunctionTemplate::New(isolate, V8Env::SendMailFunction);
        global_functions->Set(name_mail, callback_mail);
        
        // Exposing the N1qlQuery() constructor to Js.
        auto name_constructor = v8::String::NewFromUtf8(isolate, "N1qlQuery");
        auto callback_constructor = v8::FunctionTemplate::New(isolate, V8Env::N1qlQueryConstructor);
        global_functions->Set(name_constructor, callback_constructor);
        
        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global_functions);
        
        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);
        
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, js_source.c_str());
        
        // Compile the source code.
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        
        // Run the script to get the result.
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
        
        // Convert the result to an UTF8 string and print it.
        v8::String::Utf8Value utf8(result);
        
        utf8Result = *utf8;
    }
    
    return utf8Result;
}



void V8Env::StopIterFunction(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    stop_signal = true;
}

// This function accepts a function as argument and executes it as part of the parent script.
void V8Env::IterFunction(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    
    // Get the "query" attribute of the instance.
    v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
    v8::Local<v8::Value> query_value = args.This()->Get(query_name);
    v8::String::Utf8Value query_string(query_value);
    
    v8::Local<v8::Function> ffunc = v8::Local<v8::Function>::Cast(args[0]);
    
    QueryEngine qEngine;
    qEngine.ExecQuery(*query_string, ffunc);
}


// This function just prints the arguments to the standard output.
void V8Env::LogFunction(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    
    if (args[0]->IsString())
        std::cout << N1qlUtils::GetArgAsString(args) << std::endl;
    else
    {
        v8::Local<v8::Object> json = isolate->GetCurrentContext()
        ->Global()
        ->Get(String::NewFromUtf8(isolate, "JSON"))
        ->ToObject();
        
        v8::Local<v8::Function> stringify = json
        ->Get(v8::String::NewFromUtf8(isolate, "stringify")).As<v8::Function>();
        
        v8::Local<v8::Value> param = args[0];
        v8::Local<v8::Value> result = stringify->Call(json, 1, &param);
        
        v8::String::Utf8Value const str_result(result);
        
        
        std::cout << *str_result << std::endl;
    }
}

void V8Env::SendMailFunction(const v8::FunctionCallbackInfo<v8::Value> &args) {
    v8::HandleScope scope(args.GetIsolate());
    v8::String::Utf8Value to_address(args[0]);
    v8::String::Utf8Value subject(args[1]);
    v8::String::Utf8Value message(args[2]);
    
    std::string to_address_str = *to_address;
    std::string subject_str = *subject;
    std::string message_str = *message;
    
    std::string encoded_message;
    encoded_message += base64_encode(reinterpret_cast<const unsigned char *>(to_address_str.c_str()), to_address_str.length()) + ";";
    encoded_message += base64_encode(reinterpret_cast<const unsigned char *>(subject_str.c_str()), subject_str.length()) + ";";
    encoded_message += base64_encode(reinterpret_cast<const unsigned char *>(message_str.c_str()), message_str.length());
    
    FILE *in;
    if (!(in = popen(
                     "python /Users/gautham/projects/file_modifier/send_email.py",
                     "w"))) {
        std::cout << "error in spawning python mail sender process" << std::endl;
        return;
    }
    
    fprintf(in, "%s", encoded_message.c_str());
    fflush(in);
    
    pclose(in);
}

// Accepts the N1QL query and executes it.
void V8Env::ExecQueryFunction(const v8::FunctionCallbackInfo<Value> &args)
{
    v8::Isolate *isolate =v8::Isolate::GetCurrent();
    v8::HandleScope handleScope(isolate);
    
    // Get the "query" attribute of the instance.
    v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
    v8::Local<v8::Value> query_value = args.This()->Get(query_name);
    v8::String::Utf8Value query_string(query_value);
    
    // Execute the query.
    QueryEngine qEngine;
    std::vector<std::string> rows = qEngine.ExecQuery(*query_string);
    
    // Array to store the result.
    v8::Local<v8::Array> result_array = v8::Array::New(isolate, static_cast<int>(rows.size()));
    
    // Populate the array with the rows of the result.
    for (int i = 0; i < rows.size(); ++i)
    {
        v8::Local<v8::Value> json_row = v8::JSON::Parse(v8::String::NewFromUtf8(isolate, rows[i].c_str()));
        result_array->Set(static_cast<uint32_t>(i), json_row);
    }
    
    // Return the array filled with rows of the result.
    args.GetReturnValue().Set(result_array);
}

// Constructor for N1qlQuery().
// Accepts a string as N1QL query and returns an instance of N1qlQuery.
void V8Env::N1qlQueryConstructor(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handleScope(isolate);
    
    v8::Local<v8::ObjectTemplate> obj = v8::ObjectTemplate::New();
    
    // Attribute name - query.
    v8::Local<v8::Name> query_name = v8::String::NewFromUtf8(isolate, "query");
    v8::Local<v8::Value> empty_string = v8::String::NewFromUtf8(isolate, "");
    
    // Initialize the attribute "name" with empty string.
    obj->Set(query_name, empty_string);
    
    v8::Local<v8::Name> is_inst_name = v8::String::NewFromUtf8(isolate, "isInstance");
    v8::Local<v8::Value> is_inst_value=v8::Boolean::New(isolate, true);
    obj->Set(is_inst_name, is_inst_value);
    
    // Method name - execQuery.
    v8::Local<v8::String> exec_query_name = v8::String::NewFromUtf8(isolate, "execQuery");
    obj->Set(exec_query_name, v8::FunctionTemplate::New(isolate, V8Env::ExecQueryFunction));
    
    // Method name - iter.
    v8::Local<v8::String> iter_name = v8::String::NewFromUtf8(isolate, "iter");
    obj->Set(iter_name, v8::FunctionTemplate::New(isolate, V8Env::IterFunction));
    
    // Method name - iter.
    v8::Local<v8::String> stop_iter_name = String::NewFromUtf8(isolate, "stopIter");
    obj->Set(stop_iter_name, v8::FunctionTemplate::New(isolate, V8Env::StopIterFunction));
    
    // If the arguments to the constructor is non-empty, initialize the instance with query string.
    if (!args[0].IsEmpty() && args[0]->IsString())
        obj->Set(query_name, args[0]);
    
    // Return the new instance.
    args.GetReturnValue().Set(obj->NewInstance());
}

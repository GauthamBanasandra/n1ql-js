//
//  comm.hpp
//  transpiler
//
//  Created by Gautham Banasandra on 29/12/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef comm_hpp
#define comm_hpp

#include <curl/curl.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <v8.h>

struct CURLResponse {
  bool is_error;
  std::string response;
  std::unordered_map<std::string, std::string> headers;
};

struct ParseInfo {
  bool is_valid;
  std::string info;
};

struct NamedParamsInfo {
  ParseInfo p_info;
  std::vector<std::string> named_params;
};

class CURLClient {
private:
  CURL *curl_handle;
  CURLcode code;
  struct curl_slist *headers;

  static size_t BodyCallback(void *buffer, size_t size, size_t nmemb,
                             void *cookie);
  static size_t HeaderCallback(char *buffer, size_t size, size_t nitems,
                               void *cookie);

public:
  CURLClient();
  ~CURLClient();

  CURLResponse HTTPPost(const std::vector<std::string> &headers,
                        const std::string &url, const std::string &body);
};

class Communicator {
public:
  Communicator(const std::string &host_port, v8::Isolate *isolate);

  ParseInfo ParseQuery(const std::string &query);
  NamedParamsInfo GetNamedParams(const std::string &query);

private:
  std::string parse_query_url;
  std::string get_named_params_url;
  v8::Isolate *isolate;
  
  ParseInfo ExtractParseInfo(v8::Local<v8::Object> &parse_info_v8val);
};

#endif /* comm_hpp */

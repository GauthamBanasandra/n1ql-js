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
#include <v8.h>
#include <vector>

struct CURLResponse {
  bool is_error;
  std::string response;
  std::unordered_map<std::string, std::string> headers;
};

struct DecodeKVInfo {
  bool is_valid;
  std::string msg;
  std::string key;
  std::string value;
};

struct CredsInfo {
  bool is_valid;
  std::string msg;
  std::string username;
  std::string password;
};

struct ExtractKVInfo {
  bool is_valid;
  std::string msg;
  std::unordered_map<std::string, std::string> kv;
};

// Info about parsing N1QL query
struct ParseInfo {
  bool is_valid;
  std::string info;
};

struct NamedParamsInfo {
  ParseInfo p_info;
  std::vector<std::string> named_params;
};

class CURLClient {
public:
  CURLClient();
  ~CURLClient();

  CURLResponse HTTPPost(const std::vector<std::string> &headers,
                        const std::string &url, const std::string &body);
  ExtractKVInfo ExtractKV(const std::string &encoded_str);
  
private:
  static size_t BodyCallback(void *buffer, size_t size, size_t nmemb,
                             void *cookie);
  static size_t HeaderCallback(char *buffer, size_t size, size_t nitems,
                               void *cookie);
  std::string Decode(const std::string &encoded_str);

  CURL *curl_handle;
  CURLcode code;
  struct curl_slist *headers;
};

// Channel to communicate to eventing-producer through CURL
class Communicator {
public:
  Communicator(const std::string &host_ip, const std::string &host_port);
  
  ParseInfo ParseQuery(const std::string &query);
  CredsInfo GetCreds(const std::string &endpoint);
  NamedParamsInfo GetNamedParams(const std::string &query);
  
private:
  NamedParamsInfo ExtractNamedParams(const std::string &encoded_str);
  CredsInfo ExtractCredentials(const std::string &encoded_str);
  ParseInfo ExtractParseInfo(const std::string &encoded_str);
  
  CURLClient curl;
  std::string parse_query_url;
  std::string get_creds_url;
  std::string get_named_params_url;
};

#endif /* comm_hpp */

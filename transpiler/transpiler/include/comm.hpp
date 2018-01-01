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

struct CURLResponse {
  bool is_error;
  std::string response;
  std::unordered_map<std::string, std::string> headers;
};

struct ParseInfo {
  bool is_valid;
  std::string info;
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
private:
  std::string parse_query_url;

public:
  Communicator(int host_port);

  ParseInfo ParseQuery(const std::string &query);
};

#endif /* comm_hpp */

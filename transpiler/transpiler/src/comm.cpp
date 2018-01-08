//
//  comm.cpp
//  transpiler
//
//  Created by Gautham Banasandra on 29/12/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "comm.hpp"
#include "utils.hpp"
#include <iostream>

CURLClient::CURLClient() : headers(nullptr) { curl_handle = curl_easy_init(); }

CURLClient::~CURLClient() { curl_easy_cleanup(curl_handle); }

size_t CURLClient::BodyCallback(void *buffer, size_t size, size_t nmemb,
                                void *cookie) {
  auto realsize = size * nmemb;
  auto data = static_cast<std::string *>(cookie);
  auto content = static_cast<char *>(buffer);
  data->assign(&content[0], &content[0] + realsize);
  return realsize;
}

size_t CURLClient::HeaderCallback(char *buffer, size_t size, size_t nitems,
                                  void *cookie) {
  auto realsize = size * nitems;
  auto headers =
      static_cast<std::unordered_map<std::string, std::string> *>(cookie);
  auto header = std::string(static_cast<char *>(buffer));

  // Split the header into key:value
  auto find = header.find(':');
  if (find != std::string::npos) {
    (*headers)[header.substr(0, find)] = header.substr(find + 1); // Adding 1 to discount the ':'
  }

  return realsize;
}

CURLResponse CURLClient::HTTPPost(const std::vector<std::string> &header_list,
                                  const std::string &url,
                                  const std::string &body) {
  CURLResponse response;

  code = curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response =
        "Unable to set URL: " + std::string(curl_easy_strerror(code));
    return response;
  }

  for (const auto &header : header_list) {
    headers = curl_slist_append(headers, header.c_str());
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response = "Unable to do set HTTP header(s): " +
                        std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.c_str());
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response =
        "Unable to set POST body: " + std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                          CURLClient::BodyCallback);
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response = "Unable to set body callback function: " +
                        std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION,
                          CURLClient::HeaderCallback);
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response = "Unable to set header callback function: " +
                        std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA,
                          (void *)&response.headers);
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response = "Unable to set cookie for headers: " +
                        std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,
                          (void *)&response.response);
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response = "Unable to set cookie for body: " +
                        std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response =
        "Unable to set user agent: " + std::string(curl_easy_strerror(code));
    return response;
  }

  code = curl_easy_perform(curl_handle);
  if (code != CURLE_OK) {
    response.is_error = true;
    response.response =
        "Unable to do HTTP POST: " + std::string(curl_easy_strerror(code));
    return response;
  }

  response.is_error = false;
  return response;
}

Communicator::Communicator(const std::string &host_port) {
  parse_query_url = "http://localhost:" + host_port + "/parseQuery";
  isolate = v8::Isolate::GetCurrent();
}

ParseInfo Communicator::ParseQuery(const std::string &query) {
  v8::HandleScope handle_scope(isolate);
  
  CURLClient curl;
  auto response =
      curl.HTTPPost({"Content-Type: text/plain"}, parse_query_url, query);

  ParseInfo info;
  info.is_valid = false;
  info.info = "Something went wrong while parsing the N1QL query";
  
  if (response.is_error) {
    // Something went wrong with CURL lib
    std::cout << response.response << std::endl;
    return info;
  }
  
  if (response.headers.find("Status")==response.headers.end()) {
    info.info = response.response;
    return info;
  }
  
  if (std::stoi(response.headers["Status"]) != 0) {
    // Something went wrong with N1QL parser
    return info;
  }

  auto resp_obj = v8::JSON::Parse(v8Str(isolate, response.response)).As<v8::Object>();
  if (resp_obj.IsEmpty()) {
    // Something went wrong while parsing JSON
    return info;
  }
  
  auto is_valid_v8val = resp_obj->Get(v8Str(isolate, "is_valid"));
  auto info_v8val = resp_obj->Get(v8Str(isolate, "info"));
  
  info.is_valid = is_valid_v8val->ToBoolean()->Value();
  v8::String::Utf8Value info_str(info_v8val);
  info.info = *info_str;
  
  return info;
}

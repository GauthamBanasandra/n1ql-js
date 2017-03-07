//
//  utils.hpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include "include/v8.h"
#include <iostream>

class N1qlUtils {
  public:
    static std::string GetStartupData();

    static std::string GetQueryBuilderPath();

    static std::string GetJsFormatPath();
    
    static std::string ReadFile(std::string file_path);

    static std::string
    GetArgAsString(const v8::FunctionCallbackInfo<v8::Value> &);
};
#endif /* utils_hpp */

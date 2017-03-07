//
//  v8_env.hpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef v8_env_hpp
#define v8_env_hpp

#include "../include/array_buf.h"
#include "include/libplatform/libplatform.h"
#include "include/v8.h"

class V8Env {
  private:
    Isolate *isolate;
    Platform *platform;
    
  public:
    V8Env();

    std::string Build(std::string, std::string);

    Isolate *getIsolate() const;

    virtual ~V8Env();
};

#endif /* v8_env_hpp */

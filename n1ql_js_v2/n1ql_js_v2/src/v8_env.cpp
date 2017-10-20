//
//  v8_env.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "utils.hpp"
#include "v8_env.hpp"

using namespace std;
using namespace v8;

// Initializes the v8 engine.
V8Env::V8Env() {
  V8::InitializeICU();
  V8::InitializeExternalStartupData(N1qlUtils::GetStartupData().c_str());
  Platform *platform = platform::CreateDefaultPlatform();
  V8::InitializePlatform(platform);
  V8::Initialize();
  
  // Create a new Isolate and make it the current one.
  ArrayBufferAllocator allocator;
  Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = &allocator;
  isolate = Isolate::New(create_params);
}

// Purges the existing v8 engine.
V8Env::~V8Env() {
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  // TODO : The following line causes EXC_BAD_ACCESS in xcode.
  //    delete platform;
}

Isolate *V8Env::getIsolate() const { return isolate; }

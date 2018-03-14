//
//  main.cpp
//  unit-test
//
//  Created by Gautham Banasandra on 20/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <gtest/gtest.h>
#include <libplatform/libplatform.h>
#include <v8.h>

int main(int argc, char* argv[]) {
  int code;
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  v8::Platform* platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
  v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    ::testing::InitGoogleTest(&argc, argv);
    code = RUN_ALL_TESTS();
  }
  
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete platform;
  delete create_params.array_buffer_allocator;
  return code;
}

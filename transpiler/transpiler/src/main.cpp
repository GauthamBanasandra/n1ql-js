// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "include/libplatform/libplatform.h"
#include "include/v8.h"

inline std::string ReadFile(std::string path) {
  std::string line, content;
  std::ifstream file(path);
  if (file.is_open()) {
    while (getline(file, line)) {
      content += line;
    }
    
    file.close();
  } else {
    std::cout << "Unable to open file:\t" << path << std::endl;
  }
  
  return content;
}

inline std::string GetTranspilerSrc(std::string third_party_path,
                                    const char *transpiler_path) {
  std::string transpiler_src;
  transpiler_src += ReadFile(third_party_path + transpiler_path) + "\n" +
  ReadFile(third_party_path + "esprima.js") + "\n" +
  ReadFile(third_party_path + "escodegen.js") + "\n" +
  ReadFile(third_party_path + "estraverse.js") + "\n";
  return transpiler_src;
}

int main(int argc, char *argv[]) {
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  v8::Platform *platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
  v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate *isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);
    
    std::string src = GetTranspilerSrc(argv[0], argv[1]);
    std::cout << src << std::endl;
    
    v8::Local<v8::String> source =
    v8::String::NewFromUtf8(isolate, "'Hello' + ', World!'",
                            v8::NewStringType::kNormal)
    .ToLocalChecked();
    
    v8::Local<v8::Script> script =
    v8::Script::Compile(context, source).ToLocalChecked();
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
    v8::String::Utf8Value utf8(result);
    printf("%s\n", *utf8);
  }
  
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete platform;
  delete create_params.array_buffer_allocator;
  return 0;
}

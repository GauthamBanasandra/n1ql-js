//
//  main.cpp
//  debugger
//
//  Created by Gautham Banasandra on 10/07/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "inspector_agent.h"
#include "libplatform/libplatform.h"
#include "v8.h"
#include "n1ql.hpp"

#define BUILTIN                                                                \
"/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/builtin.js"
#define TRANSPILER                                                             \
"/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/transpiler.js"
#define ESPRIMA                                                                \
"/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/esprima.js"
#define ESCODEGEN                                                              \
"/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/escodegen.js"
#define ESTRAVERSE                                                             \
"/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/estraverse.js"
#define SOURCE_MAP "/Users/gautham/projects/github/n1ql-js/n1ql_js_v3/n1ql_js_v3/source-map.js"

using namespace v8;
using namespace inspector;

N1QL *n1ql_handle;

MaybeLocal<String> ReadFile(Isolate *isolate, const char *name);
bool ExecuteString(Isolate *isolate, Local<String> source, Local<Value> name,
                   bool print_result, bool report_exceptions);

std::string ReadFile(std::string file_path) {
  std::ifstream file(file_path);
  std::string source((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
  return source;
}

void WriteFile(std::string content, std::string path) {
  std::ofstream file;
  file.open(path);
  file << content;
  file.close();
}

// Reads a file into a v8 string.
MaybeLocal<String> ReadFile(Isolate *isolate, const char *name) {
  FILE *file = fopen(name, "rb");
  if (file == NULL)
    return MaybeLocal<String>();

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *chars = new char[size + 1];
  chars[size] = '\0';
  for (size_t i = 0; i < size;) {
    i += fread(&chars[i], 1, size - i, file);
    if (ferror(file)) {
      fclose(file);
      return MaybeLocal<String>();
    }
  }
  fclose(file);
  MaybeLocal<String> result = String::NewFromUtf8(
      isolate, chars, NewStringType::kNormal, static_cast<int>(size));
  delete[] chars;
  return result;
}

// Extracts a C string from a V8 Utf8Value.
const char *ToCString(const v8::String::Utf8Value &value) {
  return *value ? *value : "<std::string conversion failed>";
}

bool ToCBool(const v8::Local<v8::Boolean> &value) {
  if (value.IsEmpty()) {
    std::cout << "Failed to convert to bool" << '\n';
  }
  
  return value->Value();
}


// Executes a string within the current v8 context.
bool ExecuteString(Isolate *isolate, Local<String> source, Local<Value> name,
                   bool print_result, bool report_exceptions) {
  HandleScope handle_scope(isolate);
  TryCatch try_catch(isolate);
  ScriptOrigin origin(name);
  Local<Context> context(isolate->GetCurrentContext());
  Local<Script> script;
  if (!Script::Compile(context, source, &origin).ToLocal(&script)) {
    return false;
  } else {
    Local<Value> result;
    if (!script->Run(context).ToLocal(&result)) {
      assert(try_catch.HasCaught());
      return false;
    } else {
      assert(!try_catch.HasCaught());
      if (print_result && !result->IsUndefined()) {
        // If all went well and the result wasn't undefined then print
        // the returned value.
        String::Utf8Value str(result);
        const char *cstr = ToCString(str);
        printf("%s\n", cstr);
      }
      Handle<Value> exponent = context->Global()->Get(
          String::NewFromUtf8(isolate, "exponent", NewStringType::kNormal)
              .ToLocalChecked());
      Handle<Function> expFun = Handle<Function>::Cast(exponent);
      Handle<Value> arg[2];
      arg[0] = Integer::New(isolate, 10);
      arg[1] = Integer::New(isolate, 2);
      Handle<Value> js_result = expFun->Call(Null(isolate), 2, arg);
      return true;
    }
  }
}

// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
void Print(const FunctionCallbackInfo<Value> &args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    HandleScope handle_scope(args.GetIsolate());
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    String::Utf8Value str(args[i]);
    const char *cstr = ToCString(str);
    printf("%s", cstr);
  }
  printf("\n");
  fflush(stdout);
}

std::string GetDebugCodePath(std::string path) {
  std::size_t i = path.rfind(".js");
  if (i!=std::string::npos) {
    std::string tjs = ".d.js";
    path.replace(i, tjs.length(), tjs);
    
    return path;
  }
  
  return "";
}

std::string GetTranspiledCodePath(std::string path) {
  std::size_t i = path.rfind(".js");
  if (i!=std::string::npos) {
    std::string tjs = ".t.js";
    path.replace(i, tjs.length(), tjs);
    
    return path;
  }
  
  return "";
}

std::string GetSourceMapPath(std::string path) {
  std::size_t i = path.rfind(".js");
  if (i!=std::string::npos) {
    std::string json = ".map.json";
    path.replace(i, json.length(), json);
    
    return path;
  }
  
  return "";
}

void AppendSourceMapPath(std::string t_code_path, std::string sm_path) {
  std::ofstream t_code_file;
  t_code_file.open(t_code_path, std::ios_base::app);
  
  t_code_file << "\n//# sourceMappingURL=file://" << sm_path;
  t_code_file.close();
}

int main(int argc, char *argv[]) {
  // Initialize V8.
  V8::InitializeICUDefaultLocation(argv[0]);
  V8::InitializeExternalStartupData(argv[0]);
  Platform *platform = platform::CreateDefaultPlatform(4);
  V8::InitializePlatform(platform);
  V8::Initialize();

  // Create a new Isolate and make it the current one.
  Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      ArrayBuffer::Allocator::NewDefaultAllocator();
  Isolate *isolate = Isolate::New(create_params);
  {
    Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Enter the context for compiling and running the hello world script.
    Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
    global->Set(String::NewFromUtf8(isolate, "print", NewStringType::kNormal)
                    .ToLocalChecked(),
                FunctionTemplate::New(isolate, Print));
    global->Set(v8::String::NewFromUtf8(isolate, "iter"),
                v8::FunctionTemplate::New(isolate, IterFunction));
    global->Set(v8::String::NewFromUtf8(isolate, "stopIter"),
                v8::FunctionTemplate::New(isolate, StopIterFunction));
    global->Set(v8::String::NewFromUtf8(isolate, "execQuery"),
                v8::FunctionTemplate::New(isolate, ExecQueryFunction));    
    global->Set(v8::String::NewFromUtf8(isolate, "getReturnValue"),
                v8::FunctionTemplate::New(isolate, GetReturnValueFunction));

    Local<Context> context = Context::New(isolate, NULL, global);
    Context::Scope context_scope(context);
    
    std::string src = ReadFile(argv[1]) + '\n';
    src += ReadFile(BUILTIN) + '\n';
    std::string d_code_path = GetDebugCodePath(argv[1]);
    WriteFile(src, d_code_path);
    
    std::string third_party_src = ReadFile(TRANSPILER) + '\n';
    third_party_src += ReadFile(SOURCE_MAP) +'\n';
    third_party_src += ReadFile(ESCODEGEN) + '\n';
    third_party_src += ReadFile(ESTRAVERSE) + '\n';
    third_party_src += ReadFile(ESPRIMA);
    
    ConnectionPool *conn_pool = new ConnectionPool(15, "127.0.0.1:12000", "default", "eventing", "asdasd");
    n1ql_handle = new N1QL(conn_pool);
    
    Transpiler transpiler(third_party_src);
    std::string transpiled_src = transpiler.Transpile(src, "input3.d.js");
    std::string t_code_path = GetTranspiledCodePath(d_code_path);
    WriteFile(transpiled_src, t_code_path);
    
    std::string source_map = transpiler.GetSourceMap(src, "input3.d.js");
    std::string sm_path = GetSourceMapPath(d_code_path);
    WriteFile(source_map, sm_path);
    
    AppendSourceMapPath(t_code_path, sm_path);
        
    Agent *agent = new Agent();
    agent->Start(isolate, platform, t_code_path.c_str());
    agent->PauseOnNextJavascriptStatement("Break on start");

    Local<String> file_name =
        String::NewFromUtf8(isolate, t_code_path.c_str(), NewStringType::kNormal)
            .ToLocalChecked();
    Local<String> source;
    if (!ReadFile(isolate, t_code_path.c_str()).ToLocal(&source)) {
      fprintf(stderr, "Error reading '%s'\n", t_code_path.c_str());
      return 1;
    }
    bool success = ExecuteString(isolate, source, file_name, false, true);
    while (platform::PumpMessageLoop(platform, isolate)) {
    };
    if (!success)
      return 1;
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  delete platform;
  delete create_params.array_buffer_allocator;
  return 0;
}

//
// Created by Gautham Banasandra on 10/02/17.
//

#include "V8Env.h"
#include "../utils/Utils.h"

using namespace std;

// Initializes the v8 engine.
V8Env::V8Env()
{
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
V8Env::~V8Env()
{
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
//    delete platform;
}

Isolate *V8Env::getIsolate() const
{
    return isolate;
}

//
// Created by Gautham Banasandra on 10/02/17.
//

#ifndef N1QL_JS_ARRAYBUFFERALLOCATOR_H
#define N1QL_JS_ARRAYBUFFERALLOCATOR_H

#include <iostream>
#include "/Users/gautham/.cbdepscache/include/libplatform/libplatform.h"
#include "/Users/gautham/.cbdepscache/include/v8.h"

using namespace v8;

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator
{
public:
    virtual void *Allocate(size_t length)
    {
        void *data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void *AllocateUninitialized(size_t length)
    { return malloc(length); }

    virtual void Free(void *data, size_t)
    { free(data); }
};

#endif //N1QL_JS_ARRAYBUFFERALLOCATOR_H

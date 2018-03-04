//
//  array_buf.h
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef array_buf_h
#define array_buf_h

#include "include/libplatform/libplatform.h"
#include "include/v8.h"
#include <iostream>

using namespace v8;

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  public:
    virtual void *Allocate(size_t length) {
        void *data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void *AllocateUninitialized(size_t length) {
        return malloc(length);
    }

    virtual void Free(void *data, size_t) { free(data); }
};

#endif /* array_buf_h */

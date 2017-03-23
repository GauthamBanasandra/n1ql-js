//
//  jsify.hpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 06/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef jsify_hpp
#define jsify_hpp
#include <iostream>

enum op_code {
    OK,
    KWD_ALTER,
    KWD_BUILD,
    KWD_CREATE,
    KWD_DELETE,
    KWD_DROP,
    KWD_EXECUTE,
    KWD_EXPLAIN,
    KWD_GRANT,
    KWD_INFER,
    KWD_INSERT,
    KWD_MERGE,
    KWD_PREPARE,
    KWD_RENAME,
    KWD_REVOKE,
    KWD_SELECT,
    KWD_UPDATE,
    KWD_UPSERT
};
int parse(const char *, std::string *);

#endif /* jsify_hpp */

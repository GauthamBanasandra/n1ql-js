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

int parse(const char *, std::string *);

enum op_code {
    OK,
    KWD_SELECT,
    KWD_CREATE,
    KWD_UPSERT,
    KWD_INSERT,
    KWD_DELETE,
    KWD_UPDATE,
    KWD_MERGE
};

#endif /* jsify_hpp */

//
//  q_builder.hpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef q_builder_hpp
#define q_builder_hpp
#include <iostream>
enum builder_mode{EXEC_JS_FORMAT, EXEC_TRANSPILER};
std::string Build(std::string js_src, std::string user_code);
#endif /* q_builder_hpp */

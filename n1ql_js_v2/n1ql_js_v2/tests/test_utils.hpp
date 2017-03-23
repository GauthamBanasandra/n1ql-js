//
//  test_utils.hpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef test_utils_hpp
#define test_utils_hpp

#include <iostream>
std::string ReadFile(std::string);
std::string FormatJs(std::string input);
std::string Transpile(std::string);
#endif /* test_utils_hpp */

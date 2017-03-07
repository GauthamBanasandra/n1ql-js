//
//  test_utils.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "test_utils.hpp"
#include <fstream>

using namespace std;

string ReadFile(string filename) {
    ifstream file(filename);
    string source((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());
    
    return source;
}

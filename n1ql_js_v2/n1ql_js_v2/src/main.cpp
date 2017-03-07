//
//  main.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 06/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "../include/jsify.hpp"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, const char *argv[]) {
    ifstream file("/Users/gautham/projects/github/n1ql-js/n1ql_js_v2/n1ql_js_v2/inputs/inputs/input_select.txt");
    string source((istreambuf_iterator<char>(file)),
                  istreambuf_iterator<char>());
    const char *input = source.c_str();
    string plain_js = parse(input);
    cout << plain_js << endl;
    
    return 0;
}

//
//  test_utils.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 07/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "test_utils.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

using namespace std;

void MapTestData(string);

map<string, string> test_data;

string ReadFile(string filename) {
    if (test_data.size() == 0) {
        MapTestData(N1qlUtils::GetTestInputPath());
    }
    
//    ifstream file(filename);
//    string source((istreambuf_iterator<char>(file)),
//                  istreambuf_iterator<char>());
    
    return test_data[filename];
}

void MapTestData(string test_data_path){
    ifstream test_data_file(test_data_path);
    string delimiter = "****************************************************************************************************";
    string line;
    string key;
    
    while (getline(test_data_file, key)) {
        string value;
        
        do {
            if (line!="") {
                value+=line+"\n";
            }
            
            getline(test_data_file, line);
        } while (line != delimiter);
        
        test_data[key] = value;
        line="";
    }
}

//
//  test-utils.hpp
//  unit-test
//
//  Created by Gautham Banasandra on 20/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#ifndef test_utils_hpp
#define test_utils_hpp

#include <string>
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
#include <gtest/gtest.h>

std::string GetTestData(std::string filename);
void MapTestData(std::string);
std::string ReadFile(std::string path);
std::string GetTranspilerSrc();

#endif /* test_utils_hpp */

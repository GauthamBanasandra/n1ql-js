//
//  test-utils.cpp
//  unit-test
//
//  Created by Gautham Banasandra on 20/10/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "test-utils.hpp"

#define INP_COMBINE "/Users/gautham/projects/file_modifier/input_combine.py"
#define INPUTS "/Users/gautham/projects/github/n1ql-js/transpiler/unit-test/input/"
#define INP_COMBINED "/Users/gautham/projects/github/n1ql-js/transpiler/unit-test/input/jsify_inputs.txt"
#define DELIMITER "****************************************************************************************************"

#define PROJECT_ROOT "/Users/gautham/projects/github/n1ql-js/transpiler"
#define THIRD_PARTY_PATH PROJECT_ROOT "/transpiler/third_party"
#define TRANSPILER_JS_PATH PROJECT_ROOT "/transpiler/src/transpiler.js"
#define BUILTIN_JS_PATH PROJECT_ROOT "/transpiler/src/builtin.js"

std::unordered_map<std::string, std::string> test_data;
std::string transpiler_js_src;

std::string GetTestData(std::string filename) {
  if (test_data.size() == 0) {
    system("python " INP_COMBINE " " INPUTS " > " INP_COMBINED);
    MapTestData(INP_COMBINED);
  }
  
  //    ifstream file(filename);
  //    string source((istreambuf_iterator<char>(file)),
  //                  istreambuf_iterator<char>());
  
  return test_data[filename];
}

void MapTestData(std::string test_data_path) {
  std::ifstream test_data_file(test_data_path);
  
  std::string line;
  std::string key;
  
  while (std::getline(test_data_file, key)) {
    std::string value;
    do {
      if (line != "") {
        value += line + "\n";
      }
      
      std::getline(test_data_file, line);
    } while (line != DELIMITER);
    
    test_data[key] = value;
    line = "";
  }
}

std::string ReadFile(std::string path) {
  std::string line, content;
  std::ifstream file(path);
  if (file.is_open()) {
    while (getline(file, line)) {
      content += line + "\n";
    }
    
    file.close();
  } else {
    std::cout << "Unable to open file:\t" << path << std::endl;
  }
  
  return content;
}

std::string GetTranspilerSrc() {
  std::string transpiler_src;
  transpiler_src += ReadFile(TRANSPILER_JS_PATH) + "\n" +
  ReadFile(THIRD_PARTY_PATH "/esprima.js") + "\n" +
  ReadFile(THIRD_PARTY_PATH "/escodegen.js") + "\n" +
  ReadFile(THIRD_PARTY_PATH "/estraverse.js") + "\n";
  return transpiler_src;
}

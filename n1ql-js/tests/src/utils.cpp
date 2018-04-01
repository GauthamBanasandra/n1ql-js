//
// Created by gauth on 01-04-2018.
//

#include <utility>
#include <fstream>
#include <iostream>
#include <cassert>

#include <tests/include/utils.h>

#define PROJECT_ROOT R"(C:\Users\gauth\source\repos\n1ql-js\n1ql-js)"
#define THIRD_PARTY_PATH PROJECT_ROOT R"(/third-party)"
#define TRANSPILER_JS_PATH PROJECT_ROOT R"(/src/transpiler.js)"

namespace utils {
    std::string ReadFile(const std::string &path) {
        std::string line, content;
        std::ifstream file(path);
        if (file.is_open()) {
            while (getline(file, line)) {
                content += line + "\n";
            }

            file.close();
        } else {
            std::cout << "Unable to open file:\t" << path << std::endl;
            assert(false);
        }

        return content;
    }

    utils::Transpiler::Transpiler() {
        source += ReadFile(TRANSPILER_JS_PATH) + "\n" +
                  ReadFile(THIRD_PARTY_PATH "/esprima.js") + "\n" +
                  ReadFile(THIRD_PARTY_PATH "/escodegen.js") + "\n" +
                  ReadFile(THIRD_PARTY_PATH "/estraverse.js") + "\n";
    }

    void Transpiler::Transpile(const std::string &input, const std::string &filename) {

    }
}
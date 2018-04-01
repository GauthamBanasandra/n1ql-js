//
// Created by gauth on 01-04-2018.
//

#ifndef N1QL_JS_UTILS_H
#define N1QL_JS_UTILS_H

#include <string>

namespace utils {
    std::string ReadFile(const std::string &path);

    class Transpiler {
    public:
        Transpiler();

        void Transpile(const std::string &input, const std::string &filename);

        const std::string &GetSource() { return source; };

    private:
        std::string source;
    };
}
#endif //N1QL_JS_UTILS_H

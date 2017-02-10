#include <iostream>
#include <fstream>
#include "n1ql_js.h"

using namespace std;

string ReadFile(string file_name)
{
    ifstream file(file_name);
    string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    return source_code;
}

int main(int argc, char *argv[])
{
    string source_code = ReadFile(argv[1]);

    ExecJs(source_code);

    return 0;
}
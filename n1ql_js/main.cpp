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

int main()
{
    string input_file_path = "inputs/input_jsify.txt";
    string source_code = ReadFile(input_file_path);

    exec_js(source_code);

    return 0;
}
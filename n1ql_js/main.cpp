#include <iostream>
#include <fstream>
#include "jsify/jsify.h"

using namespace std;

int main()
{
    ifstream file("inputs/input_jsify.txt");
    string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    string plain_js = parse(source_code.c_str());

    cout << plain_js << endl;
    return 0;
}
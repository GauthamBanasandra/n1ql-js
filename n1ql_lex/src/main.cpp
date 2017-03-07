#include <iostream>
#include <fstream>

std::string parse(const char*);

using namespace std;

int main()
{
	ifstream file("inputs/input_select.txt");
	string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	const char* input=source_code.c_str();
	string js_code=parse(input);

	cout << js_code << endl;

	return 0;
}
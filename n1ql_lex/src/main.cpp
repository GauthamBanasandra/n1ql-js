#include <iostream>
#include <fstream>

void parse(const char*, std::string *);

using namespace std;

int main()
{
	ifstream file("inputs/input_select.txt");
	string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	const char* input=source_code.c_str();
	string plain_js_code;
	parse(input, &plain_js_code);

	cout << plain_js_code << endl<<endl;

	parse(input, &plain_js_code);

	cout << plain_js_code << endl<<endl;

	return 0;
}
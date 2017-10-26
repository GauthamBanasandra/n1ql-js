#include <iostream>
#include <fstream>
#include "n1ql.h"

using namespace std;

int main()
{
	ifstream file("inputs/input3.txt");
	string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	const char* input=source_code.c_str();
	string plain_js_code;
	int ret_code = Jsify(input, &plain_js_code);

	switch(ret_code)
	{
	case kOK:
		cout << plain_js_code << endl << ret_code;
		break;
	case kKeywordSelect:
		cout << "error: select is a reserved word"<<endl<<ret_code;;
		break;
	case kKeywordCreate:
		cout << "error: create is a reserved word"<<endl<<ret_code;;
		break;
	case kKeywordUpsert:
		cout << "error: upsert is a reserved word"<<endl<<ret_code;;
		break;
	case kKeywordInsert:
		cout << "error: insert is a reserved word"<<endl<<ret_code;;
		break;
	case kKeywordDelete:
		cout << "error: delete is a reserved word"<<endl<<ret_code;;
		break;
	case kKeywordUpdate:
		cout << "error: update is a reserved word"<<endl<<ret_code;;
		break;
	case kKeywordMerge:
		cout << "error: merge is a reserved word"<<endl<<ret_code;;
		break;
	default:
		cout << "error: using a reserved word"<<endl<<ret_code;
	}

	return 0;
}
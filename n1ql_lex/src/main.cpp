#include <iostream>
#include <fstream>
#include <list>

#include "n1ql.h"

using namespace std;

int main()
{
	ifstream file("inputs/input6.js");
	string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	const char* input=source_code.c_str();
	string plain_js_code;
	std::list<Pos> n1ql_pos;
	int ret_code = CommentN1QL(input, &plain_js_code, &n1ql_pos);

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

	for(const auto &pos : n1ql_pos) {
		std::string type;
		switch(pos.type) {
			case pos_type::kN1QLBegin:
				type = "N1QL Begin";
				break;
			case pos_type::kN1QLEnd:
				type = "N1QL End";
				break;
		}

		std::cout << "Index: " << pos.index << "\tLine no: " << pos.line_no << "\tValue: " <<
		plain_js_code[pos.index] << "\tType: " << type << "\tLen: " << pos.type_len << '\n';
	}

	return 0;
}
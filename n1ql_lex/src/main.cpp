#include <iostream>
#include <fstream>
#include <list>

#include "n1ql.h"

using namespace std;

int main()
{
	ifstream file("inputs/input8.js");
	string source_code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	auto info = CommentN1QL(source_code);
	switch(info.code)
	{
	case kOK:
		cout << info.handler_code << endl << info.code;
		break;
	case kKeywordSelect:
		cout << "error: select is a reserved word"<< endl << info.code;
		break;
	case kKeywordCreate:
		cout << "error: create is a reserved word"<<endl<<info.code;;
		break;
	case kKeywordUpsert:
		cout << "error: upsert is a reserved word"<<endl<<info.code;;
		break;
	case kKeywordInsert:
		cout << "error: insert is a reserved word"<<endl<<info.code;;
		break;
	case kKeywordDelete:
		cout << "error: delete is a reserved word"<<endl<<info.code;;
		break;
	case kKeywordUpdate:
		cout << "error: update is a reserved word"<<endl<<info.code;;
		break;
	case kKeywordMerge:
		cout << "error: merge is a reserved word"<<endl<<info.code;;
		break;
	default:
		cout << "error: using a reserved word"<<endl<<info.code;
	}

//	cout << endl;
//	cout << info.handler_code << endl << endl;
//	for(auto i = 0; i < info.handler_code.length(); ++i) {
//		cout << i << " " << info.handler_code[i] << endl;
//	}

	cout << endl << "Last pos: Line no: " << info.last_pos.line_no << " Col no: " << info.last_pos.col_no << " Index: " << info.last_pos.index << " Value: " << info.handler_code[info.last_pos.index] << endl;
	for(const auto &pos : info.insertions) {
		std::string type;
		switch(pos.type) {
			case insert_type::kN1QLBegin:
				type = "N1QL Begin";
				break;
			case insert_type::kN1QLEnd:
				type = "N1QL End";
				break;
		}

		std::cout << "Index: " << pos.index << "\tLine no: " << pos.line_no << "\tValue: " <<
		info.handler_code[pos.index] << "\tType: " << type << "\tLen: " << pos.type_len << '\n';
	}

	return 0;
}
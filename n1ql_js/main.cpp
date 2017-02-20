#include "n1ql_js.h"

using namespace std;

int main(int argc, char *argv[])
{
    cin >> noskipws;
    istream_iterator<char> iterator(cin);
    istream_iterator<char> iterator_end;

    string source_code(iterator, iterator_end);

    cout << "input" << endl << source_code << endl << endl;

    ExecJs(source_code);

    return 0;
}
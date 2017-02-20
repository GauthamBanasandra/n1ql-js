#include "n1ql_js.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Reading from stdin.
    cin >> noskipws;
    istream_iterator<char> iterator(cin);
    istream_iterator<char> iterator_end;

    string source_code(iterator, iterator_end);

    // Execute js.
    ExecJs(source_code);

    return 0;
}
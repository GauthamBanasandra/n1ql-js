#include "n1ql_js.h"
#include "utils/Utils.h"

using namespace std;

int main(int argc, char *argv[])
{
    string source_code = N1qlUtils::ReadFile(argv[1]);

    ExecJs(source_code);

    return 0;
}
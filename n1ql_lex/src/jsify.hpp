#include <iostream>

enum op_code {OK, KWD_SELECT, KWD_CREATE, KWD_UPSERT, KWD_INSERT, KWD_DELETE, KWD_UPDATE, KWD_MERGE};
int parse(const char *, std::string *);

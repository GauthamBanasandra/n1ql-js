#include <string>

enum op_code {
  kOK,
  kKeywordAlter,
  kKeywordBuild,
  kKeywordCreate,
  kKeywordDelete,
  kKeywordDrop,
  kKeywordExecute,
  kKeywordExplain,
  kKeywordGrant,
  kKeywordInfer,
  kKeywordInsert,
  kKeywordMerge,
  kKeywordPrepare,
  kKeywordRename,
  kKeywordRevoke,
  kKeywordSelect,
  kKeywordUpdate,
  kKeywordUpsert
};

enum lex_op_code { kJsify, kUniLineN1QL };

int Jsify(const char* input, std::string *output);
int UniLineN1ql(const char *input, std::string *output);
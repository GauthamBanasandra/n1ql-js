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

enum lex_op_code { kJsify, kUniLineN1QL, kCommentN1QL };

// Insertion types for CommentN1QL
enum class insert_type { kN1QLBegin, kN1QLEnd };
// Keeps track of the type of literal inserted during CommentN1QL
struct InsertedCharsInfo {
  InsertedCharsInfo(insert_type type) : type(type), type_len(0), line_no(0), index(0) {}

  insert_type type;
  int type_len;
  unsigned long long line_no;
  unsigned long long index;
};


int Jsify(const char* input, std::string *output);
int UniLineN1ql(const char *input, std::string *output);
int CommentN1QL(const char *input, std::string *output, std::list<InsertedCharsInfo> *pos_out);

void HandleStrStart(int state);
void HandleStrStop(int state);
bool IsEsc();
void UpdatePos(insert_type type);
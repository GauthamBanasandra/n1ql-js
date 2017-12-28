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
  kKeywordFrom,
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
  InsertedCharsInfo(insert_type type)
  : type(type), type_len(0), line_no(0), index(0) {}

  insert_type type;
  int type_len;
  int64_t line_no;
  int64_t index;
};

// Represents position of each char in the source code
struct Pos {
  Pos() : line_no(0), col_no(0), index(0) {}

  int64_t line_no;
  int64_t col_no;
  int64_t index;
};

int Jsify(const char* input, std::string *output, Pos *last_pos_out);
int UniLineN1QL(const char *input, std::string *output, Pos *last_pos_out);
int CommentN1QL(const char *input, std::string *output, std::list<InsertedCharsInfo> *pos_out, Pos *last_pos_out);

void HandleStrStart(int state);
void HandleStrStop(int state);
bool IsEsc(const std::string &str);
void UpdatePos(insert_type type);
void UpdatePos(Pos *pos);

std::string TranspileQuery(const std::string &query);
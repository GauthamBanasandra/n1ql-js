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

struct JsifyInfo {
	int code;
	std::string handler_code;
	Pos last_pos;
};

struct UniLineN1QLInfo {
	int code;
	std::string handler_code;
	Pos last_pos;
};

struct CommentN1QLInfo {
	int code;
	std::string handler_code;
	std::list<InsertedCharsInfo> insertions;
	Pos last_pos;
};

JsifyInfo Jsify(const std::string &input);
UniLineN1QLInfo UniLineN1QL(const std::string &info);
CommentN1QLInfo CommentN1QL(const std::string &input);

void HandleStrStart(int state);
void HandleStrStop(int state);
bool IsEsc(const std::string &str);
void UpdatePos(insert_type type);
void UpdatePos(Pos *pos);
std::string TranspileQuery(const std::string &query);
void ReplaceRecentChar(std::string &str, char m, char n);

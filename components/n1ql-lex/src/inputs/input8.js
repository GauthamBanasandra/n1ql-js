function OnUpdate(doc,meta) {
var query =
FROM
`src_bucket`
LET
uname
=
UPPER(name)
SELECT
uname
LIMIT
10
;
query.execQuery();

var query = FROM `src_bucket`
LET uname = UPPER(name)
SELECT uname
LIMIT 10;
query.execQuery();
}
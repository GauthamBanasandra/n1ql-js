var k = 'key';
var v = '{"k" : "v"}';
// hello
var res = new N1qlQuery('UPSERT  INTO sample (KEY, VALUE) VALUES (:k, :v)');
// hi
res.execQuery();

// hello
var res = new N1qlQuery('UPSERT  INTO sample (KEY, VALUE) VALUES (:k, :v)');
// hi
res.execQuery();

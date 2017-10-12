var k = 'key';
var v = {"k" : "v"};
var res = new N1qlQuery('UPSERT  INTO sample (KEY, VALUE) VALUES (:k, :v)');
res.execQuery();

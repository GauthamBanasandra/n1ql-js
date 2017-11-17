function OnUpdate(doc, meta) {
  var lim = 10,
  count = 0;
  
  // Unlabeled break
  var res1 =   SELECT *
  FROM `beer-sample`
  LIMIT :lim;
  for(var row1 of res1) {
    var res2 = SELECT * FROM `beer-sample` LIMIT :lim;
    for(var row2 of res2) {
      var docId = meta.id + (++count);
      var ins = INSERT INTO dst_bucket (KEY, VALUE) VALUES (:docId, 'Hello world');
      ins.execQuery();
      break;
    }
  }
}

function OnDelete(meta) {
}

OnUpdate('hello', {id:'12321'});

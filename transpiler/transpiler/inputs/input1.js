function OnUpdate(doc, meta) {
  var lim = 10,
  count = 0;
  
  // Throw
  var res1 = new N1qlQuery('SELECT  * FROM `beer-sample` LIMIT :lim');
  for(var row1 of res1) {
    var res2 = new N1qlQuery('SELECT  * FROM `beer-sample` LIMIT :lim');
    try{
      for(var row2 of res2) {
        var res3 = new N1qlQuery('SELECT  * FROM `beer-sample` LIMIT :lim');
        for(var row3 of res3) {
          var docId = meta.id + (++count);
          var ins = new N1qlQuery('INSERT  INTO dst_bucket (KEY, VALUE) VALUES (:docId, \'Hello world\')');
          ins.execQuery();
          throw 'Error';
        }
      }
    } catch(e){
    }
  }
}

function OnDelete(meta) {
}

OnUpdate('ABCD', {id:'abcd1234'});

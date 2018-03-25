function OnUpdate(doc, meta) {
  var lim = 5,
  count = 0;
  
  try	{
    var res1 = SELECT * FROM `default` LIMIT $lim;
    for(var row1 of res1) {
      var res2 = SELECT * FROM `default` LIMIT $lim;
      for(var row2 of res2) {
        var res3 = SELECT * FROM `default` LIMIT $lim;
        for(var row3 of res3) {
          var res4 = SELECT * FROM `default` LIMIT $lim;
          for(var row4 of res4) {
            var res5 = SELECT * FROM `default` LIMIT $lim;
            for(var row5 of res5) {
              var docId = meta.id + (++count);
              var res6 = INSERT INTO `hello-world` (KEY, VALUE) VALUES ($docId, 'Hello world');
              res6.execQuery();
            }
          }
        }
      }
    }
  } catch (e) {
    log(e);
  }
}

OnUpdate({
         "name": "abc0",
         "age": 0,
         "index": 0,
         "body": "VTKGNKUHMP"
         }, {
         "id": "abc0",
         "rev": "3-14f82c94d47600000000000000000000",
         "expiration": 0,
         "flags": 0
         })

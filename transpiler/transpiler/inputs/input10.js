function OnUpdate(doc, meta) {
//  var query = INSERT INTO sample ( KEY, VALUE ) VALUES ("a", "b");
  var lim = 10;
  try {
    var query = SELECT * FROM `beer-sample` LIMIT :lim;
    query.execQuery();
  } catch(e) {
    log(e);
  }
}
function OnDelete(meta) {
}

OnUpdate(1, 2);

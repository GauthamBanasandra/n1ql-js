function OnUpdate(doc, meta) {
  var lim = 10;
  try {
    var query = SELECT * FROM `beer-sample` LIMIT $lim;
    query.execQuery();
  } catch(e) {
    log(e);
  }
}
function OnDelete(meta) {
}

function OnUpdate(doc, meta) {
  var query = INSERT INTO dst_bucket ( KEY, VALUE ) VALUES ($ABCD ,'@meta.id'), {ABCD:1};
  query.execQuery();
}
function OnDelete(meta) {
}

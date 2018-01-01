function OnUpdate(doc, meta) {
	var query = INSERT INTO sample
  ( KEY, VALUE ) VALUES ("a", "b")
;

  query.execQuery();
}
function OnDelete(meta) {
}

OnUpdate(1, 2);

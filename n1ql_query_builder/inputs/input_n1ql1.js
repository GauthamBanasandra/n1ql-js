function OnUpdate(doc, meta) {
	var bucket = '`beer-sample`',
		lim = 10;

	// Here's the N1QL query.
	var res = new N1qlQuery(`select * FROM :bucket WHERE LIMIT :lim`);
	for (var row of res) {
		log(row);
	}
}
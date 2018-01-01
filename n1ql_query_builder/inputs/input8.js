function name(params) {
	var name = 'Rochefort 10',
		lim = 10;
	var res = new N1qlQuery('FROM `src_bucket` LET uname = UPPER(name) SELECT uname LIMIT 10 ;');
	x:for (var row of res) {
		log(row);
		return;
	}
}
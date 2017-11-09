function func(params) {
	var name = 'Rochefort 10',
		lim = 10,
		address = undefined;
	var res = new N1qlQuery('SELECT  * FROM `beer-sample` WHERE address = :address;');

	for (var row of res) {
		log(row);
		log('\n');
	}

	log(res.query);
}

log(res.metadata);
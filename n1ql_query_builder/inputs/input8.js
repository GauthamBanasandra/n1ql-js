function name(params) {
	var name = 'Rochefort 10',
		lim = 10;
	var res = /*SELECT * FROM `beer-sample` LIMIT :lim;*/$;
	for (var row of res.x.y) {
		log(row);
	}
}
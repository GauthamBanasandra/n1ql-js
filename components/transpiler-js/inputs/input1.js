function OnUpdate(doc, meta) {
	var bucket = '`beer-sample`',
		city = 'Bangalore';
	var res = new N1qlQuery(`SELECT * FROM system:bucket WHERE city == "city"`);
}


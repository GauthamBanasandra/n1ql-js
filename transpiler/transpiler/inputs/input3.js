var name = 'Rochefort 10',
	lim = 10,
boolean_field = null;
var res = new N1qlQuery('SELECT  * FROM `beer-sample` WHERE boolean_field = NULL;');

for (var row of res) {
  log(row);
  log('\n');
}

log(res.query);
log(res.metadata);

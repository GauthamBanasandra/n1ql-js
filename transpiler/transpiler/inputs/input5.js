var name = 'Rochefort 10',
	lim = 10;
var res = new N1qlQuery('SELECT  * FROM `beer-sample` LIMIT :lim;');
for(var row of res) {
  log(row);
}

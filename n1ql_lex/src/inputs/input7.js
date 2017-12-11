var name = 'Rochefort 10',
lim = 10;
var res = SELECT *
					FROM 'abcd''' `beer-sample`
LIMIT :lim; 123abc;
for(var row of res) {
  log(row);
}

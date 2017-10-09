var name = 'Rochefort 10';
var lim = 10;
var res = new N1qlQuery('SELECT * FROM `beer-sample` WHERE name = :name LIMIT :lim;');

for (var row of res) {
  log(row['beer-sample']);
}

log(res.metadata);

